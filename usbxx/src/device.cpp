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

UINT DeviceBase::_usbxx_change_notification(ULONG new_state)
{
  return _devbase->on_change(new_state);
}

#include <threadxx/ring_buffer.hpp>

TXX::ring_buffer_base<int, 32> event_ring;

uint32_t DeviceBase::on_change(uint32_t new_state)
{
  if (new_state != UX_DCD_STM32_SOF_RECEIVED)
    event_ring.push(new_state);

  switch (new_state)
  {
    case UX_DEVICE_ATTACHED:
      return on_attached();
    case UX_DEVICE_REMOVED:
      return on_removed();
    case UX_DCD_STM32_DEVICE_CONNECTED:
      return on_connected();
    case UX_DCD_STM32_DEVICE_DISCONNECTED:
      return on_disconnected();
    case UX_DCD_STM32_DEVICE_SUSPENDED:
      return on_suspended();
    case UX_DCD_STM32_DEVICE_RESUMED:
      return on_resumed();
    case UX_DCD_STM32_SOF_RECEIVED:
      return on_sof();
    default:
      dbg::dbgout << "Unknown state: " << new_state << std::endl;
      return 0;
  }

}

void DeviceBase::setup_device()
{
  fs_desc.build();
  hs_desc.build();

  strings.add_string(USBD_IDX_MFC_STR, get_manufacturer());
  strings.add_string(USBD_IDX_PRODUCT_STR, get_product());
  strings.add_string(USBD_IDX_SERIAL_STR, get_serial());

  lang_ids.add_language();


  Interface              *interfaces_pool;
  UX_SLAVE_TRANSFER               *xfer;
  UINT                            status;
  ULONG                           interfaces_found;
  ULONG                           endpoints_found;
  #if !defined(UX_DEVICE_INITIALIZE_FRAMEWORK_SCAN_DISABLE)
  ULONG                           max_interface_number;
  ULONG                           local_interfaces_found;
  ULONG                           local_endpoints_found;
  ULONG                           endpoints_in_interface_found;
  const UCHAR                           *device_framework;
  ULONG                           device_framework_length;
  UCHAR                           descriptor_type;
  ULONG                           descriptor_length;
  #endif
  UCHAR                           *memory;

  /* Get the pointer to the device. */
  auto device = _ux_system_slave->device;

#if 0
  /* Store the high speed device framework address and length in the project structure.  */
  _ux_system_slave -> ux_system_slave_device_framework_high_speed =             device_framework_high_speed;
  _ux_system_slave -> ux_system_slave_device_framework_length_high_speed =      device_framework_length_high_speed;

  /* Store the string framework address and length in the project structure.  */
  _ux_system_slave -> ux_system_slave_device_framework_full_speed =             device_framework_full_speed;
  _ux_system_slave -> ux_system_slave_device_framework_length_full_speed =      device_framework_length_full_speed;

  /* Store the string framework address and length in the project structure.  */
  _ux_system_slave -> ux_system_slave_string_framework =                         string_framework;
  _ux_system_slave -> ux_system_slave_string_framework_length =                  string_framework_length;

  /* Store the language ID list in the project structure.  */
  _ux_system_slave -> ux_system_slave_language_id_framework =                 language_id_framework;
  _ux_system_slave -> ux_system_slave_language_id_framework_length =          language_id_framework_length;
#endif

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

  /* Acquire a buffer for the size of the endpoint.  */
  xfer -> data =
        (UCHAR *)::malloc(UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH);

  /* Ensure we have enough memory.  */
  if (xfer -> data == nullptr)
      status = UX_MEMORY_INSUFFICIENT;
  else
      status = UX_SUCCESS;

  ::memset(xfer -> data, 0, UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH);

#if defined(UX_DEVICE_INITIALIZE_FRAMEWORK_SCAN_DISABLE)

  /* No scan, just assign predefined value.  */
  interfaces_found = UX_MAX_SLAVE_INTERFACES;
  endpoints_found = UX_MAX_DEVICE_ENDPOINTS;
#else

  /* Reset all values we are using during the scanning of the framework.  */
  interfaces_found                   =  0;
  endpoints_found                    =  0;
  max_interface_number               =  0;

  /* Go on to scan interfaces if no error.  */
  if (status == UX_SUCCESS)
  {

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

          /* Error trap. */
          _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_INIT, UX_DESCRIPTOR_CORRUPTED);

          /* If trace is enabled, insert this event into the trace buffer.  */
          UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_DESCRIPTOR_CORRUPTED, device_framework, 0, 0, UX_TRACE_ERRORS, 0, 0)

          status = UX_DESCRIPTOR_CORRUPTED;
      }

      /* We do a sanity check on the finding. Max interface number should not exceed limit.  */
      if (status == UX_SUCCESS &&
          max_interface_number >= UX_MAX_SLAVE_INTERFACES)
      {

          /* Error trap. */
          _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_INIT, UX_MEMORY_INSUFFICIENT);

          /* If trace is enabled, insert this event into the trace buffer.  */
          UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_MEMORY_INSUFFICIENT, device_framework, 0, 0, UX_TRACE_ERRORS, 0, 0)

          status = UX_MEMORY_INSUFFICIENT;
      }
  }
