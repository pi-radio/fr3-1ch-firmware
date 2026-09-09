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



STM32::Endpoint::Endpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr) :
  USBXX::Endpoint(_device),
  transfer(),
  state(EndpointState::IDLE),
  epaddr(_epaddr),
  direction(0),
  dcd(_dcd)
{
  if (epaddr != 0) {
    int a = 0;
  }
  reset_flags();
}

void STM32::Endpoint::init()
{
  transfer.endpoint = shared_from_this();
  device = device;
  direction = (epaddr & 0x80) ? true : false;
}


PCD_EPTypeDef *STM32::Endpoint::get_epdata()
{
  auto hpcd = dcd->get_hpcd();

  /* initialize ep structure*/
  if (is_in())
    return &hpcd->IN_ep[epindex()];

  return &hpcd->OUT_ep[epindex()];
}


void STM32::Endpoint::open()
{
  uint32_t pmaaddr = 0xC0 + 0x80 * epindex() + (is_in() ? 0x40 : 0x00);

  auto ep = get_epdata();

  ep->doublebuffer = 0;
  ep->pmaadress = pmaaddr;
  ep->is_in = is_in();
  ep->num = epindex();
  ep->maxpacket = descriptor.wMaxPacketSize & 0x7FFU;
  ep->type = descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE;

  /* Set initial data PID. */
  if (ep->type == EP_TYPE_BULK)
  {
    ep->data_pid_start = 0U;
  }

  {
    auto g = dcd->guard();

    activate();
    //USB_ActivateEndpoint(dcd->get_PCD(), ep);
  }
}

UINT STM32::Endpoint::create()
{
  assert(descriptor.wMaxPacketSize != 0);



  /* Calculate endpoint transfer payload max size.  */
  auto max_transfer_length =
          descriptor.wMaxPacketSize &
                                              UX_MAX_PACKET_SIZE_MASK;

  if ((_ux_system_slave -> ux_system_slave_speed == UX_HIGH_SPEED_DEVICE) &&
      (descriptor.bmAttributes & 0x1u))
  {
      auto n_trans = descriptor.wMaxPacketSize &
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
  transfer.endpoint = shared_from_this();

  /* By default the timeout is infinite on request.  */
  transfer.timeout = UX_WAIT_FOREVER;

  direction = descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION;

  open();

  /* Return successful completion.  */
  return 0;
}

UINT STM32::Endpoint::destroy()
{
 reset_flags();

 if ((descriptor.bEndpointAddress & 0x7F) != 0)
   int a = 0;

  /* Deactivate the endpoint.  */
 HAL_PCD_EP_Close(dcd->get_pcd_handle(), descriptor.bEndpointAddress);

  /* This function never fails.  */
 return 0;
}

void STM32::Endpoint::abort_transfer()
{
  HAL_PCD_EP_Abort(dcd->get_pcd_handle(), descriptor.bEndpointAddress);
}

bool STM32::Endpoint::is_stalled()
{
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
  HAL_PCD_EP_ClrStall(pcd_handle, descriptor.bEndpointAddress);

  /* Flush buffer. Only OTG */

  transfer.reset();

  UX_RESTORE

  /* This function never fails.  */
  return 0;
}


void STM32::Endpoint::stall()
{
  stalled = true;

  /* Stall the endpoint.  */
  HAL_PCD_EP_SetStall(dcd->get_pcd_handle(), descriptor.bEndpointAddress | direction);
}

void STM32::Endpoint::on_data_in()
{
  auto PCD = dcd->get_PCD();
  auto hpcd = dcd->get_hpcd();
  auto wEPVal = (uint16_t)PCD_GET_ENDPOINT(PCD, epindex());

  auto ep = &hpcd->IN_ep[epindex()];

  /* clear int flag */
  PCD_CLEAR_TX_EP_CTR(PCD, epindex());

  /* Multi-packet on the NON control IN endpoint */
  auto TxPctSize = (uint16_t)PCD_GET_EP_TX_CNT(PCD, ep->num);

  if (ep->xfer_len > TxPctSize)
  {
    ep->xfer_len -= TxPctSize;
  }
  else
  {
    ep->xfer_len = 0U;
  }

  /* Zero Length Packet? */
  if (ep->xfer_len == 0U)
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
    ep->xfer_buff += TxPctSize;
    ep->xfer_count += TxPctSize;
    start_transfer(ep);
  }
}

