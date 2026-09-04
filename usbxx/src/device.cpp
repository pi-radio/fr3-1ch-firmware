/*
 * usbxx.cpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */
#include <stdexcept>
#include <cstring>
#include <cassert>

#include <usbxx/usbxx.hpp>

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>

#include <usb.h>

/* Includes ------------------------------------------------------------------*/
#include <usbxx/ux_device_descriptors.h>

UX_SYSTEM ux_system;
UX_SYSTEM *_ux_system = &ux_system;

using namespace USBXX;

/* Define USBX Host variable.  */
UX_SYSTEM_SLAVE ux_system_slave;
UX_SYSTEM_SLAVE *_ux_system_slave = &ux_system_slave;


void DeviceBase::thread_entry()
{
  dbg::dbgout << "Initializing USB Hardware" << std::endl;

  dcd->initialize();

  // Call app thread, if desired

  dbg::dbgout << "Hardware initialized" << std::endl;
}


DeviceBase *_devbase = NULL;

DeviceBase::DeviceBase() : fs_desc(USBD_FULL_SPEED), hs_desc(USBD_HIGH_SPEED)
{
  ::memset(&ux_system_slave, 0, sizeof(ux_system_slave));
  _devbase = this;
  _ux_system_slave->device = this;
}

#include <threadxx/ring_buffer.hpp>

TXX::ring_buffer_base<int, 32> event_ring;