#endif

  /* Go on to allocate interfaces pool if no error.  */
  if (status == UX_SUCCESS)
  {

      /* Memorize both pool sizes.  */
      device -> interfaces_pool_number = interfaces_found;
      device -> endpoints_pool_number  = endpoints_found;

      /* We assign a pool for the interfaces.  */
      interfaces_pool = (Interface*)::malloc(interfaces_found * sizeof(Interface));
      if (interfaces_pool == nullptr)
          status = UX_MEMORY_INSUFFICIENT;
      else
          /* Save the interface pool address in the device container.  */
          device -> interfaces_pool =  interfaces_pool;

      ::memset(interfaces_pool, 0, interfaces_found * sizeof(Interface));
  }

#if 0
  /* Install the device portion of USBX */
  if (ux_device_stack_initialize(hs_desc.get_desc(),
                                 hs_desc.get_desc_len(),
                                 fs_desc.get_desc(),
                                 fs_desc.get_desc_len(),
                                 strings.get_buffer(),
                                 strings.get_buffer_len(),
                                 lang_ids.get_buffer(),
                                 lang_ids.get_buffer_len(),
                                 _usbxx_change_notification) != UX_SUCCESS)
  {
    throw std::runtime_error("Failed to initialize USB device stack\n");
  }
#endif
}


void DeviceBase::start()
{
  try {
    start_system();
    setup_device();
    register_class();
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
Interface          *interface_ptr;
Interface          *next_interface;
UX_SLAVE_CLASS              *class_ptr;
UX_SLAVE_CLASS_COMMAND      class_command;

    /* Get the pointer to the device.  */
    auto device =  _ux_system_slave->device;


    /* If the device was in the configured state, there may be interfaces
       attached to the configuration.  */
    if (device -> state == UX_DEVICE_CONFIGURED)
    {
        /* Get the pointer to the first interface.  */
        interface_ptr =  device -> first_interface;

#if !defined(UX_DEVICE_INITIALIZE_FRAMEWORK_SCAN_DISABLE) || UX_MAX_DEVICE_INTERFACES > 1
        /* Parse all the interfaces if any.  */
        while (interface_ptr != nullptr)
        {
#endif

            /* Build all the fields of the Class Command.  */
            class_command.ux_slave_class_command_request =   UX_SLAVE_CLASS_COMMAND_DEACTIVATE;
            class_command.ux_slave_class_command_interface =  (VOID *) interface_ptr;

            /* Get the pointer to the class container of this interface.  */
            class_ptr =  interface_ptr -> usb_class;

            /* Store the class container. */
            class_command.ux_slave_class_command_class_ptr =  class_ptr;

            /* If there is a class container for this instance, deactivate it.  */
            if (class_ptr != nullptr)

                /* Call the class with the DEACTIVATE signal.  */
                class_ptr -> ux_slave_class_entry_function(&class_command);

#if !defined(UX_DEVICE_INITIALIZE_FRAMEWORK_SCAN_DISABLE) || UX_MAX_DEVICE_INTERFACES > 1
            /* Get the next interface.  */
            next_interface =  interface_ptr -> next_interface;
#endif

            /* Remove the interface and all endpoints associated with it.  */
            _ux_device_stack_interface_delete(interface_ptr);

#if !defined(UX_DEVICE_INITIALIZE_FRAMEWORK_SCAN_DISABLE) || UX_MAX_DEVICE_INTERFACES > 1
            /* Now we refresh the interface pointer.  */
            interface_ptr =  next_interface;
        }
#endif

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

UINT USBXX::_ux_device_stack_class_register(const std::string &class_name,
                        UINT (*class_entry_function)(UX_SLAVE_CLASS_COMMAND *),
                        ULONG configuration_number,
                        ULONG interface_number,
                        VOID *parameter)
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


