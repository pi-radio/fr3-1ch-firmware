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

using namespace USBXX;

UINT  _ux_device_stack_transfer_abort(UX_SLAVE_TRANSFER *xfer, ULONG completion_code)
{

UX_INTERRUPT_SAVE_AREA

USBXX::DCD    *dcd;

    UX_PARAMETER_NOT_USED(completion_code);

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_TRANSFER_ABORT, xfer, completion_code, 0, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)

    /* Get the pointer to the DCD.  */
    dcd = STM32::gDCD;

    /* Sets the completion code due to bus reset.  */
    xfer -> ux_slave_transfer_request_completion_code = completion_code;

    /* Ensure we're not preempted by the transfer completion ISR.  */
    UX_DISABLE

    /* It's possible the transfer already completed. Ensure it hasn't before doing the abort.  */
    if (xfer -> ux_slave_transfer_request_status == UX_TRANSFER_STATUS_PENDING)
    {

        /* Call the DCD if necessary for cleaning up the pending transfer.  */
        dcd->abort_transfer(xfer);

        /* Restore interrupts. Note that the transfer request should not be modified now.  */
        UX_RESTORE

        /* We need to set the completion code for the transfer to aborted. Note
           that the transfer request function cannot simultaneously modify this 
           because if the transfer was pending, then the transfer's thread is 
           currently waiting for it to complete.  */
        xfer -> ux_slave_transfer_request_status =  UX_TRANSFER_STATUS_ABORT;

        /* Wake up the device driver who is waiting on the semaphore.  */
        _ux_device_semaphore_put(&xfer -> ux_slave_transfer_request_semaphore);
    }
    else
    {

        /* Restore interrupts.  */
        UX_RESTORE
    }

    /* This function never fails.  */
    return 0;       
}

