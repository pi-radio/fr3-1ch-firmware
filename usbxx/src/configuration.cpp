#include <cassert>

#include <usbxx/ux_api.h>

#include <usbxx/device.hpp>

using namespace USBXX;

uint32_t DeviceBase::on_get_alternate_setting(uint32_t interface_value)
{
  Transfer       *xfer;
  uint32_t                    status;

    /* If the device was in the configured state, there may be interfaces
       attached to the configuration.  */
    if (!is_configured())
    {
      return UX_ERROR;
    }

    auto iface = find_interface(interface_value);

    if (iface == nullptr) {
      return UX_ERROR;
    }

    xfer =  get_control_transfer();
    *xfer -> data =
                (uint8_t) iface -> descriptor.bAlternateSetting;

    xfer -> requested_length =  1;

    xfer -> phase =  TransferPhase::DATA_OUT;
    status = transfer_request(xfer, 1, 1);

    return(status);
}

uint32_t  DeviceBase::on_set_alternate_setting(uint32_t interface_value, uint32_t alternate_setting_value)
{
  const uint8_t                           *device_framework;
  uint32_t                           device_framework_length;
  uint32_t                           descriptor_length;
  uint8_t                           descriptor_type;
  ConfigurationDescriptor     configuration_descriptor;
  InterfaceDescriptor         interface_descriptor;
  USBClass                  *class_ptr;
  uint32_t                            status;

    /* Protocol error must be reported when it's unconfigured */
    if (!is_configured())
        return(UX_FUNCTION_NOT_SUPPORTED);

    auto iface = find_interface(interface_value);

    /* We must have found the interface pointer for the interface value
       requested by the caller.  */
    if (iface == nullptr)
    {
      throw USBXX::runtime_error("Unable to find interface");
    }

    /* If the host is requesting a change of alternate setting to the current one,
       we do not need to do any work.  */
    if (iface -> descriptor.bAlternateSetting == alternate_setting_value)
        return 0;

    /* We may have multiple configurations!  */
    device_framework = get_current_descriptor().get_desc();
    device_framework_length = get_current_descriptor().get_desc_len();

    /* Parse the device framework and locate a configuration descriptor. */
    while (device_framework_length != 0)
    {

        /* Get the length of the current descriptor.  */
        descriptor_length =  (uint32_t) *device_framework;

        /* And its length.  */
        descriptor_type = *(device_framework + 1);

        /* Check if this is a configuration descriptor. */
        if (descriptor_type == UX_CONFIGURATION_DESCRIPTOR_ITEM)
        {
            /* Parse the descriptor in something more readable. */
            configuration_descriptor = read_in_descriptor<ConfigurationDescriptor>(device_framework);

            /* Now we need to check the configuration value.  */
            if (configuration_descriptor.bConfigurationValue == configuration_selected)
            {

                /* Limit the search in current configuration descriptor. */
                device_framework_length = configuration_descriptor.wTotalLength;

                /* We have found the configuration value that was selected by the host
                   We need to scan all the interface descriptors following this
                   configuration descriptor and locate the interface for which the alternate
                   setting must be changed. */
                while (device_framework_length != 0)
                {

                    /* Get the length of the current descriptor.  */
                    descriptor_length =  (uint32_t) *device_framework;

                    /* And its type.  */
                    descriptor_type = *(device_framework + 1);

                    /* Check if this is an interface descriptor. */
                    if (descriptor_type == UX_INTERFACE_DESCRIPTOR_ITEM)
                    {
                      interface_descriptor = read_in_descriptor<InterfaceDescriptor>(device_framework);

                        /* Check if this is the interface we are searching. */
                        if (interface_descriptor.bInterfaceNumber == interface_value &&
                            interface_descriptor.bAlternateSetting == alternate_setting_value)
                        {

                            /* We have found the right interface and alternate setting. Before
                               we mount all the endpoints for this interface, we need to
                               unmount the endpoints associated with the previous alternate setting.  */
                          for(auto endpoint : iface->endpoints) {
                            endpoint->abort_all_transfers(UX_TRANSFER_BUS_RESET);

                            endpoint->destroy();

                            // TODO -- Add DCD callback to free endpoint
                          }

                            /* Point beyond the interface descriptor.  */
                            device_framework_length -=  (uint32_t) *device_framework;
                            device_framework +=  (uint32_t) *device_framework;

                            /* Parse the device framework and locate endpoint descriptor(s).  */
                            while (device_framework_length != 0)
                            {

                                /* Get the length of the current descriptor.  */
                                descriptor_length =  (uint32_t) *device_framework;

                                /* And its type.  */
                                descriptor_type =  *(device_framework + 1);

                                /* Check if this is an endpoint descriptor.  */
                                switch(descriptor_type)
                                {

                                case UX_ENDPOINT_DESCRIPTOR_ITEM:
                                {
                                  USBXX::EndpointDescriptor desc;

                                  desc = USBXX::read_in_descriptor<EndpointDescriptor>(device_framework);

                                    /* Find a free endpoint in the pool and hook it to the
                                       existing interface after it's created by DCD.  */
                                    auto endpoint = dcd->allocate_endpoint(iface, desc.bEndpointAddress);


                                    /* Create the endpoint at the DCD level.  */
                                    status = endpoint->create();

                                    /* Do a sanity check on endpoint creation.  */
                                    if (status != UX_SUCCESS)
                                    {

                                        /* Error was returned, endpoint cannot be created.  */
                                        // TODO -- Add free endpoint
                                        assert(0);
                                        return(status);
                                    }

                                    iface->endpoints.push_back(endpoint);
                                }
                                break;

                                case UX_CONFIGURATION_DESCRIPTOR_ITEM:
                                case UX_INTERFACE_DESCRIPTOR_ITEM:

                                    /* We have found a new configuration or interface descriptor, this is the end of the current
                                       interface. The search for the endpoints must be terminated as if it was the end of the
                                       entire descriptor.  */
                                    device_framework_length =  descriptor_length;

                                    break;


                                default:

                                    /* We have found another descriptor embedded in the interface. Ignore it.  */
                                    break;
                                }

                                /* Adjust what is left of the device framework.  */
                                device_framework_length -=  descriptor_length;

                                /* Point to the next descriptor.  */
                                device_framework +=  descriptor_length;
                            }

                            /* The interface descriptor in the current class must be changed to the new alternate setting.  */
                            iface->descriptor = interface_descriptor;

                            /* Get the class for the interface.  */
                            auto class_ptr = iface_to_class[iface -> descriptor.bInterfaceNumber];

                            /* Check if class driver is available. */
                            if (class_ptr == nullptr)
                                return (UX_NO_CLASS_MATCH);

                            /* We can now memorize the interface pointer associated with this class.  */
                            class_ptr -> interface = iface;

                            /* We have found a potential candidate. Call this registered class entry function to change the alternate setting.  */
                            status = /* class_ptr -> */ class_on_change();

                            /* We are done here.  */
                            return(status);
                        }
                    }

                    /* Adjust what is left of the device framework.  */
                    device_framework_length -=  descriptor_length;

                    /* Point to the next descriptor.  */
                    device_framework +=  descriptor_length;
                }

                /* In case alter setting not found, report protocol error. */
                break;
            }
        }

        /* Adjust what is left of the device framework.  */
        device_framework_length -=  descriptor_length;

        /* Point to the next descriptor.  */
        device_framework +=  descriptor_length;
    }

    /* Return error completion.  */
    return(UX_ERROR);
}