void STM32::Endpoint::on_data_out()
{
  auto PCD = dcd->get_PCD();
  auto hpcd = dcd->get_hpcd();

  PCD_CLEAR_RX_EP_CTR(PCD, epindex());
  auto ep = &hpcd->OUT_ep[epindex()];

  /* OUT Single Buffering */
  assert(ep->doublebuffer == 0U);

  auto count = (uint16_t)PCD_GET_EP_RX_CNT(PCD, ep->num);

  if (count != 0U)
  {
    read_pma(ep->xfer_buff, ep->pmaadress, count);
  }

  /* multi-packet on the NON control OUT endpoint */
  ep->xfer_count += count;

  if ((ep->xfer_len == 0U) || (count < ep->maxpacket))
  {
    transfer.actual_length = HAL_PCD_EP_GetRxCount(hpcd, epindex());

    transfer.complete(UX_SUCCESS);
  }
  else
  {
     ep->xfer_buff += count;
     start_transfer(ep);
  }
}


void STM32::Endpoint::on_interrupt()
{
  auto PCD = dcd->get_PCD();

  auto wEPVal = (uint16_t)PCD_GET_ENDPOINT(PCD, epindex());

  if ((wEPVal & USB_EP_VTRX) != 0U)
  {
    on_data_out();
  }

  if ((wEPVal & USB_EP_VTTX) != 0U)
  {
    on_data_in();
  }
}

uint16_t STM32::Endpoint::receive(PCD_EPTypeDef *ep, uint16_t wEPVal)
{
  auto PCD = dcd->get_PCD();

  uint16_t count;

  /* Manage Buffer0 OUT */
  if ((wEPVal & USB_EP_DTOG_RX) != 0U)
  {
    /* Get count of received Data on buffer0 */
    count = (uint16_t)PCD_GET_EP_DBUF0_CNT(PCD, ep->num);

    if (ep->xfer_len >= count)
    {
      ep->xfer_len -= count;
    }
    else
    {
      ep->xfer_len = 0U;
    }

    if (ep->xfer_len == 0U)
    {
      /* Set NAK to OUT endpoint since double buffer is enabled */
      PCD_SET_EP_RX_STATUS(PCD, ep->num, USB_EP_RX_NAK);
    }

    /* Check if Buffer1 is in blocked state which requires to toggle */
    if ((wEPVal & USB_EP_DTOG_TX) != 0U)
    {
      PCD_FREE_USER_BUFFER(PCD, ep->num, 0U);
    }

    if (count != 0U)
    {
      read_pma(ep->xfer_buff, ep->pmaaddr0, count);
    }
  }
  /* Manage Buffer 1 DTOG_RX=0 */
  else
  {
    /* Get count of received data */
    count = (uint16_t)PCD_GET_EP_DBUF1_CNT(PCD, ep->num);

    if (ep->xfer_len >= count)
    {
      ep->xfer_len -= count;
    }
    else
    {
      ep->xfer_len = 0U;
    }

    if (ep->xfer_len == 0U)
    {
      /* Set NAK on the current endpoint */
      PCD_SET_EP_RX_STATUS(PCD, ep->num, USB_EP_RX_NAK);
    }

    /* Need to FreeUser Buffer */
    if ((wEPVal & USB_EP_DTOG_TX) == 0U)
    {
      PCD_FREE_USER_BUFFER(PCD, ep->num, 0U);
    }

    if (count != 0U)
    {
      read_pma(ep->xfer_buff, ep->pmaaddr1, count);
    }
  }

  return count;
}


