#include <stdexcept>
#include <cassert>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>
#include <usbxx/device.hpp>
#include <usbxx/event_log.hpp>
//#include <usbxx/ux_device_stack.h>

using namespace USBXX;

STM32::OutEndpoint::OutEndpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr) :
    STM32::Endpoint(_device, _dcd, _epaddr)
{
  assert(!(epaddr & 0x80));

  ep.pmaaddress = pmaaddr();
}

void STM32::OutEndpoint::activate()
{
  auto PCD = dcd->get_PCD();

  uint32_t wEpRegVal;

  wEpRegVal = PCD_GET_ENDPOINT(PCD, epindex()) & USB_EP_T_MASK;

  switch (get_type())
  {
    case EP_TYPE_CTRL:
      wEpRegVal |= USB_EP_CONTROL;
      break;

    case EP_TYPE_BULK:
      wEpRegVal |= USB_EP_BULK;
      break;

    case EP_TYPE_INTR:
      wEpRegVal |= USB_EP_INTERRUPT;
      break;

    case EP_TYPE_ISOC:
      wEpRegVal |= USB_EP_ISOCHRONOUS;
      break;

    default:
      throw USBXX::runtime_error("Invalid endpoint type");
  }

  PCD_SET_ENDPOINT(PCD, epindex(), (wEpRegVal | USB_EP_VTRX | USB_EP_VTTX));

  PCD_SET_EP_ADDRESS(PCD, epindex(), epindex()); // ??

  assert(ep.pmaaddress != 0);

  /* Set the endpoint Receive buffer address */
  pcd_set_rx_address(epindex(), ep.pmaaddress);

  /* Set the endpoint Receive buffer counter */
  pcd_set_rx_cnt(epindex(), max_packet_size());
  PCD_CLEAR_RX_DTOG(PCD, epindex());


  PCD_SET_EP_RX_STATUS(PCD, epindex(), USB_EP_RX_NAK);
}

void STM32::OutEndpoint::deactivate()
{
  auto PCD = dcd->get_PCD();

  PCD_CLEAR_RX_DTOG(PCD, epindex());
  PCD_SET_EP_RX_STATUS(PCD, epindex(), USB_EP_RX_DIS);
}

void STM32::OutEndpoint::clear_stall()
{
  auto PCD = dcd->get_PCD();
  auto g = dcd->guard();

  stalled = false;

  PCD_CLEAR_RX_DTOG(PCD, epindex());
  PCD_SET_EP_RX_STATUS(PCD, epindex(), USB_EP_RX_VALID);
}

void STM32::OutEndpoint::stall()
{
  auto PCD = dcd->get_PCD();
  auto g = dcd->guard();

  stalled = true;

  PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_STALL);
}

void STM32::OutEndpoint::on_data_out()
{
  auto PCD = dcd->get_PCD();

  PCD_CLEAR_RX_EP_CTR(PCD, epindex());

  auto count = (uint16_t)PCD_GET_EP_RX_CNT(PCD, epindex());

  if (count != 0U)
  {
    read_pma(ep.xfer_buff, ep.pmaaddress, count);
  }

  /* multi-packet on the NON control OUT endpoint */
  ep.xfer_count += count;

  if ((ep.xfer_len == 0U) || (count < max_packet_size()))
  {
    transfer.actual_length = ep.xfer_count;

    transfer.complete(0);
  }
  else
  {
     ep.xfer_buff += count;
     start_transfer_out(&ep);
  }
}


void STM32::OutEndpoint::ll_receive(uint8_t *buf, uint32_t len)
{
  ep.xfer_buff = buf;
  ep.xfer_len = len;
  ep.xfer_count = 0U;

  start_transfer_out(&ep);
}

void STM32::OutEndpoint::abort_transfer()
{
  auto PCD = dcd->get_PCD();

  if (get_type() != EP_TYPE_ISOC)
  {
    /* Configure NAK status for the Endpoint */
    PCD_SET_EP_RX_STATUS(PCD, epindex(), USB_EP_RX_NAK);
  }
  else
  {
    /* Configure RX Endpoint to disabled state */
    PCD_SET_EP_RX_STATUS(PCD, epindex(), USB_EP_RX_DIS);
  }
}
