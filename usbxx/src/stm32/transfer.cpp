#include <threadxx/intr.hpp>

#include <usbxx/ux_api.h>
#include <usbxx/stm32/transfer.hpp>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/ux_utility.h>
#include <usbxx/ux_device_stack.h>

using namespace USBXX;

void STM32::Transfer::complete(uint32_t code)
{
  status = TransferStatus::COMPLETED;
  completion_code = code;

  if (!endpoint->is_control())
    semaphore.put();
}

#if 0
UX_INTERRUPT_SAVE_AREA

USBXX::DCD    *dcd;

    UX_PARAMETER_NOT_USED(completion_code);

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_TRANSFER_ABORT, xfer, completion_code, 0, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)

    /* Get the pointer to the DCD.  */
    dcd = STM32::gDCD;

    /* Sets the completion code due to bus reset.  */
    xfer -> completion_code = completion_code;

    /* Ensure we're not preempted by the transfer completion ISR.  */
    UX_DISABLE

    /* It's possible the transfer already completed. Ensure it hasn't before doing the abort.  */
    if (xfer -> status == UX_TRANSFER_STATUS_PENDING)
    {

        /* Call the DCD if necessary for cleaning up the pending transfer.  */
        dcd->abort_transfer(xfer);

        /* Restore interrupts. Note that the transfer request should not be modified now.  */
        UX_RESTORE

        /* We need to set the completion code for the transfer to aborted. Note
           that the transfer request function cannot simultaneously modify this
           because if the transfer was pending, then the transfer's thread is
           currently waiting for it to complete.  */

        /* Wake up the device driver who is waiting on the semaphore.  */
        xfer->abort();
    }
    else
    {

        /* Restore interrupts.  */
        UX_RESTORE
    }

    /* This function never fails.  */
    return 0;

#endif

void STM32::Transfer::abort(uint32_t code)
{
  STM32::Endpoint *stm32ep = (STM32::Endpoint *)endpoint;

  completion_code = code;

  {
    TXX::lock_intr l;

    if (status != TransferStatus::PENDING)
      return;

    status = TransferStatus::ABORTED;
  }

  stm32ep->abort_transfer();

  if (!endpoint->is_control())
    semaphore.put();
}




UINT  STM32::DCD::transfer_in(Transfer *xfer)
{
  /* Get the pointer to the logical endpoint from the transfer request.  */
  auto endpoint =  xfer -> endpoint;

  /* We have a request for a SETUP or OUT Endpoint.  */
  /* Receive data.  */
  HAL_PCD_EP_Receive(pcd_handle,
                      endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                      xfer->data,
                      xfer->requested_length);

  /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
     thread to suspend.  */
  if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & (UINT)~UX_ENDPOINT_DIRECTION) != 0)
  {
    return xfer->wait();
  }

  return 0;
}


UINT  STM32::DCD::transfer_out(Transfer *xfer)
{
  /* Get the pointer to the logical endpoint from the transfer request.  */
  auto endpoint =  xfer -> endpoint;

  if (endpoint->ux_slave_endpoint_descriptor.bEndpointAddress != 0) {
    int a = 0;
  }

/* Transmit data.  */
  HAL_PCD_EP_Transmit(pcd_handle,
                      endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                      xfer->data,
                      xfer->requested_length);

  /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
     thread to suspend.  */
  if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & (UINT)~UX_ENDPOINT_DIRECTION) != 0)
  {
    /* We should wait for the semaphore to wake us up.  */
    auto status = xfer->wait();

    xfer -> actual_length = xfer->requested_length;
  }

  /* Return to caller with success.  */
  return 0;
}

UINT  STM32::DCD::transfer_request(USBXX::Transfer *_xfer)
{
  Transfer *xfer = (Transfer *)_xfer;

  /* Check for transfer direction.  Is this a IN endpoint ? */
  if (xfer -> phase == TransferPhase::DATA_OUT)
    return transfer_out(xfer);

  return transfer_in(xfer);
}