void DeviceBase::setup_device()
{
  fs_desc.build();
  hs_desc.build();

  strings.add_string(USBD_IDX_MFC_STR, get_manufacturer());
  strings.add_string(USBD_IDX_PRODUCT_STR, get_product());
  strings.add_string(USBD_IDX_SERIAL_STR, get_serial());

  lang_ids.add_language();

  Transfer               *xfer;
  ULONG                           interfaces_found;
  ULONG                           endpoints_found;
  ULONG                           max_interface_number;
  ULONG                           local_interfaces_found;
  ULONG                           local_endpoints_found;
  ULONG                           endpoints_in_interface_found;
  const UCHAR                     *device_framework;
  ULONG                           device_framework_length;
  UCHAR                           descriptor_type;
  ULONG                           descriptor_length;
  UCHAR                           *memory;

  /* Get the pointer to the device. */
  auto device = _ux_system_slave->device;

  /* Store the max number of slave class drivers in the project structure.  */
  UX_SYSTEM_DEVICE_MAX_CLASS_SET(UX_MAX_SLAVE_CLASS_DRIVER);

  /* Allocate memory for the classes.
   * sizeof(UX_SLAVE_CLASS) * UX_MAX_SLAVE_CLASS_DRIVER) overflow is checked
   * outside of the function.
   */
  memory = (uint8_t *)::malloc(sizeof(UX_SLAVE_CLASS) * UX_MAX_SLAVE_CLASS_DRIVER);
  if (memory == nullptr)
    throw std::runtime_error("Unable to allocate room for class entries");

  ::memset(classes, 0, sizeof(UX_SLAVE_CLASS) * UX_MAX_SLAVE_CLASS_DRIVER);

  /* Save this memory allocation in the USBX project.  */
  _ux_system_slave -> ux_system_slave_class_array = classes;

  /* Allocate some memory for the Control Endpoint.  First get the address of the transfer request for the
     control endpoint. */
  xfer = device->get_control_transfer();

  ::memset(xfer -> data, 0, UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH);

  interfaces_found                   =  0;
  endpoints_found                    =  0;
  max_interface_number               =  0;

 /* We need to determine the maximum number of interfaces and endpoints declared in the device framework.
  This mechanism requires that both framework behave the same way regarding the number of interfaces
  and endpoints.  */
  device_framework        = fs_desc.get_desc();
  device_framework_length = fs_desc.get_desc_len();

  /* Reset all values we are using during the scanning of the framework.  */
  local_interfaces_found             =  0;
  local_endpoints_found              =  0;
  endpoints_in_interface_found       =  0;

  /* Parse the device framework and locate interfaces and endpoint descriptor(s).  */
  while (device_framework_length != 0)
  {

      /* Get the length of this descriptor.  */
      descriptor_length =  (ULONG) *device_framework;

      /* And its type.  */
      descriptor_type =  *(device_framework + 1);

      /* Check if this is an endpoint descriptor.  */
      switch(descriptor_type)
      {

      case UX_INTERFACE_DESCRIPTOR_ITEM:

          /* Check if this is alternate setting 0. If not, do not add another interface found.
          If this is alternate setting 0, reset the endpoints count for this interface.  */
          if (*(device_framework + 3) == 0)
          {

              /* Add the cumulated number of endpoints in the previous interface.  */
              local_endpoints_found += endpoints_in_interface_found;

              /* Read the number of endpoints for this alternate setting.  */
              endpoints_in_interface_found = (ULONG) *(device_framework + 4);

              /* Increment the number of interfaces found in the current configuration.  */
              local_interfaces_found++;
          }
          else
          {

              /* Compare the number of endpoints found in this non 0 alternate setting.  */
              if (endpoints_in_interface_found < (ULONG) *(device_framework + 4))

                  /* Adjust the number of maximum endpoints in this interface.  */
                  endpoints_in_interface_found = (ULONG) *(device_framework + 4);
          }

          /* Check and update max interface number.  */
          if (*(device_framework + 2) > max_interface_number)
              max_interface_number = *(device_framework + 2);

          break;

      case UX_CONFIGURATION_DESCRIPTOR_ITEM:

          /* Check if the number of interfaces found in this configuration is the maximum so far. */
          if (local_interfaces_found > interfaces_found)

              /* We need to adjust the number of maximum interfaces.  */
              interfaces_found =  local_interfaces_found;

          /* We have a new configuration. We need to reset the number of local interfaces. */
          local_interfaces_found =  0;

          /* Add the cumulated number of endpoints in the previous interface.  */
          local_endpoints_found += endpoints_in_interface_found;

          /* Check if the number of endpoints found in the previous configuration is the maximum so far. */
          if (local_endpoints_found > endpoints_found)

              /* We need to adjust the number of maximum endpoints.  */
              endpoints_found =  local_endpoints_found;

          /* We have a new configuration. We need to reset the number of local endpoints. */
          local_endpoints_found         =  0;
          endpoints_in_interface_found  =  0;

          break;

      default:
          break;
      }

      /* Adjust what is left of the device framework.  */
      device_framework_length -=  descriptor_length;

      /* Point to the next descriptor.  */
      device_framework +=  descriptor_length;
  }

  /* Add the cumulated number of endpoints in the previous interface.  */
  local_endpoints_found += endpoints_in_interface_found;

  /* Check if the number of endpoints found in the previous interface is the maximum so far. */
  if (local_endpoints_found > endpoints_found)

      /* We need to adjust the number of maximum endpoints.  */
      endpoints_found =  local_endpoints_found;


  /* Check if the number of interfaces found in this configuration is the maximum so far. */
  if (local_interfaces_found > interfaces_found)

      /* We need to adjust the number of maximum interfaces.  */
      interfaces_found =  local_interfaces_found;

  /* We do a sanity check on the finding. At least there must be one interface but endpoints are
  not necessary.  */
  if (interfaces_found == 0)
  {
    throw std::runtime_error("Corrupted descriptor");
  }
}


void DeviceBase::start()
{
  try {
    start_system();
    setup_device();
    class_init();
    start_app();
  } catch(std::runtime_error &e) {
    dbg::dbgout << "Exception in starting USB device: " << e.what() << std::endl;
    __asm volatile ("BKPT     %0" : : "i"(0));
  } catch (...) {
    dbg::dbgout << "Unknown exception in starting USB device!" << std::endl;
    __asm volatile ("BKPT     %0" : : "i"(0));
  }
}

