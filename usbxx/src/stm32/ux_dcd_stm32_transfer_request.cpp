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
/**   STM32 Controller Driver                                             */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE


/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/ux_utility.h>
#include <usbxx/ux_device_stack.h>

using namespace USBXX;

UINT  STM32::DCD::transfer_request(UX_SLAVE_TRANSFER *xfer)
{

UX_SLAVE_ENDPOINT       *endpoint;
UINT                    status;


    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint =  xfer -> ux_slave_transfer_request_endpoint;

    /* Check for transfer direction.  Is this a IN endpoint ? */
    if (xfer -> ux_slave_transfer_request_phase == UX_TRANSFER_PHASE_DATA_OUT)
    {

        /* Transmit data.  */
        HAL_PCD_EP_Transmit(pcd_handle,
                            endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                            xfer->ux_slave_transfer_request_data_pointer,
                            xfer->ux_slave_transfer_request_requested_length);

        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & (UINT)~UX_ENDPOINT_DIRECTION) != 0)
        {

            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&xfer -> ux_slave_transfer_request_semaphore,
                                                (ULONG)xfer -> ux_slave_transfer_request_timeout);

            /* Check the completion code. */
            if (status != UX_SUCCESS)
                return(status);

            xfer -> ux_slave_transfer_request_actual_length = xfer->ux_slave_transfer_request_requested_length;

            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (xfer -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
                return(xfer -> ux_slave_transfer_request_completion_code);

            /* Return to caller with success.  */
            return(UX_SUCCESS);
        }
    }
    else
    {

        /* We have a request for a SETUP or OUT Endpoint.  */
        /* Receive data.  */
        HAL_PCD_EP_Receive(pcd_handle,
                            endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                            xfer->ux_slave_transfer_request_data_pointer,
                            xfer->ux_slave_transfer_request_requested_length);

        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & (UINT)~UX_ENDPOINT_DIRECTION) != 0)
        {

            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&xfer -> ux_slave_transfer_request_semaphore,
                                                (ULONG)xfer -> ux_slave_transfer_request_timeout);

            /* Check the completion code. */
            if (status != UX_SUCCESS)
                return(status);

            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (xfer -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
                return(xfer -> ux_slave_transfer_request_completion_code);

            /* Return to caller with success.  */
            return(UX_SUCCESS);
        }
    }

    /* Return to caller with success.  */
    return(UX_SUCCESS);
}
