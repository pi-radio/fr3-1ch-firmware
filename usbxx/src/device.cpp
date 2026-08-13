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

#include <ux_api.h>

#include <usb.h>

extern "C" {
/* Includes ------------------------------------------------------------------*/
#include "ux_device_descriptors.h"
}


void USBXX::DeviceBase::thread_entry()
{
  dbg::dbgout << "Initializing USB Hardware" << std::endl;

  MX_USB_PCD_Init();
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x00 , PCD_SNG_BUF, 0x40);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x80 , PCD_SNG_BUF, 0x80);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x01, PCD_SNG_BUF, 0xC0);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x81, PCD_SNG_BUF, 0x100);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x82, PCD_SNG_BUF, 0x140);
  ux_dcd_stm32_initialize((ULONG)USB_DRD_FS, (ULONG)&hpcd_USB_DRD_FS);
  HAL_PCD_Start(&hpcd_USB_DRD_FS);

  // Call app thread, if desired

  dbg::dbgout << "Hardware initialized" << std::endl;
}


USBXX::DeviceBase *_devbase = NULL;

USBXX::DeviceBase::DeviceBase() : fs_desc(USBD_FULL_SPEED), hs_desc(USBD_HIGH_SPEED)
{
  _devbase = this;
}

UINT USBXX::_usbxx_change_notification(ULONG new_state)
{
  return _devbase->on_change(new_state);
}

#include <threadxx/ring_buffer.hpp>

TXX::ring_buffer_base<int, 32> event_ring;