void DeviceBase::disconnect()
{
  UX_SLAVE_CLASS              *class_ptr;
  UX_SLAVE_CLASS_COMMAND      class_command;

    /* Get the pointer to the device.  */
    auto device =  _ux_system_slave->device;


    /* If the device was in the configured state, there may be interfaces
       attached to the configuration.  */
    if (device -> state == UX_DEVICE_CONFIGURED)
    {
        /* Get the pointer to the first interface.  */
        for (auto iface : interfaces) {
          class_command.ux_slave_class_command_request =   UX_SLAVE_CLASS_COMMAND_DEACTIVATE;
          class_command.ux_slave_class_command_interface =  iface;

          /* Get the pointer to the class container of this interface.  */
          class_ptr =  iface -> usb_class;

          /* Store the class container. */
          class_command.ux_slave_class_command_class_ptr =  class_ptr;

          /* If there is a class container for this instance, deactivate it.  */
          if (class_ptr != nullptr)
              class_ptr -> ux_slave_class_entry_function(&class_command);

          iface->stop();
      }

      /* Mark the device as attached now.  */
      device -> state =  UX_DEVICE_ATTACHED;
    }

    /* If the device was attached, we need to destroy the control endpoint.  */
    if (device->state == UX_DEVICE_ATTACHED)
      device->get_control_endpoint()->destroy();

    /* We are reverting to configuration 0.  */
    device -> configuration_selected =  0;

    /* Set the device to be non attached.  */
    device -> state =  UX_DEVICE_RESET;

    device->on_removed();
}

uint32_t DeviceBase::register_class(const std::string &class_name,
                        uint32_t (*class_entry_function)(UX_SLAVE_CLASS_COMMAND *),
                        uint32_t configuration_number,
                        uint32_t interface_number,
                        void *parameter)
{

UX_SLAVE_CLASS              *class_inst;
UINT                        status;
UX_SLAVE_CLASS_COMMAND      command;
#if UX_MAX_SLAVE_CLASS_DRIVER > 1
ULONG                       class_index;
#endif

    /* Get first class.  */
    class_inst =  _ux_system_slave -> ux_system_slave_class_array;

#if UX_MAX_SLAVE_CLASS_DRIVER > 1
    /* We need to parse the class table to find an empty spot.  */
    for (class_index = 0; class_index < _ux_system_slave -> ux_system_slave_max_class; class_index++)
    {
#endif

        /* Check if this class is already used.  */
        if (class_inst -> ux_slave_class_status == UX_UNUSED)
        {

#if defined(UX_NAME_REFERENCED_BY_POINTER)
            class_inst -> ux_slave_class_name = (const UCHAR *)class_name;
#else
            /* We have found a free container for the class. Copy the name (with null-terminator).  */
           class_inst->name = class_name;
#endif

            /* Memorize the entry function of this class.  */
            class_inst -> ux_slave_class_entry_function =  class_entry_function;

            /* Memorize the pointer to the application parameter.  */
            class_inst -> ux_slave_class_interface_parameter =  parameter;

            /* Memorize the configuration number on which this instance will be called.  */
            class_inst -> ux_slave_class_configuration_number =  configuration_number;

            /* Memorize the interface number on which this instance will be called.  */
            class_inst -> ux_slave_class_interface_number =  interface_number;

            /* Build all the fields of the Class Command to initialize the class.  */
            command.ux_slave_class_command_request    =  UX_SLAVE_CLASS_COMMAND_INITIALIZE;
            command.ux_slave_class_command_parameter  =  parameter;
            command.ux_slave_class_command_class_ptr  =  class_inst;

            /* Call the class initialization routine.  */
            status = class_entry_function(&command);

            /* Check the status.  */
            if (status != UX_SUCCESS)
                return(status);

            /* Make this class used now.  */
            class_inst -> ux_slave_class_status = UX_USED;

            /* Return successful completion.  */
            return 0;
        }

#if UX_MAX_SLAVE_CLASS_DRIVER > 1
        /* Move to the next class.  */
        class_inst ++;
    }
#endif

    /* No more entries in the class table.  */
    return(UX_MEMORY_INSUFFICIENT);
}

