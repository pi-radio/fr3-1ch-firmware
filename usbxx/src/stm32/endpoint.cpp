
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

#include <stdexcept>
#include <cassert>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>
#include <usbxx/device.hpp>
#include <usbxx/ux_device_stack.h>

using namespace USBXX;

UINT STM32::Endpoint::create()
{
  assert(ux_slave_endpoint_descriptor.wMaxPacketSize != 0);

  /* Calculate endpoint transfer payload max size.  */
  auto max_transfer_length =
          ux_slave_endpoint_descriptor.wMaxPacketSize &
                                              UX_MAX_PACKET_SIZE_MASK;

  if ((_ux_system_slave -> ux_system_slave_speed == UX_HIGH_SPEED_DEVICE) &&
      (ux_slave_endpoint_descriptor.bmAttributes & 0x1u))
  {
      auto n_trans = ux_slave_endpoint_descriptor.wMaxPacketSize &
                                  UX_MAX_NUMBER_OF_TRANSACTIONS_MASK;
      if (n_trans)
      {
          n_trans >>= UX_MAX_NUMBER_OF_TRANSACTIONS_SHIFT;
          n_trans ++;
          max_transfer_length *= n_trans;
      }
  }

  /* Validate max transfer size and save it.  */
  UX_ASSERT(max_transfer_length <= UX_SLAVE_REQUEST_DATA_MAX_LENGTH);
  transfer.transfer_length = max_transfer_length;

  /* We store the endpoint in the transfer request as well.  */
  transfer.endpoint = this;

  /* By default the timeout is infinite on request.  */
  transfer.timeout = UX_WAIT_FOREVER;

  /* Attach the interface to the endpoint.  */


  /* Attach the device to the endpoint.  */


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

void STM32::Endpoint::abort_transfer()
{
  HAL_PCD_EP_Abort(dcd->get_pcd_handle(), ux_slave_endpoint_descriptor.bEndpointAddress);
  HAL_PCD_EP_Flush(dcd->get_pcd_handle(), ux_slave_endpoint_descriptor.bEndpointAddress);
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

  state =  EndpointState::IDLE;

  auto pcd_handle = dcd->get_pcd_handle();

  /* Clear STALL condition.  */
  HAL_PCD_EP_ClrStall(pcd_handle, ux_slave_endpoint_descriptor.bEndpointAddress);

  /* Flush buffer.  */
  HAL_PCD_EP_Flush(pcd_handle, ux_slave_endpoint_descriptor.bEndpointAddress);

  transfer.reset();

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
