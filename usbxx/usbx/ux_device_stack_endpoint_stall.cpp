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

#include <usbxx/stm32/dcd.hpp>

#include <usbxx/device.hpp>

using namespace USBXX;

UINT  _ux_device_stack_endpoint_stall(Endpoint *endpoint)
{

UX_INTERRUPT_SAVE_AREA

USBXX::DCD        *dcd;
UINT                status;

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_ENDPOINT_STALL, endpoint, 0, 0, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)

    /* Get the pointer to the DCD.  */
    dcd = STM32::gDCD;

    /* Assume device is in an invalid state here in order to reduce code in following 
       section where interrupts are disabled.  */
    status =  UX_ERROR;

    /* Ensure we don't change the endpoint's state after disconnection routine
       resets it.  */
    UX_DISABLE

    /* Check if the device is in a valid state; as soon as the device is out 
       of the RESET state, transfers occur and thus endpoints may be stalled. */
    if (_ux_system_slave -> device->ux_slave_device_state != UX_DEVICE_RESET &&
        endpoint -> ux_slave_endpoint_state != UX_ENDPOINT_HALTED)
    {

        /* Stall the endpoint.  */
        endpoint->stall();

        /* Mark the endpoint state.  */
        if ((endpoint -> ux_slave_endpoint_descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE) !=
            UX_CONTROL_ENDPOINT)
            endpoint -> ux_slave_endpoint_state =  UX_ENDPOINT_HALTED;
    }

    /* Restore interrupts.  */
    UX_RESTORE

    /* Return completion status.  */
    return(status);       
}