uint32_t DeviceBase::get_interface(uint8_t interface_value)
{

Transfer       *xfer;
Endpoint       *endpoint;
uint32_t                    retval;

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_INTERFACE_GET, interface_value, 0, 0, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)


    /* Get the pointer to the device.  */
    auto device = _ux_system_slave->device;

    /* Get the control endpoint for the device.  */
    endpoint = device->get_control_endpoint();

    /* If the device was in the configured state, there may be interfaces
       attached to the configuration.  */
    if (device -> state == UX_DEVICE_CONFIGURED)
    {
      for (auto iface : interfaces) {
        if (iface -> descriptor.bInterfaceNumber == interface_value)
          xfer = get_control_transfer();

        /* Set the value of the alternate setting in the buffer.  */
        *xfer -> data =
            (UCHAR) iface -> descriptor.bAlternateSetting;

        /* Setup the length appropriately.  */
        xfer -> requested_length =  1;

        /* Set the phase of the transfer to data out.  */
        xfer -> phase =  TransferPhase::DATA_OUT;

        /* Send the descriptor with the appropriate length to the host.  */
        retval = dcd->transfer_request(xfer);

        /* Return the function status code.  */
        return(retval);
      }
    }

    /* The alternate setting value was not found, so we return a stall error.  */
    endpoint->stall();

    /* Return the status to the caller.  */
    return(UX_ERROR);
}

void DeviceBase::uninitialize(void)
{
  Transfer               *xfer;
    /* Get the pointer to the device. */
    auto device =  _ux_system_slave->device;

    /* Free class memory. */
    ::free(_ux_system_slave -> ux_system_slave_class_array);

    /* Allocate some memory for the Control Endpoint.  First get the address of the transfer request for the
       control endpoint. */
    xfer = device->get_control_transfer();

    /* Free memory for the control endpoint buffer.  */
    ::free(xfer -> data);


    // TODO -- RELEASE ALL ENDPOINTS
}


uint32_t DeviceBase::set_feature(uint32_t request_type, uint32_t request_value, uint32_t request_index)
{
  Endpoint       *endpoint;

    /* Get the control endpoint for the device.  */
    endpoint = get_control_endpoint();

    /* The feature can be for either the device or the endpoint.  */
    switch (request_type & UX_REQUEST_TARGET)
    {

    case UX_REQUEST_TARGET_DEVICE:

        /* Check if we have a DEVICE_REMOTE_WAKEUP Feature.  */
        if (request_value == UX_REQUEST_FEATURE_DEVICE_REMOTE_WAKEUP)
        {

            /* Check if we have the capability. */
            if (_ux_system_slave -> ux_system_slave_remote_wakeup_capability)
            {

                /* Enable the feature. */
                _ux_system_slave -> ux_system_slave_remote_wakeup_enabled = UX_TRUE;

                /* OK. */
                return (UX_SUCCESS);
            }
            else

                /* Protocol error. */
                return (UX_FUNCTION_NOT_SUPPORTED);
        }

#ifdef UX_OTG_SUPPORT
        /* Check if we have a A_HNP_SUPPORT Feature. This is set when the Host is HNP capable. */
        if (request_value == UX_OTG_FEATURE_A_HNP_SUPPORT)
        {

            /* Store the A_HNP_SUPPORT flag.  */
            _ux_system_otg -> ux_system_otg_slave_set_feature_flag |= UX_OTG_FEATURE_A_HNP_SUPPORT;

            /* OK.  */
            return 0;
        }

        /* Check if the host asks us to perform HNP.  If also we become the host.  */
        if (request_value == UX_OTG_FEATURE_B_HNP_ENABLE)
        {

            /* The ISR will pick up the suspend event and check if we need to become IDLE or HOST.  */
            _ux_system_otg -> ux_system_otg_slave_set_feature_flag |= UX_OTG_FEATURE_B_HNP_ENABLE;

            /* OK.  */
            return 0;
        }
#endif

        /* Request value not supported.  */
        return(UX_FUNCTION_NOT_SUPPORTED);

    case UX_REQUEST_TARGET_ENDPOINT:

        /* The only set feature for endpoint is ENDPOINT_STALL. This forces
           the endpoint to the stall situation.
           We need to find the endpoint through the interface(s). */
      for (auto iface : interfaces) {
            /* Get the first endpoint for this interface.  */
          for (auto endpoint_target : iface->endpoints) {
            if (endpoint_target -> ux_slave_endpoint_descriptor.bEndpointAddress == request_index)
            {
              endpoint_target->stall();
              return 0;
            }
          }

        }

        /* We get here when the endpoint is wrong. Should not happen though.  */
        /* Intentionally fall through into the default case. */
        /* fall through */
    default:

        /* We stall the command.  */
        endpoint->stall();

        /* No more work to do here.  The command failed but the upper layer does not depend on it.  */
        return 0;
    }
}