HAL_StatusTypeDef STM32::Endpoint::transmit(PCD_EPTypeDef *ep, uint16_t wEPVal)
{
  auto PCD = dcd->get_PCD();

  uint32_t len;
  uint16_t TxPctSize;

  /* Data Buffer0 ACK received */
  if ((wEPVal & USB_EP_DTOG_TX) != 0U)
  {
    /* multi-packet on the NON control IN endpoint */
    TxPctSize = (uint16_t)PCD_GET_EP_DBUF0_CNT(PCD, ep->num);

    if (ep->xfer_len > TxPctSize)
    {
      ep->xfer_len -= TxPctSize;
    }
    else
    {
      ep->xfer_len = 0U;
    }

    /* Transfer is completed */
    if (ep->xfer_len == 0U)
    {
      USB_DRD_SET_CHEP_DBUF0_CNT(PCD, ep->num, ep->is_in, 0U);
      pcd_set_dbuf1_cnt(ep->num, ep->is_in, 0U);

      if (ep->type == EP_TYPE_BULK)
      {
        /* Set Bulk endpoint in NAK state */
        PCD_SET_EP_TX_STATUS(PCD, ep->num, USB_EP_TX_NAK);
      }

      /* TX COMPLETE */
      on_data_in();

      if ((wEPVal & USB_EP_DTOG_RX) != 0U)
      {
        PCD_FREE_USER_BUFFER(PCD, ep->num, 1U);
      }

      return HAL_OK;
    }
    else /* Transfer is not yet Done */
    {
      /* Need to Free USB Buffer */
      if ((wEPVal & USB_EP_DTOG_RX) != 0U)
      {
        PCD_FREE_USER_BUFFER(PCD, ep->num, 1U);
      }

      /* Still there is data to Fill in the next Buffer */
      if (ep->xfer_fill_db == 1U)
      {
        ep->xfer_buff += TxPctSize;
        ep->xfer_count += TxPctSize;

        /* Calculate the len of the new buffer to fill */
        if (ep->xfer_len_db >= ep->maxpacket)
        {
          len = ep->maxpacket;
          ep->xfer_len_db -= len;
        }
        else if (ep->xfer_len_db == 0U)
        {
          len = TxPctSize;
          ep->xfer_fill_db = 0U;
        }
        else
        {
          ep->xfer_fill_db = 0U;
          len = ep->xfer_len_db;
          ep->xfer_len_db = 0U;
        }

        /* Write remaining Data to Buffer */
        /* Set the Double buffer counter for pma buffer0 */
        PCD_SET_EP_DBUF0_CNT(PCD, ep->num, ep->is_in, len);

        /* Copy user buffer to USB PMA */
        write_pma(ep->pmaaddr0, ep->xfer_buff, (uint16_t)len);
      }
    }
  }
  else /* Data Buffer1 ACK received */
  {
    /* multi-packet on the NON control IN endpoint */
    TxPctSize = (uint16_t)PCD_GET_EP_DBUF1_CNT(PCD, ep->num);

    if (ep->xfer_len >= TxPctSize)
    {
      ep->xfer_len -= TxPctSize;
    }
    else
    {
      ep->xfer_len = 0U;
    }

    /* Transfer is completed */
    if (ep->xfer_len == 0U)
    {
      PCD_SET_EP_DBUF0_CNT(PCD, ep->num, ep->is_in, 0U);
      PCD_SET_EP_DBUF1_CNT(PCD, ep->num, ep->is_in, 0U);

      if (ep->type == EP_TYPE_BULK)
      {
        /* Set Bulk endpoint in NAK state */
        PCD_SET_EP_TX_STATUS(PCD, ep->num, USB_EP_TX_NAK);
      }

      /* TX COMPLETE */
      on_data_in();

      /* need to Free USB Buff */
      if ((wEPVal & USB_EP_DTOG_RX) == 0U)
      {
        PCD_FREE_USER_BUFFER(PCD, ep->num, 1U);
      }

      return HAL_OK;
    }
    else /* Transfer is not yet Done */
    {
      /* Need to Free USB Buffer */
      if ((wEPVal & USB_EP_DTOG_RX) == 0U)
      {
        PCD_FREE_USER_BUFFER(PCD, ep->num, 1U);
      }

      /* Still there is data to Fill in the next Buffer */
      if (ep->xfer_fill_db == 1U)
      {
        ep->xfer_buff += TxPctSize;
        ep->xfer_count += TxPctSize;

        /* Calculate the len of the new buffer to fill */
        if (ep->xfer_len_db >= ep->maxpacket)
        {
          len = ep->maxpacket;
          ep->xfer_len_db -= len;
        }
        else if (ep->xfer_len_db == 0U)
        {
          len = TxPctSize;
          ep->xfer_fill_db = 0U;
        }
        else
        {
          len = ep->xfer_len_db;
          ep->xfer_len_db = 0U;
          ep->xfer_fill_db = 0;
        }

        /* Set the Double buffer counter for pma buffer1 */
        PCD_SET_EP_DBUF1_CNT(PCD, ep->num, ep->is_in, len);

        /* Copy the user buffer to USB PMA */
        write_pma(ep->pmaaddr1, ep->xfer_buff, (uint16_t)len);
      }
    }
  }

  /* Enable endpoint IN */
  PCD_SET_EP_TX_STATUS(PCD, ep->num, USB_EP_TX_VALID);

  return HAL_OK;
}

