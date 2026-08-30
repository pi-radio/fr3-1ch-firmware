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

#include <usbxx/ux_api.h>
#include <usbxx/ux_device_stack.h>

#include <usbxx/device.hpp>
#include <usbxx/stm32/dcd.hpp>

#include <usbxx/device.hpp>

using namespace USBXX;

UINT  _ux_device_stack_disconnect(VOID)
{

USBXX::DCD                *dcd;
UX_SLAVE_DEVICE             *device;
UX_SLAVE_INTERFACE          *interface_ptr; 
UX_SLAVE_INTERFACE          *next_interface; 
UX_SLAVE_CLASS              *class_ptr;
UX_SLAVE_CLASS_COMMAND      class_command;
UINT                        status = UX_ERROR;
                        
    /* Get the pointer to the DCD.  */
    dcd =  STM32::gDCD;

    /* Get the pointer to the device.  */
    device =  &_ux_system_slave -> ux_system_slave_device;

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_DISCONNECT, device, 0, 0, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)

    /* If trace is enabled, register this object.  */
    UX_TRACE_OBJECT_UNREGISTER(device);

    /* If the device was in the configured state, there may be interfaces
       attached to the configuration.  */
    if (device -> ux_slave_device_state == UX_DEVICE_CONFIGURED)
    {
        /* Get the pointer to the first interface.  */
        interface_ptr =  device -> ux_slave_device_first_interface;

#if !defined(UX_DEVICE_INITIALIZE_FRAMEWORK_SCAN_DISABLE) || UX_MAX_DEVICE_INTERFACES > 1
        /* Parse all the interfaces if any.  */
        while (interface_ptr != nullptr)
        {
#endif

            /* Build all the fields of the Class Command.  */
            class_command.ux_slave_class_command_request =   UX_SLAVE_CLASS_COMMAND_DEACTIVATE;
            class_command.ux_slave_class_command_interface =  (VOID *) interface_ptr;

            /* Get the pointer to the class container of this interface.  */
            class_ptr =  interface_ptr -> ux_slave_interface_class;
            
            /* Store the class container. */
            class_command.ux_slave_class_command_class_ptr =  class_ptr;

            /* If there is a class container for this instance, deactivate it.  */
            if (class_ptr != nullptr)
            
                /* Call the class with the DEACTIVATE signal.  */
                class_ptr -> ux_slave_class_entry_function(&class_command);

#if !defined(UX_DEVICE_INITIALIZE_FRAMEWORK_SCAN_DISABLE) || UX_MAX_DEVICE_INTERFACES > 1
            /* Get the next interface.  */
            next_interface =  interface_ptr -> ux_slave_interface_next_interface;
#endif

            /* Remove the interface and all endpoints associated with it.  */
            _ux_device_stack_interface_delete(interface_ptr);

#if !defined(UX_DEVICE_INITIALIZE_FRAMEWORK_SCAN_DISABLE) || UX_MAX_DEVICE_INTERFACES > 1
            /* Now we refresh the interface pointer.  */
            interface_ptr =  next_interface;
        }
#endif

        /* Mark the device as attached now.  */
        device -> ux_slave_device_state =  UX_DEVICE_ATTACHED;
    }

    /* If the device was attached, we need to destroy the control endpoint.  */
    if (device -> ux_slave_device_state == UX_DEVICE_ATTACHED)

        /* Now we can destroy the default control endpoint.  */
        status =  dcd->destroy_endpoint(&device -> ux_slave_device_control_endpoint);

    /* We are reverting to configuration 0.  */
    device -> ux_slave_device_configuration_selected =  0;

    /* Set the device to be non attached.  */
    device -> ux_slave_device_state =  UX_DEVICE_RESET;

    /* Check the status change callback.  */
    if(_ux_system_slave -> ux_system_slave_change_function != nullptr)
    {

        /* Inform the application if a callback function was programmed.  */
        _ux_system_slave -> ux_system_slave_change_function(UX_DEVICE_REMOVED);
    }

    /* Return the status to the caller.  */
    return(status);
}

