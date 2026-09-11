#include <stdexcept>
#include <cassert>

/* Include necessary system files.  */

#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>
#include <usbxx/device.hpp>
#include <usbxx/event_log.hpp>

using namespace USBXX;

STM32::InEndpoint::InEndpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr) :
    STM32::Endpoint(_device, _dcd, _epaddr)
{
  assert((epaddr & 0x80));
  ep.xfer_buff = 0U;
  ep.xfer_len = 0U;
  ep.pmaaddress = 0x80;
}

void STM32::InEndpoint::activate()
{
  auto PCD = dcd->get_PCD();

  uint32_t wEpRegVal;

  wEpRegVal = PCD_GET_ENDPOINT(PCD, epindex()) & USB_EP_T_MASK;

  /* initialize Endpoint */
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

  /*Set the endpoint Transmit buffer address */
  pcd_set_tx_address(epindex(), ep.pmaaddress);
  PCD_CLEAR_TX_DTOG(PCD, epindex());

  if (get_type() != EP_TYPE_ISOC)
  {
    /* Configure NAK status for the Endpoint */
    PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_NAK);
  }
  else
  {
    /* Configure TX Endpoint to disabled state */
    PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_DIS);
  }
}

void STM32::InEndpoint::deactivate()
{
  auto PCD = dcd->get_PCD();

  PCD_CLEAR_TX_DTOG(PCD, epindex());
  PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_DIS);
}

void STM32::InEndpoint::clear_stall()
{
  auto PCD = dcd->get_PCD();
  auto g = dcd->guard();

  PCD_CLEAR_TX_DTOG(PCD, epindex());

  if (get_type() != EP_TYPE_ISOC)
  {
    /* Configure NAK status for the Endpoint */
    PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_NAK);
  }
}

void STM32::InEndpoint::abort_transfer()
{
  auto PCD = dcd->get_PCD();

  if (get_type() != EP_TYPE_ISOC)
  {
    /* Configure NAK status for the Endpoint */
    PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_NAK);
  }
  else
  {
    /* Configure TX Endpoint to disabled state */
    PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_DIS);
  }
}

void STM32::InEndpoint::stall()
{
  auto PCD = dcd->get_PCD();

  stalled = true;

  {
    auto g = dcd->guard();

    PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_STALL);
  }
}

void STM32::InEndpoint::on_data_in()
{
  auto PCD = dcd->get_PCD();

  /* clear int flag */
  PCD_CLEAR_TX_EP_CTR(PCD, epindex());

  /* Multi-packet on the NON control IN endpoint */
  auto tx_len = (uint16_t)PCD_GET_EP_TX_CNT(PCD, epindex());

  if (ep.xfer_len > tx_len)
  {
    ep.xfer_len -= tx_len;
  }
  else
  {
    ep.xfer_len = 0U;
  }

  if (ep.xfer_len)
  {
    /* Transfer is not yet Done */
    ep.xfer_buff += tx_len;
    ep.xfer_count += tx_len;
    start_transfer_in(&ep);

    return;
  }

  /* Check if a ZLP should be armed.  */
  if (transfer.force_zlp &&
      transfer.requested_length)
  {
    transfer.force_zlp = false;
    transfer.in_transfer_length = 0;

    /* Arm a ZLP packet on IN.  */
    ll_transmit(0, 0);
    return;
  }


  transfer.actual_length = transfer.requested_length;
  transfer.complete(0);
}

void STM32::InEndpoint::ll_transmit(uint8_t *buf, uint32_t len)
{
  ep.xfer_buff = buf;
  ep.xfer_len = len;
  ep.xfer_fill_db = 1U;
  ep.xfer_len_db = len;
  ep.xfer_count = 0U;

  start_transfer_in(&ep);

  event_log.push_event(UsbEvent::ENDPOINT_XMIT, epaddr);
}
