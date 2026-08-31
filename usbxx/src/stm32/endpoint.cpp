
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

#include <stdexcept>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>
#include <usbxx/ux_device_stack.h>

using namespace USBXX;

UINT STM32::DCD::create_endpoint(Endpoint *endpoint)
{
  STM32Endpoint *stmep = (STM32Endpoint *)endpoint;

  stmep->direction = endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION;

  /* Check if it is non-control endpoint.  */
  if (stmep->index != 0)
  {
    HAL_PCD_EP_Open(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                    endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize,
                    endpoint->ux_slave_endpoint_descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE);
  }

  /* Return successful completion.  */
  return 0;
}

UINT STM32::DCD::destroy_endpoint(Endpoint *endpoint)
{
 auto ed =  (STM32Endpoint *) endpoint;

 ed->reset_flags();

  /* Deactivate the endpoint.  */
  HAL_PCD_EP_Close(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);

  /* This function never fails.  */
  return 0;
}

UINT STM32::DCD::get_endpoint_status(ULONG endpoint_index)
{
  STM32Endpoint      *ed;

  /* Fetch the address of the physical endpoint.  */
  ed = __get_endpoint(endpoint_index);

  /* Check the endpoint status, if it is free, we have a illegal endpoint.  */
  if (!ed->used)
    throw std::runtime_error("Status on invalid endpoint");

  /* Check if the endpoint is stalled.  */
  if (!ed->stalled)
      return(UX_FALSE);
  else
      return(UX_TRUE);
}

UINT  STM32::DCD::reset_endpoint(Endpoint *endpoint)
{
  auto ed =  (STM32Endpoint *)endpoint;
  UX_INTERRUPT_SAVE_AREA

  UX_DISABLE

  ed->stalled = false;
  ed->done = false;
  ed->setup = false;


  /* Set the state of the endpoint to IDLE.  */
  ed->state =  STM32Endpoint_STATE_IDLE;

  /* Clear STALL condition.  */
  HAL_PCD_EP_ClrStall(pcd_handle, endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress);

  /* Flush buffer.  */
  HAL_PCD_EP_Flush(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);

  /* Wakeup pending thread.  */
  if (endpoint -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_semaphore.tx_semaphore_suspended_count)
      _ux_utility_semaphore_put(&endpoint -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_semaphore);

  UX_RESTORE

  /* This function never fails.  */
  return 0;
}


UINT  STM32::DCD::stall(Endpoint *endpoint)
{
  auto ed =  (STM32Endpoint *)endpoint;

  ed->stalled = true;

    /* Stall the endpoint.  */
  HAL_PCD_EP_SetStall(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress | ed -> direction);

  /* This function never fails.  */
  return 0;
}
