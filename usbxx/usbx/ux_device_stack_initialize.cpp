/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Device Stack                                                        */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include <cstdint>

#include <usbxx/ux_api.h>
#include <usbxx/ux_device_stack.h>

#include <usbxx/device.hpp>

UX_COMPILE_TIME_ASSERT(!UX_OVERFLOW_CHECK_MULC_ULONG(sizeof(UX_SLAVE_CLASS), UX_MAX_SLAVE_CLASS_DRIVER), UX_MAX_SLAVE_CLASS_DRIVER_mul_ovf)

/* Define the names of all the USB Classes of USBX.  */

UCHAR _ux_system_slave_class_storage_name[] =                               "ux_slave_class_storage";
UCHAR _ux_system_slave_class_cdc_acm_name[] =                               "ux_slave_class_cdc_acm";
UCHAR _ux_system_slave_class_dpump_name[] =                                 "ux_slave_class_dpump";
UCHAR _ux_system_slave_class_pima_name[] =                                  "ux_slave_class_pima";
UCHAR _ux_system_slave_class_hid_name[] =                                   "ux_slave_class_hid";
UCHAR _ux_system_slave_class_rndis_name[] =                                 "ux_slave_class_rndis";
UCHAR _ux_system_slave_class_cdc_ecm_name[] =                               "ux_slave_class_cdc_ecm";
UCHAR _ux_system_slave_class_dfu_name[] =                                   "ux_slave_class_dfu";
UCHAR _ux_system_slave_class_audio_name[] =                                 "ux_slave_class_audio";

UCHAR _ux_system_device_class_printer_name[] =                              "ux_device_class_printer";
UCHAR _ux_system_device_class_ccid_name[] =                                 "ux_device_class_ccid";
UCHAR _ux_system_device_class_video_name[] =                                "ux_device_class_video";


UINT  _ux_device_stack_initialize(UCHAR * device_framework_high_speed, ULONG device_framework_length_high_speed,
                                  UCHAR * device_framework_full_speed, ULONG device_framework_length_full_speed,
                                  UCHAR * string_framework, ULONG string_framework_length,
                                  UCHAR * language_id_framework, ULONG language_id_framework_length,
                                  UINT (*ux_system_slave_change_function)(ULONG))
{
UX_SLAVE_INTERFACE              *interfaces_pool;
UX_SLAVE_TRANSFER               *xfer;
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
    auto device = _ux_system_slave->device;

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
    memory = (uint8_t *)::malloc(sizeof(UX_SLAVE_CLASS) * UX_MAX_SLAVE_CLASS_DRIVER);
    if (memory == nullptr)
        return(UX_MEMORY_INSUFFICIENT);
    
    ::memset(memory, 0, sizeof(UX_SLAVE_CLASS) * UX_MAX_SLAVE_CLASS_DRIVER);

    /* Save this memory allocation in the USBX project.  */
    _ux_system_slave -> ux_system_slave_class_array =  (UX_SLAVE_CLASS *) ((void *) memory);

    /* Allocate some memory for the Control Endpoint.  First get the address of the transfer request for the 
       control endpoint. */
    xfer = device->get_control_transfer();

    /* Acquire a buffer for the size of the endpoint.  */
    xfer -> ux_slave_transfer_request_data_pointer =
          (UCHAR *)::malloc(UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH);

    /* Ensure we have enough memory.  */
    if (xfer -> ux_slave_transfer_request_data_pointer == nullptr)
        status = UX_MEMORY_INSUFFICIENT;
    else
        status = UX_SUCCESS;

    ::memset(xfer -> ux_slave_transfer_request_data_pointer, 0, UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH);

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
        interfaces_pool = (UX_SLAVE_INTERFACE*)::malloc(interfaces_found * sizeof(UX_SLAVE_INTERFACE));
        if (interfaces_pool == nullptr)
            status = UX_MEMORY_INSUFFICIENT;
        else
            /* Save the interface pool address in the device container.  */
            device -> ux_slave_device_interfaces_pool =  interfaces_pool;

        ::memset(interfaces_pool, 0, interfaces_found * sizeof(UX_SLAVE_INTERFACE));
    }


    /* Return successful completion.  */
    if (status == UX_SUCCESS)
        return 0;
    

    /* Free device -> ux_slave_device_interfaces_pool.  */
    if (device -> ux_slave_device_interfaces_pool)
        ::free(device -> ux_slave_device_interfaces_pool);

#if 0
    /* Free device -> ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer.  */
    if (device -> ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer)
        ::free(device -> ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer);
#endif

    /* Free _ux_system_slave -> ux_system_slave_class_array.  */
    ::free(_ux_system_slave -> ux_system_slave_class_array);

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
    if (((device_framework_high_speed == nullptr) && (device_framework_length_high_speed != 0)) ||
        (device_framework_full_speed == nullptr) || (device_framework_length_full_speed == 0) ||
        ((string_framework == nullptr) && (string_framework_length != 0)) ||
        (language_id_framework == nullptr) || (language_id_framework_length == 0))
        return(UX_INVALID_PARAMETER);

    /* Invoke stack initialize function.  */
    return(_ux_device_stack_initialize(device_framework_high_speed, device_framework_length_high_speed,
                                       device_framework_full_speed, device_framework_length_full_speed,
                                       string_framework, string_framework_length,
                                       language_id_framework, language_id_framework_length,
                                       ux_system_slave_change_function));
}
