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

void STM32::Transfer::abort(uint32_t code)
{
  auto stm32ep = static_pointer_cast<STM32::Endpoint>(endpoint);

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

uint32_t STM32::Transfer::transfer()
{
  /* Check for transfer direction.  Is this a IN endpoint ? */
  if (phase == TransferPhase::DATA_OUT)
    return transfer_out();
  else if (phase == TransferPhase::DATA_IN) {
    return transfer_in();
  }

  // Invalid transfer
  assert(0);
}



uint32_t  STM32::Transfer::transfer_in()
{
  auto stm32ep = static_pointer_cast<STM32::Endpoint>(endpoint);

  /* We have a request for a SETUP or OUT Endpoint.  */
  /* Receive data.  */
  stm32ep->ll_receive(data,
      requested_length);

  if (endpoint->is_control())
      return 0;


  return wait();
}


uint32_t STM32::Transfer::transfer_out()
{
  uint32_t retval = 0;

  if (!endpoint->is_control()) {
    int a = 0;
  }

  auto stm32ep = static_pointer_cast<STM32::Endpoint>(endpoint);

/* Transmit data.  */
  stm32ep->ll_transmit(data, requested_length);

  if (endpoint->is_control())
    return 0;

  retval = wait();

  actual_length = requested_length;

  /* Return to caller with success.  */
  return retval;
}