uint32_t DeviceBase::on_get_configuration()
{
  Transfer       *xfer;

  /* Get the pointer to the device.  */
  /* Get the pointer to the transfer request associated with the endpoint.  */
  xfer = get_control_transfer();

  /* Set the value of the configuration in the buffer.  */
  *xfer->data = (uint8_t) configuration_selected;

  /* Set the phase of the transfer to data out.  */
  xfer->phase =  TransferPhase::DATA_OUT;

  /* Send the descriptor with the appropriate length to the host.  */
  return transfer_request(xfer, 1, 1);
}

uint32_t  DeviceBase::on_set_configuration(uint32_t configuration_value)
{
  /* If the configuration value is already selected, keep it.  */
  if (configuration_selected == configuration_value)
      return 0;

  auto cur_desc = get_current_descriptor();

  auto di = cur_desc.begin();

  ConfigurationDescriptor config_desc;

  for(; di != cur_desc.end(); ++di) {
    if (di.type() != ConfigurationDescriptor::desc_type)
      continue;

    config_desc = di->read_in<ConfigurationDescriptor>();

    if (config_desc.bConfigurationValue == configuration_value) {
      break;
    }
  }

  if (di == cur_desc.end()) {
    return UX_ERROR;
  }

  /* We unmount the configuration if there is previous configuration selected. */
  if (configuration_selected)
  {
    for (auto iface : interfaces) {
        auto class_inst =  iface -> usb_class;

        if (class_inst != nullptr)
            /*class_inst -> */ class_deactivate();

        iface->stop();
    }

    interfaces.clear();
  }

  configuration_selected =  0;

  state = DeviceState::ATTACHED;

  dcd->on_state_change(DeviceState::ATTACHED);

  if (configuration_value == 0)
      return 0;

  configuration_selected =  configuration_value;

  /* Configuration character D6 is for Self-powered */
  _ux_system_slave -> ux_system_slave_power_state = (config_desc.bmAttributes & 0x40) ? UX_DEVICE_SELF_POWERED : UX_DEVICE_BUS_POWERED;

  /* Configuration character D5 is for Remote Wakeup */
  _ux_system_slave -> ux_system_slave_remote_wakeup_capability = (config_desc.bmAttributes & 0x20) ? true : false;

  di.trim(config_desc.wTotalLength);

  bool     iad_flag = false;
  uint32_t iad_first_interface =  0;
  uint32_t iad_number_interfaces =  0;
  InterfaceDescriptor interface_descriptor;
  USBClass::ptr current_class =  nullptr;


  while(di != cur_desc.end()) {
    /* Check if this is an interface association descriptor.  */
    if(di.type() == InterfaceAssociationDescriptor::desc_type)
    {
      auto iad_desc = di->read_in<InterfaceAssociationDescriptor>();

      iad_flag = true;
      iad_first_interface = iad_desc.bFirstInterface;
      iad_number_interfaces = iad_desc.bInterfaceCount;

      ++di;

      continue;
    }

    if (di.type() != InterfaceDescriptor::desc_type) {
      ++di;
      continue;
    }

    interface_descriptor = di->read_in<InterfaceDescriptor>();

    if (interface_descriptor.bAlternateSetting == 0)
    {
      if (iad_flag)
      {
        if (interface_descriptor.bInterfaceNumber == iad_first_interface)
        {
          for (auto class_inst : classes) {
            if ((interface_descriptor.bInterfaceNumber == class_inst -> interface_number) &&
                (configuration_value == class_inst -> configuration_number))
            {
              iface_to_class[interface_descriptor.bInterfaceNumber] = class_inst;
              current_class = class_inst;
              break;
            }
          }
        }
        else
          iface_to_class[interface_descriptor.bInterfaceNumber] = current_class;

        /* Decrement the number of interfaces found in the same IAD.  */
        iad_number_interfaces--;

        if (iad_number_interfaces == 0)
          iad_flag = false;
      }
      else
      {
        for (auto class_inst : classes)
        {
          if ((interface_descriptor.bInterfaceNumber == class_inst -> interface_number) &&
              (configuration_value == class_inst -> configuration_number))
          {
            iface_to_class[interface_descriptor.bInterfaceNumber] = class_inst;
            break;
          }
        }
      }

      set_interface(di, 0);
    }
  }

  /* Mark the device as configured now. */
  state = DeviceState::CONFIGURED;

  /* The DCD needs to update the device state too.  */
  dcd->on_state_change(DeviceState::CONFIGURED);

  /* Configuration mounted. */
  return 0;
}