void STM32::Endpoint::read_pma(uint8_t *buf, uint32_t pmaaddr, uint32_t len)
{
  assert(pmaaddr != 0);

  uint32_t *pout = (uint32_t *)buf;
  volatile uint32_t *pin = (volatile uint32_t *)(USB_DRD_PMAADDR + (uint32_t)pmaaddr);

  while (len >= 4) {
    *pout++ = *pin++;
    len -= 4;
  }

  if (len == 0)
    return;

  uint32_t v = *pin;
  buf = (uint8_t *)pout;

  while (len) {
    *buf++ = v & 0xFF;
    v >>= 8;
    len--;
  }
}

void STM32::Endpoint::write_pma(uint32_t pmaaddr, const uint8_t *buf, uint32_t len)
{
  assert(pmaaddr != 0);

  volatile uint32_t *pout = (volatile uint32_t *)(USB_DRD_PMAADDR + pmaaddr);
  const uint32_t *pin = (const uint32_t *)buf;

  while (len >= 4) {
    *pout++ = *pin++;
    len -= 4;
  }

  if (len == 0)
    return;

  uint32_t v = 0;
  buf = (const uint8_t *)pin;

  for (uint32_t i = 0; i < len; i++) {
    v |= *buf++ << (8 * i);
  }

  *pout = v;
}


void STM32::Endpoint::ll_receive(uint8_t *buf, uint32_t len)
{
  PCD_EPTypeDef *ep;

  ep = &dcd->get_hpcd()->OUT_ep[epindex()];

  /*setup and start the Xfer */
  ep->xfer_buff = buf;
  ep->xfer_len = len;
  ep->xfer_count = 0U;
  ep->is_in = 0U;
  ep->num = epindex();

  start_transfer(ep);
}

void STM32::Endpoint::ll_transmit(uint8_t *buf, uint32_t len)
{
  PCD_EPTypeDef *ep;

  ep = &dcd->get_hpcd()->IN_ep[epindex()];

  ep->xfer_buff = buf;
  ep->xfer_len = len;
  ep->xfer_fill_db = 1U;
  ep->xfer_len_db = len;
  ep->xfer_count = 0U;
  ep->is_in = 1U;
  ep->num = epindex();

  start_transfer(ep);

  event_log.push_event(UsbEvent::ENDPOINT_XMIT, epaddr);
}

void STM32::Endpoint::start_transfer(PCD_EPTypeDef *ep)
{
  auto PCD = dcd->get_PCD();
  uint32_t len;

  /* IN endpoint */
  if (ep->is_in == 1U)
  {
    /* Multi packet transfer */
    if (ep->xfer_len > ep->maxpacket)
    {
      len = ep->maxpacket;
    }
    else
    {
      len = ep->xfer_len;
    }

    if (ep->num) {
      int a = 0;
    }

    write_pma(ep->pmaadress, ep->xfer_buff,(uint16_t)len);

    (USB_DRD_PMA_BUFF + (ep->num))->TXBD &= 0xFFFF;
    (USB_DRD_PMA_BUFF + (ep->num))->TXBD |= (uint32_t)((uint32_t)(len) << 16U);

    PCD_SET_EP_TX_STATUS(PCD, ep->num, USB_EP_TX_VALID);
  }
  else /* OUT endpoint */
  {
    if ((ep->xfer_len == 0U) && (ep->type == EP_TYPE_CTRL))
    {
      PCD_SET_OUT_STATUS(PCD, ep->num);
    }
    else
    {
      PCD_CLEAR_OUT_STATUS(PCD, ep->num);
    }

    /* Multi packet transfer */
    if (ep->xfer_len > ep->maxpacket)
    {
      ep->xfer_len -= ep->maxpacket;
    }
    else
    {
      ep->xfer_len = 0U;
    }

    PCD_SET_EP_RX_STATUS(PCD, ep->num, USB_EP_RX_VALID);
  }
}

