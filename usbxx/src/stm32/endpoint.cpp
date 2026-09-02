
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

#include <stdexcept>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>
#include <usbxx/ux_device_stack.h>

using namespace USBXX;

UINT STM32::Endpoint::create()
{
  direction = ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION;

  if (index != 0)
  {
    HAL_PCD_EP_Open(dcd->get_pcd_handle(), ux_slave_endpoint_descriptor.bEndpointAddress,
                    ux_slave_endpoint_descriptor.wMaxPacketSize,
                    ux_slave_endpoint_descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE);
  }

  /* Return successful completion.  */
  return 0;
}

UINT STM32::Endpoint::destroy()
{
 reset_flags();

  /* Deactivate the endpoint.  */
 HAL_PCD_EP_Close(dcd->get_pcd_handle(), ux_slave_endpoint_descriptor.bEndpointAddress);

  /* This function never fails.  */
 return 0;
}

bool STM32::Endpoint::is_stalled()
{
  if (!used)
    throw std::runtime_error("Status on invalid endpoint");

  return stalled;
}

UINT  STM32::Endpoint::reset()
{
  UX_INTERRUPT_SAVE_AREA

  UX_DISABLE

  stalled = false;
  done = false;
  setup = false;


  /* Set the state of the endpoint to IDLE.  */
  state =  EndpointState::IDLE;

  auto pcd_handle = dcd->get_pcd_handle();

  /* Clear STALL condition.  */
  HAL_PCD_EP_ClrStall(pcd_handle, ux_slave_endpoint_descriptor.bEndpointAddress);

  /* Flush buffer.  */
  HAL_PCD_EP_Flush(pcd_handle, ux_slave_endpoint_descriptor.bEndpointAddress);

  /* Wakeup pending thread.  */
  if (ux_slave_endpoint_transfer_request.semaphore.tx_semaphore_suspended_count)
    _ux_utility_semaphore_put(&ux_slave_endpoint_transfer_request.semaphore);

  UX_RESTORE

  /* This function never fails.  */
  return 0;
}


void STM32::Endpoint::stall()
{
  stalled = true;

  /* Stall the endpoint.  */
  HAL_PCD_EP_SetStall(dcd->get_pcd_handle(), ux_slave_endpoint_descriptor.bEndpointAddress | direction);
}
