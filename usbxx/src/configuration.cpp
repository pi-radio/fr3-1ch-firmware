#include <usbxx/ux_api.h>
#include <usbxx/ux_device_stack.h>

#include <usbxx/device.hpp>

using namespace USBXX;

uint32_t DeviceBase::on_get_alternate_setting(ULONG interface_value)
{
  Transfer       *xfer;
  UINT                    status;

    /* If the device was in the configured state, there may be interfaces
       attached to the configuration.  */
    if (state != UX_DEVICE_CONFIGURED)
    {
      return UX_ERROR;
    }

    auto iface = find_interface(interface_value);

    if (iface == nullptr) {
      return UX_ERROR;
    }

    xfer =  get_control_transfer();
    *xfer -> data =
                (UCHAR) iface -> descriptor.bAlternateSetting;

    xfer -> requested_length =  1;

    xfer -> phase =  TransferPhase::DATA_OUT;
    status = transfer_request(xfer, 1, 1);

    return(status);
}

uint32_t  DeviceBase::on_set_alternate_setting(ULONG interface_value, ULONG alternate_setting_value)
{
const UCHAR                           *device_framework;
ULONG                           device_framework_length;
ULONG                           descriptor_length;
UCHAR                           descriptor_type;
ConfigurationDescriptor     configuration_descriptor;
InterfaceDescriptor         interface_descriptor;
Endpoint               *endpoint;
USBClass                  *class_ptr;
UINT                            status;

    /* Protocol error must be reported when it's unconfigured */
    if (state != UX_DEVICE_CONFIGURED)
        return(UX_FUNCTION_NOT_SUPPORTED);

    auto iface = find_interface(interface_value);

    /* We must have found the interface pointer for the interface value
       requested by the caller.  */
    if (iface == nullptr)
    {
      throw std::runtime_error("Unable to find interface");
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
        descriptor_length =  (ULONG) *device_framework;

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
                    descriptor_length =  (ULONG) *device_framework;

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
                            endpoint->used = false;
                            endpoint -> ux_slave_endpoint_state =  0;
                            endpoint -> ux_slave_endpoint_next_endpoint =  nullptr;
                            endpoint -> ux_slave_endpoint_interface =  nullptr;
                            endpoint -> ux_slave_endpoint_device =  nullptr;

                          }

                            /* Point beyond the interface descriptor.  */
                            device_framework_length -=  (ULONG) *device_framework;
                            device_framework +=  (ULONG) *device_framework;

                            /* Parse the device framework and locate endpoint descriptor(s).  */
                            while (device_framework_length != 0)
                            {

                                /* Get the length of the current descriptor.  */
                                descriptor_length =  (ULONG) *device_framework;

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
                                    endpoint = dcd->allocate_endpoint(iface, desc.bEndpointAddress);


                                    /* Create the endpoint at the DCD level.  */
                                    status = endpoint->create();

                                    /* Do a sanity check on endpoint creation.  */
                                    if (status != UX_SUCCESS)
                                    {

                                        /* Error was returned, endpoint cannot be created.  */
                                        endpoint->used = false;
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
                            ::memcpy(&iface -> descriptor, &interface_descriptor, sizeof(UX_INTERFACE_DESCRIPTOR)); /* Use case of memcpy is verified. */

                            /* Get the class for the interface.  */
                            class_ptr =  _ux_system_slave -> ux_system_slave_interface_class_array[iface -> descriptor.bInterfaceNumber];

                            /* Check if class driver is available. */
                            if (class_ptr == nullptr || class_ptr -> status == UX_UNUSED)
                            {

                                return (UX_NO_CLASS_MATCH);
                            }
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
  *xfer->data = (UCHAR) configuration_selected;

  /* Set the phase of the transfer to data out.  */
  xfer->phase =  TransferPhase::DATA_OUT;

  /* Send the descriptor with the appropriate length to the host.  */
  return transfer_request(xfer, 1, 1);
}

uint32_t  DeviceBase::on_set_configuration(uint32_t configuration_value)
{
const UCHAR *                         device_framework;
ULONG                           device_framework_length;
ULONG                           descriptor_length;
UCHAR                           descriptor_type;
ConfigurationDescriptor     configuration_descriptor = { 0 };
InterfaceDescriptor         interface_descriptor;
USBClass                  *class_inst;
USBClass                  *current_class =  nullptr;
ULONG                           iad_flag;
ULONG                           iad_first_interface =  0;
ULONG                           iad_number_interfaces =  0;
#if UX_MAX_SLAVE_CLASS_DRIVER > 1
ULONG                           class_index;
#endif


    /* Reset the IAD flag.  */
    iad_flag =  UX_FALSE;

    /* If the configuration value is already selected, keep it.  */
    if (configuration_selected == configuration_value)
        return 0;

    /* We may have multiple configurations !, the index will tell us what
       configuration descriptor we need to return.  */
    device_framework = get_current_descriptor().get_desc();
    device_framework_length = get_current_descriptor().get_desc_len();

    /* Parse the device framework and locate a configuration descriptor.  */
    while (device_framework_length != 0)
    {
        /* Get the length of the current descriptor.  */
        descriptor_length =  (ULONG) *device_framework;

        /* And its type.  */
        descriptor_type =  *(device_framework + 1);

        /* Check if this is a configuration descriptor.  */
        if (descriptor_type == UX_CONFIGURATION_DESCRIPTOR_ITEM)
        {
          configuration_descriptor = read_in_descriptor<ConfigurationDescriptor>(device_framework);

          /* Now we need to check the configuration value. It has
               to be the same as the one specified in the setup function.  */
            if (configuration_descriptor.bConfigurationValue == configuration_value)
                /* The configuration is found. */
                break;
        }

        /* Adjust what is left of the device framework.  */
        device_framework_length -= descriptor_length;
        /* Point to the next descriptor.  */
        device_framework += descriptor_length;
    }

    /* Configuration not found. */
    if (device_framework_length == 0 && configuration_value != 0)
        return(UX_ERROR);

    /* We unmount the configuration if there is previous configuration selected. */
    if (configuration_selected)
    {
      for (auto iface : interfaces) {
          class_inst =  iface -> usb_class;

          if (class_inst != nullptr)
              /*class_inst -> */ class_deactivate();

          iface->stop();
      }

      interfaces.clear();
    }

    /* No configuration is selected.  */
    configuration_selected =  0;

    /* Mark the device as attached now. */
    state =  UX_DEVICE_ATTACHED;

    /* The DCD needs to update the device state too.  */
    dcd->on_state_change(UX_DEVICE_ATTACHED);

    /* If the host tries to unconfigure, we are done. */
    if (configuration_value == 0)
        return 0;

    /* Memorize the configuration selected.  */
    configuration_selected =  configuration_value;

    configuration_descriptor = read_in_descriptor<ConfigurationDescriptor>(device_framework);

    /* Configuration character D6 is for Self-powered */
    _ux_system_slave -> ux_system_slave_power_state = (configuration_descriptor.bmAttributes & 0x40) ? UX_DEVICE_SELF_POWERED : UX_DEVICE_BUS_POWERED;

    /* Configuration character D5 is for Remote Wakeup */
    _ux_system_slave -> ux_system_slave_remote_wakeup_capability = (configuration_descriptor.bmAttributes & 0x20) ? UX_TRUE : UX_FALSE;

    /* Search only in current configuration */
    device_framework_length =  configuration_descriptor.wTotalLength;

    /*  We need to scan all the interface descriptors following this
        configuration descriptor and enable all endpoints associated
        with the default alternate setting of each interface.  */
    while (device_framework_length != 0)
    {

        /* Get the length of the current descriptor.  */
        descriptor_length =  (ULONG) *device_framework;

        /* And its type.  */
        descriptor_type =  *(device_framework + 1);

        /* Check if this is an interface association descriptor.  */
        if(descriptor_type == UX_INTERFACE_ASSOCIATION_DESCRIPTOR_ITEM)
        {

            /* Set the IAD flag.  */
            iad_flag = UX_TRUE;

            /* Get the first interface we have in the IAD. */
            iad_first_interface = (ULONG)  *(device_framework + 2);

            /* Get the number of interfaces we have in the IAD. */
            iad_number_interfaces = (ULONG)  *(device_framework + 3);
        }

        /* Check if this is an interface descriptor.  */
        if(descriptor_type == UX_INTERFACE_DESCRIPTOR_ITEM)
        {
          interface_descriptor = read_in_descriptor<InterfaceDescriptor>(device_framework);

            /* If the alternate setting is 0 for this interface, we need to
               memorize its class association and start it.  */
            if (interface_descriptor.bAlternateSetting == 0)
            {

                /* Are we in a IAD scenario ? */
                if (iad_flag == UX_TRUE)
                {

                    /* Check if this is the first interface from the IAD. In this case,
                       we need to match a class to this interface.  */
                    if (interface_descriptor.bInterfaceNumber == iad_first_interface)
                    {

                        /* First interface. Scan the list of classes to find a match.  */
                        class_inst =  _ux_system_slave -> ux_system_slave_class_array;

#if UX_MAX_SLAVE_CLASS_DRIVER > 1
                        /* Parse all the class drivers.  */
                        for (class_index = 0; class_index < _ux_system_slave -> ux_system_slave_max_class; class_index++)
                        {
#endif

                            /* Check if this class driver is used.  */
                            if (class_inst -> status == UX_USED)
                            {

                                /* Check if this is the same interface for the same configuration. */
                                if ((interface_descriptor.bInterfaceNumber == class_inst -> interface_number) &&
                                    (configuration_value == class_inst -> configuration_number))
                                {

                                    /* Memorize the class in the class/interface array.  */
                                    _ux_system_slave -> ux_system_slave_interface_class_array[interface_descriptor.bInterfaceNumber] = class_inst;

                                    /* And again as the current class.  */
                                    current_class = class_inst;

#if UX_MAX_SLAVE_CLASS_DRIVER > 1
                                    /* We are done here.  */
                                    break;
#endif
                                }
                            }

#if UX_MAX_SLAVE_CLASS_DRIVER > 1
                            /* Move to the next registered class.  */
                            class_inst ++;
                        }
#endif
                    }
                    else

                        /* Memorize the class in the class/interface array.  We use the current class. */
                        _ux_system_slave -> ux_system_slave_interface_class_array[interface_descriptor.bInterfaceNumber] = current_class;

                    /* Decrement the number of interfaces found in the same IAD.  */
                    iad_number_interfaces--;

                    /* If none are left, get out of the IAD state machine.  */
                    if (iad_number_interfaces == 0)

                        /* We have exhausted the interfaces within the IAD.  */
                        iad_flag = UX_FALSE;

                }
                else
                {

                    /* First interface. Scan the list of classes to find a match.  */
                    class_inst =  _ux_system_slave -> ux_system_slave_class_array;

#if UX_MAX_SLAVE_CLASS_DRIVER > 1
                    /* Parse all the class drivers.  */
                    for (class_index = 0; class_index < _ux_system_slave -> ux_system_slave_max_class; class_index++)
                    {
#endif

                        /* Check if this class driver is used.  */
                        if (class_inst -> status == UX_USED)
                        {

                            /* Check if this is the same interface for the same configuration. */
                            if ((interface_descriptor.bInterfaceNumber == class_inst -> interface_number) &&
                                    (configuration_value == class_inst -> configuration_number))
                            {

                                /* Memorize the class in the class/interface array.  */
                                _ux_system_slave -> ux_system_slave_interface_class_array[interface_descriptor.bInterfaceNumber] = class_inst;

#if UX_MAX_SLAVE_CLASS_DRIVER > 1
                                /* We are done here.  */
                                break;
#endif
                            }
                        }

#if UX_MAX_SLAVE_CLASS_DRIVER > 1
                        /* Move to the next registered class.  */
                        class_inst ++;
                    }
#endif
                }

                /* Set the interface.  */
                set_interface(device_framework, device_framework_length, 0);
            }
        }

        /* Adjust what is left of the device framework.  */
        device_framework_length -=  descriptor_length;

        /* Point to the next descriptor.  */
        device_framework +=  descriptor_length;
    }

    /* Mark the device as configured now. */
    state =  UX_DEVICE_CONFIGURED;

    /* The DCD needs to update the device state too.  */
    dcd->on_state_change(UX_DEVICE_CONFIGURED);

    /* Configuration mounted. */
    return 0;
}