uint32_t USBXX::DeviceBase::on_change(uint32_t new_state)
{
  if (new_state != UX_DCD_STM32_SOF_RECEIVED)
    event_ring.pushc(new_state);

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

void USBXX::DeviceBase::setup_device()
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


void USBXX::DeviceBase::start()
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


#if 0
UINT USBXX::DeviceBase::initialize(UCHAR * device_framework_high_speed, ULONG device_framework_length_high_speed,
                                   UCHAR * device_framework_full_speed, ULONG device_framework_length_full_speed,
                                   UCHAR * string_framework, ULONG string_framework_length,
                                   UCHAR * language_id_framework, ULONG language_id_framework_length,
                                   UINT (*ux_system_slave_change_function)(ULONG))
{
UX_SLAVE_ENDPOINT               *endpoints_pool;
UX_SLAVE_INTERFACE              *interfaces_pool;
UX_SLAVE_TRANSFER               *transfer_request;
UINT                            status;
ULONG                           interfaces_found;
ULONG                           endpoints_found;
#if !defined(UX_DEVICE_INITIALIZE_FRAMEWORK_SCAN_DISABLE)
ULONG                           max_interface_number;
ULONG                           local_interfaces_found;
ULONG                           local_endpoints_found;
ULONG                           endpoints_in_interface_found;
UCHAR                           *device_framework;
ULONG                           device_framework_length;
UCHAR                           descriptor_type;
ULONG                           descriptor_length;
#endif
UCHAR                           *memory;

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_INITIALIZE, 0, 0, 0, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)

    /* Get the pointer to the device. */
    device =  &_ux_system_slave -> ux_system_slave_device;

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

    /* Store the max number of slave class drivers in the project structure.  */
    UX_SYSTEM_DEVICE_MAX_CLASS_SET(UX_MAX_SLAVE_CLASS_DRIVER);

    /* Store the device state change function callback.  */
    _ux_system_slave -> ux_system_slave_change_function =  ux_system_slave_change_function;

    /* Allocate memory for the classes.
     * sizeof(UX_SLAVE_CLASS) * UX_MAX_SLAVE_CLASS_DRIVER) overflow is checked
     * outside of the function.
     */
    memory =  _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, sizeof(UX_SLAVE_CLASS) * UX_MAX_SLAVE_CLASS_DRIVER);
    if (memory == UX_NULL)
        return(UX_MEMORY_INSUFFICIENT);

    /* Save this memory allocation in the USBX project.  */
    _ux_system_slave -> ux_system_slave_class_array =  (UX_SLAVE_CLASS *) ((void *) memory);

    /* Allocate some memory for the Control Endpoint.  First get the address of the transfer request for the
       control endpoint. */
    transfer_request =  &device -> ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;

    /* Acquire a buffer for the size of the endpoint.  */
    transfer_request -> ux_slave_transfer_request_data_pointer =
          _ux_utility_memory_allocate(UX_NO_ALIGN, UX_CACHE_SAFE_MEMORY, UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH);

    /* Ensure we have enough memory.  */
    if (transfer_request -> ux_slave_transfer_request_data_pointer == UX_NULL)
        status = UX_MEMORY_INSUFFICIENT;
    else
        status = UX_SUCCESS;

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
        device_framework        =  _ux_system_slave -> ux_system_slave_device_framework_full_speed;
        device_framework_length =  _ux_system_slave -> ux_system_slave_device_framework_length_full_speed;

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
        device -> ux_slave_device_interfaces_pool_number = interfaces_found;
        device -> ux_slave_device_endpoints_pool_number  = endpoints_found;

        /* We assign a pool for the interfaces.  */
        interfaces_pool =  _ux_utility_memory_allocate_mulc_safe(UX_NO_ALIGN, UX_REGULAR_MEMORY, interfaces_found, sizeof(UX_SLAVE_INTERFACE));
        if (interfaces_pool == UX_NULL)
            status = UX_MEMORY_INSUFFICIENT;
        else

            /* Save the interface pool address in the device container.  */
            device -> ux_slave_device_interfaces_pool =  interfaces_pool;
    }

    /* Do we need an endpoint pool ?  */
    if (endpoints_found != 0 && status == UX_SUCCESS)
    {

        /* We assign a pool for the endpoints.  */
        endpoints_pool =  _ux_utility_memory_allocate_mulc_safe(UX_NO_ALIGN, UX_REGULAR_MEMORY, endpoints_found, sizeof(UX_SLAVE_ENDPOINT));
        if (endpoints_pool == UX_NULL)
            status = UX_MEMORY_INSUFFICIENT;
        else
        {

            /* Save the endpoint pool address in the device container.  */
            device -> ux_slave_device_endpoints_pool =  endpoints_pool;

            /* We need to assign a transfer buffer to each endpoint. Each endpoint is assigned the
            maximum buffer size.  We also assign the semaphore used by the endpoint to synchronize transfer
            completion. */
            while (endpoints_pool < (device -> ux_slave_device_endpoints_pool + endpoints_found))
            {

#if UX_DEVICE_ENDPOINT_BUFFER_OWNER == 0

                /* Obtain some memory.  */
                endpoints_pool -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer =
                                _ux_utility_memory_allocate(UX_NO_ALIGN, UX_CACHE_SAFE_MEMORY, UX_SLAVE_REQUEST_DATA_MAX_LENGTH);

                /* Ensure we could allocate memory.  */
                if (endpoints_pool -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer == UX_NULL)
                {
                    status = UX_MEMORY_INSUFFICIENT;
                    break;
                }
#endif

                /* Create the semaphore for the endpoint.  */
                status =  _ux_device_semaphore_create(&endpoints_pool -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_semaphore,
                                                    "ux_transfer_request_semaphore", 0);

                /* Check completion status.  */
                if (status != UX_SUCCESS)
                {
                    status = UX_SEMAPHORE_ERROR;
                    break;
                }

                /* Next endpoint.  */
                endpoints_pool++;
            }
        }
    }
    else
        endpoints_pool = UX_NULL;

    /* Return successful completion.  */
    if (status == UX_SUCCESS)
        return(UX_SUCCESS);

    /* Free resources when there is error.  */

    /* Free device -> ux_slave_device_endpoints_pool.  */
    if (endpoints_pool)
    {

        /* In error cases creating endpoint resources, endpoints_pool is endpoint that failed.
         * Previously allocated things should be freed.  */
        while(endpoints_pool >= device -> ux_slave_device_endpoints_pool)
        {

            /* Delete ux_slave_transfer_request_semaphore.  */
            if (_ux_device_semaphore_created(&endpoints_pool -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_semaphore))
                _ux_device_semaphore_delete(&endpoints_pool -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_semaphore);

#if UX_DEVICE_ENDPOINT_BUFFER_OWNER == 0

            /* Free ux_slave_transfer_request_data_pointer buffer.  */
            if (endpoints_pool -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer)
                _ux_utility_memory_free(endpoints_pool -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer);
#endif

            /* Move to previous endpoint.  */
            endpoints_pool --;
        }

        _ux_utility_memory_free(device -> ux_slave_device_endpoints_pool);
    }

    /* Free device -> ux_slave_device_interfaces_pool.  */
    if (device -> ux_slave_device_interfaces_pool)
        _ux_utility_memory_free(device -> ux_slave_device_interfaces_pool);

    /* Free device -> ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer.  */
    if (device -> ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer)
        _ux_utility_memory_free(device -> ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer);

    /* Free _ux_system_slave -> ux_system_slave_class_array.  */
    _ux_utility_memory_free(_ux_system_slave -> ux_system_slave_class_array);

    /* Return completion status.  */
    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_device_stack_initialize                        PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in device stack initialization          */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    class_name                            Name of class                 */
/*    class_function_entry                  Class entry function          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_device_stack_initialize           Device Stack Initialize       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2023     Chaoqiong Xiao           Initial Version 6.3.0         */
/*                                                                        */
/**************************************************************************/
UINT  _uxe_device_stack_initialize(UCHAR * device_framework_high_speed, ULONG device_framework_length_high_speed,
                                  UCHAR * device_framework_full_speed, ULONG device_framework_length_full_speed,
                                  UCHAR * string_framework, ULONG string_framework_length,
                                  UCHAR * language_id_framework, ULONG language_id_framework_length,
                                  UINT (*ux_system_slave_change_function)(ULONG))
{

    /* Sanity checks.  */
    if (((device_framework_high_speed == UX_NULL) && (device_framework_length_high_speed != 0)) ||
        (device_framework_full_speed == UX_NULL) || (device_framework_length_full_speed == 0) ||
        ((string_framework == UX_NULL) && (string_framework_length != 0)) ||
        (language_id_framework == UX_NULL) || (language_id_framework_length == 0))
        return(UX_INVALID_PARAMETER);

    /* Invoke stack initialize function.  */
    return(_ux_device_stack_initialize(device_framework_high_speed, device_framework_length_high_speed,
                                       device_framework_full_speed, device_framework_length_full_speed,
                                       string_framework, string_framework_length,
                                       language_id_framework, language_id_framework_length,
                                       ux_system_slave_change_function));
}
#endif
