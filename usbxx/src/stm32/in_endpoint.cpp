#include <stdexcept>
#include <cassert>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>
#include <usbxx/device.hpp>
#include <usbxx/event_log.hpp>
#include <usbxx/ux_device_stack.h>

using namespace USBXX;

STM32::InEndpoint::InEndpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr) :
    STM32::Endpoint(_device, _dcd, _epaddr)
{
  assert((epaddr & 0x80));
  ep.is_in = 1U;
  ep.num = 0;
  ep.type = EP_TYPE_CTRL;
  ep.maxpacket = 0U;
  ep.xfer_buff = 0U;
  ep.xfer_len = 0U;
  ep.pmaaddress = 0x80;}

PCD_EPTypeDef *STM32::InEndpoint::get_epdata()
{
  return &ep;
}


void STM32::InEndpoint::activate()
{
  auto PCD = dcd->get_PCD();

  auto ep = get_epdata();

  uint32_t wEpRegVal;

  wEpRegVal = PCD_GET_ENDPOINT(PCD, epindex()) & USB_EP_T_MASK;

  /* initialize Endpoint */
  switch (ep->type)
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

  assert(ep->pmaaddress != 0);

  /*Set the endpoint Transmit buffer address */
  pcd_set_tx_address(ep->num, ep->pmaaddress);
  PCD_CLEAR_TX_DTOG(PCD, ep->num);

  if (ep->type != EP_TYPE_ISOC)
  {
    /* Configure NAK status for the Endpoint */
    PCD_SET_EP_TX_STATUS(PCD, ep->num, USB_EP_TX_NAK);
  }
  else
  {
    /* Configure TX Endpoint to disabled state */
    PCD_SET_EP_TX_STATUS(PCD, ep->num, USB_EP_TX_DIS);
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
  auto ep = get_epdata();
  auto g = dcd->guard();

  PCD_CLEAR_TX_DTOG(PCD, epindex());

  if (ep->type != EP_TYPE_ISOC)
  {
    /* Configure NAK status for the Endpoint */
    PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_NAK);
  }
}

void STM32::InEndpoint::abort_transfer()
{
  auto PCD = dcd->get_PCD();
  PCD_EPTypeDef *ep = get_epdata();

  if (ep->type != EP_TYPE_ISOC)
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
  auto ep = get_epdata();

  stalled = true;
  ep->num = epindex();

  {
    auto g = dcd->guard();

    PCD_SET_EP_TX_STATUS(PCD, epindex(), USB_EP_TX_STALL);
  }
}

void STM32::InEndpoint::on_data_in()
{
  auto PCD = dcd->get_PCD();
  auto hpcd = dcd->get_hpcd();

  /* clear int flag */
  PCD_CLEAR_TX_EP_CTR(PCD, epindex());

  /* Multi-packet on the NON control IN endpoint */
  auto TxPctSize = (uint16_t)PCD_GET_EP_TX_CNT(PCD, epindex());

  if (ep.xfer_len > TxPctSize)
  {
    ep.xfer_len -= TxPctSize;
  }
  else
  {
    ep.xfer_len = 0U;
  }

  /* Zero Length Packet? */
  if (ep.xfer_len == 0U)
  {
    /* Check if a ZLP should be armed.  */
    if (transfer.force_zlp &&
        transfer.requested_length)
    {
      transfer.force_zlp = UX_FALSE;
      transfer.in_transfer_length = 0;

      /* Arm a ZLP packet on IN.  */
      ll_transmit(0, 0);
    }
    else
    {
      transfer.actual_length = transfer.requested_length;

    /* Non control endpoint operation, use semaphore.  */
      transfer.complete(UX_SUCCESS);
    }
  }
  else
  {
    /* Transfer is not yet Done */
    ep.xfer_buff += TxPctSize;
    ep.xfer_count += TxPctSize;
    start_transfer(&ep);
  }
}

void STM32::InEndpoint::ll_transmit(uint8_t *buf, uint32_t len)
{
  ep.xfer_buff = buf;
  ep.xfer_len = len;
  ep.xfer_fill_db = 1U;
  ep.xfer_len_db = len;
  ep.xfer_count = 0U;
  ep.is_in = 1U;
  ep.num = epindex();

  start_transfer(&ep);

  event_log.push_event(UsbEvent::ENDPOINT_XMIT, epaddr);
}
