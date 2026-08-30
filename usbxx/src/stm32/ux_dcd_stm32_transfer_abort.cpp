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

UINT  STM32::DCD::abort_transfer(UX_SLAVE_TRANSFER *transfer_request)
{

#if !defined (USBD_HAL_TRANSFER_ABORT_NOT_SUPPORTED)

   UX_SLAVE_ENDPOINT       *endpoint;


    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint =  transfer_request -> ux_slave_transfer_request_endpoint;

    HAL_PCD_EP_Abort(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);
    HAL_PCD_EP_Flush(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);

    /* No semaphore put here since it's already done in stack.  */
#endif /* USBD_HAL_TRANSFER_ABORT_NOT_SUPPORTED */

    /* Return to caller with success.  */
    return(UX_SUCCESS);
}
