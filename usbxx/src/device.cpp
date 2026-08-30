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
  _devbase = this;
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
      return UX_SUCCESS;
  }

}

void DeviceBase::setup_device()
{
#if 0
  UCHAR *device_framework_high_speed;
  UCHAR *device_framework_full_speed;
  ULONG device_framework_hs_length;
  ULONG device_framework_fs_length;
#endif

  ULONG string_framework_length;
  ULONG language_id_framework_length;
  UCHAR *string_framework;
  UCHAR *language_id_framework;

  fs_desc.build();
  hs_desc.build();

  strings.add_string(USBD_IDX_MFC_STR, get_manufacturer());
  strings.add_string(USBD_IDX_PRODUCT_STR, get_product());
  strings.add_string(USBD_IDX_SERIAL_STR, get_serial());

  lang_ids.add_language();

#if 0
  /* Get Language Id Framework and get the length */
  language_id_framework = USBD_Get_Language_Id_Framework(&language_id_framework_length);
#endif

#if 0
  /* Install the device portion of USBX */
  if (ux_device_stack_initialize(device_framework_high_speed,
                                 device_framework_hs_length,
                                 device_framework_full_speed,
                                 device_framework_fs_length,
                                 string_framework,
                                 string_framework_length,
                                 language_id_framework,
                                 language_id_framework_length,
                                 _usbxx_change_notification) != UX_SUCCESS)
#else
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
#endif
  {
    throw std::runtime_error("Failed to initialize USB device stack\n");
  }

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
  } catch (...) {
    dbg::dbgout << "Unknown exception in starting USB device!" << std::endl;
  }
}


UINT  _ux_device_stack_class_register(UCHAR *class_name,
                        UINT (*class_entry_function)(struct UX_SLAVE_CLASS_COMMAND_STRUCT *),
                        ULONG configuration_number,
                        ULONG interface_number,
                        VOID *parameter)
{

UX_SLAVE_CLASS              *class_inst;
UINT                        status;
UX_SLAVE_CLASS_COMMAND      command;
UINT                        class_name_length =  0;
#if UX_MAX_SLAVE_CLASS_DRIVER > 1
ULONG                       class_index;
#endif

  if(::strnlen((const char *)class_name, UX_MAX_CLASS_NAME_LENGTH) == UX_MAX_CLASS_NAME_LENGTH) {
    throw std::runtime_error("Class name too long");
  }

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
            ::memcpy(class_inst -> ux_slave_class_name, class_name, class_name_length + 1);
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
            return(UX_SUCCESS);
        }

#if UX_MAX_SLAVE_CLASS_DRIVER > 1
        /* Move to the next class.  */
        class_inst ++;
    }
#endif

    /* No more entries in the class table.  */
    return(UX_MEMORY_INSUFFICIENT);
}