uint32_t DeviceBase::set_interface(const uint8_t * device_framework, uint32_t device_framework_length,
    uint32_t alternate_setting_value)
{
Endpoint       *endpoint;
ULONG                   descriptor_length;
UCHAR                   descriptor_type;
UINT                    status;

    /* Get the pointer to the device.  */
    auto device = _ux_system_slave ->device;

    auto dcd = device->get_dcd();

    /* Find a free interface in the pool and hook it to the
       existing interface.  */

    interfaces.push_back(std::make_shared<Interface>(this));

    auto iface = interfaces.back();

    /* Mark this interface as used now.  */
    iface -> status = UX_USED;

    iface -> descriptor = read_in_descriptor<InterfaceDescriptor>(device_framework);

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
          EndpointDescriptor desc = USBXX::read_in_descriptor<EndpointDescriptor>(device_framework);
            /* Find a free endpoint in the pool and hook it to the
               existing interface after it's created by DCD.  */

          endpoint = dcd->allocate_endpoint(iface, desc);

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

            /* If the descriptor is a configuration or interface,
               we have parsed and mounted all endpoints.
               The interface attached to this configuration must be started at the class level.  */
          iface->start();

            /* Return the status to the caller.  */
            return(status);

        default:
            break;
        }

        /* Adjust what is left of the device framework.  */
        device_framework_length -=  descriptor_length;

        /* Point to the next descriptor.  */
        device_framework +=  descriptor_length;
    }

    /* The interface attached to this configuration must be started at the class
       level.  */
    iface->start();

    /* Return the status to the caller.  */
    return(status);
}


uint32_t  DeviceBase::clear_feature(uint32_t request_type, uint32_t request_value, uint32_t request_index)
{
  Endpoint       *endpoint;

    UX_PARAMETER_NOT_USED(request_value);

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_CLEAR_FEATURE, request_type, request_value, request_index, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)

    /* Get the pointer to the device.  */
    auto device = _ux_system_slave->device;

    /* Get the control endpoint for the device.  */
    endpoint = device->get_control_endpoint();

    /* The request can be for either the device or the endpoint.  */
    switch (request_type & UX_REQUEST_TARGET)
    {

    case UX_REQUEST_TARGET_DEVICE:

        /* Check if we have a DEVICE_REMOTE_WAKEUP Feature.  */
        if (request_value == UX_REQUEST_FEATURE_DEVICE_REMOTE_WAKEUP)
        {

            /* Check if we have the capability. */
            if (_ux_system_slave -> ux_system_slave_remote_wakeup_capability)
            {

                /* Disable the feature. */
                _ux_system_slave -> ux_system_slave_remote_wakeup_enabled = UX_FALSE;
            }

            else

                /* Protocol error. */
                return (UX_FUNCTION_NOT_SUPPORTED);
        }

        break;

    case UX_REQUEST_TARGET_ENDPOINT:

        /* The only clear feature for endpoint is ENDPOINT_STALL. This clears
           the endpoint of the stall situation and resets its data toggle.
           We need to find the endpoint through the interface(s). */
      for(auto iface : interfaces) {
          for (auto endpoint_target : iface->endpoints) {
              if (endpoint_target -> ux_slave_endpoint_descriptor.bEndpointAddress != request_index)
                continue;

              endpoint_target->reset();
              endpoint_target -> ux_slave_endpoint_state = UX_ENDPOINT_RESET;
              return 0;
            }
      }

        /* Intentional fallthrough and go into the default case. */
        /* fall through */

    /* We get here when the endpoint is wrong. Should not happen though.  */
    default:

        /* We stall the command.  */
      endpoint->stall();

        /* No more work to do here.  The command failed but the upper layer does not depend on it.  */
        return 0;
    }

    /* Return the function status.  */
    return 0;
}
