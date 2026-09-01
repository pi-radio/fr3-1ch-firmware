#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/ux_utility.h>
#include <usbxx/ux_device_stack.h>

using namespace USBXX;

UINT  STM32::DCD::abort_transfer(UX_SLAVE_TRANSFER *transfer_request)
{
  Endpoint       *endpoint;

  endpoint =  (Endpoint *)transfer_request -> ux_slave_transfer_request_endpoint;

  HAL_PCD_EP_Abort(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);
  HAL_PCD_EP_Flush(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);

  return 0;
}

UINT  STM32::DCD::transfer_in(UX_SLAVE_TRANSFER *xfer)
{
  /* Get the pointer to the logical endpoint from the transfer request.  */
  auto endpoint =  xfer -> ux_slave_transfer_request_endpoint;

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
    auto status =  _ux_utility_semaphore_get(&xfer -> ux_slave_transfer_request_semaphore,
                                        (ULONG)xfer -> ux_slave_transfer_request_timeout);

    /* Check the completion code. */
    if (status != UX_SUCCESS)
      return status;

    /* Check the transfer request completion code. We may have had a BUS reset or
       a device disconnection.  */
    if (xfer -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
      return(xfer -> ux_slave_transfer_request_completion_code);
  }

  return 0;
}


UINT  STM32::DCD::transfer_out(UX_SLAVE_TRANSFER *xfer)
{
  /* Get the pointer to the logical endpoint from the transfer request.  */
  auto endpoint =  xfer -> ux_slave_transfer_request_endpoint;

  if (endpoint->ux_slave_endpoint_descriptor.bEndpointAddress != 0) {
    int a = 0;
  }

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
    auto status =  _ux_utility_semaphore_get(&xfer -> ux_slave_transfer_request_semaphore,
                                        (ULONG)xfer -> ux_slave_transfer_request_timeout);

    /* Check the completion code. */
    if (status != UX_SUCCESS)
        return status;

    xfer -> ux_slave_transfer_request_actual_length = xfer->ux_slave_transfer_request_requested_length;

    /* Check the transfer request completion code. We may have had a BUS reset or
       a device disconnection.  */
    if (xfer -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
      return(xfer -> ux_slave_transfer_request_completion_code);

  }

  /* Return to caller with success.  */
  return 0;
}

UINT  STM32::DCD::transfer_request(UX_SLAVE_TRANSFER *xfer)
{
  /* Check for transfer direction.  Is this a IN endpoint ? */
  if (xfer -> ux_slave_transfer_request_phase == UX_TRANSFER_PHASE_DATA_OUT)
    return transfer_out(xfer);

  return transfer_in(xfer);
}
