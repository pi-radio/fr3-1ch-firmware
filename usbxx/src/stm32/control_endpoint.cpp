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

STM32::ControlEndpoint::ControlEndpoint(DeviceBase *_device, DCD *_dcd):
  STM32::Endpoint(_device, _dcd, 0),
  state(ControlEndpointState::IDLE),
  ack_mode(AckMode::NONE)
{

}

PCD_EPTypeDef *STM32::ControlEndpoint::get_epdata()
{
  return &dcd->get_hpcd()->IN_ep[epindex()];
}


void STM32::ControlEndpoint::init()
{
  transfer.endpoint = shared_from_this();
  transfer.timeout =  UX_MS_TO_TICK(UX_CONTROL_TRANSFER_TIMEOUT);

  /* Adjust the current data pointer as well.  */
  transfer.current_data_pointer = transfer.data;
}

UINT STM32::ControlEndpoint::destroy()
{
 reset_flags();

 if ((get_addr() & 0x7F) != 0)
   int a = 0;

 {
   auto g = dcd->guard();
   deactivate();
 }

  /* This function never fails.  */
 return 0;
}

void STM32::ControlEndpoint::ack_ctrl()
{
  switch(ack_mode)
  {
  case AckMode::NONE:
    assert(0);
    break;

  case AckMode::SETUP:
    state = ControlEndpointState::STATUS_RX;
    break;

  case AckMode::DATA_OUT:
    state = ControlEndpointState::STATUS_TX;
    break;

  case AckMode::DATA_IN:
    ack_mode = AckMode::NONE;
    return;
  }

  ll_transmit(nullptr, 0U);

  ack_mode = AckMode::NONE;
}

void STM32::ControlEndpoint::activate()
{
  auto hpcd = dcd->get_hpcd();

  auto PCD = dcd->get_PCD();

  auto ep = get_epdata();

  uint32_t wEpRegVal;

  wEpRegVal = PCD_GET_ENDPOINT(PCD, epindex()) & USB_EP_T_MASK;

  wEpRegVal |= USB_EP_CONTROL;

  PCD_SET_ENDPOINT(PCD, 0, (wEpRegVal | USB_EP_VTRX | USB_EP_VTTX));
  PCD_SET_EP_ADDRESS(PCD, 0, 0);

  auto pma_out = hpcd->OUT_ep[0].pmaadress;
  auto pma_in = hpcd->IN_ep[0].pmaadress;

  assert(ep->pmaadress != 0);

  /* Set the endpoint Receive buffer address */
  pcd_set_rx_address(0, pma_out);
  pcd_set_tx_address(0, pma_in);
  PCD_CLEAR_TX_DTOG(PCD, ep->num);

  /* Set the endpoint Receive buffer counter */
  pcd_set_rx_cnt(0, ep->maxpacket);
  PCD_CLEAR_RX_DTOG(PCD, ep->num);

  PCD_SET_EP_RX_STATUS(PCD, ep->num, USB_EP_RX_VALID);
  PCD_SET_EP_TX_STATUS(PCD, ep->num, USB_EP_TX_NAK);
}

void STM32::ControlEndpoint::deactivate()
{
  auto PCD = dcd->get_PCD();

  PCD_CLEAR_RX_DTOG(PCD, epindex());
  PCD_SET_EP_RX_STATUS(PCD, epindex(), USB_EP_RX_DIS);
}

void STM32::ControlEndpoint::open()
{
  auto hpcd = dcd->get_hpcd();

  hpcd->IN_ep[0].doublebuffer = 0;
  hpcd->IN_ep[0].pmaadress = 0x80;
  hpcd->IN_ep[0].is_in = true;
  hpcd->IN_ep[0].num = epindex();
  hpcd->IN_ep[0].maxpacket = descriptor.wMaxPacketSize & 0x7FFU;
  hpcd->IN_ep[0].type = UX_CONTROL_ENDPOINT;

  hpcd->OUT_ep[0].doublebuffer = 0;
  hpcd->OUT_ep[0].pmaadress = 0x40;
  hpcd->OUT_ep[0].is_in = false;
  hpcd->OUT_ep[0].num = epindex();
  hpcd->OUT_ep[0].maxpacket = descriptor.wMaxPacketSize & 0x7FFU;
  hpcd->OUT_ep[0].type = UX_CONTROL_ENDPOINT;

  {
    auto g = dcd->guard();

    activate();
  }

  state = ControlEndpointState::RESET;

  /* Ensure the control endpoint is properly reset.  */

  /* Mark the phase as SETUP.  */
  transfer.type =  TransferType::SETUP;

  /* Mark this transfer request as pending.  */
  transfer.set_pending();

  /* Ask for 8 bytes of the SETUP packet.  */
  transfer.requested_length =    UX_SETUP_SIZE;
  transfer.in_transfer_length =  UX_SETUP_SIZE;

  /* Reset the number of bytes sent/received.  */
  transfer.actual_length =  0;
}

UINT STM32::ControlEndpoint::create()
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

  /* Return successful completion.  */
  return 0;
}

void STM32::ControlEndpoint::clear_stall()
{
  auto PCD = dcd->get_PCD();
  auto g = dcd->guard();

  PCD_CLEAR_RX_DTOG(PCD, epindex());
  PCD_SET_EP_RX_STATUS(PCD, epindex(), USB_EP_RX_VALID);
}

void STM32::ControlEndpoint::stall()
{
  auto PCD = dcd->get_PCD();
  auto g = dcd->guard();

#if 0
  if (ep->is_in != 0U)
    PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_STALL);
  else
#endif
  PCD_SET_EP_RX_STATUS(dcd->get_PCD(), epindex(), USB_EP_RX_STALL);
}

void STM32::ControlEndpoint::abort_transfer()
{
  auto hpcd = dcd->get_hpcd();
  auto PCD = dcd->get_PCD();

  /* Configure NAK status for the Endpoint */
  PCD_SET_EP_RX_STATUS(PCD, epindex(), USB_EP_RX_NAK);
}

void STM32::ControlEndpoint::on_setup()
{
  transfer.actual_length = 0;
  transfer.type =  TransferType::SETUP;

  /* Mark the transfer as successful.  */
  transfer.complete(UX_SUCCESS);

  in_transfer = false;
  stalled = false;
  done = false;

  /* Check if the transaction is IN.  */
  if (*transfer.setup & UX_REQUEST_IN)
  {
    direction = UX_ENDPOINT_IN;
    state = ControlEndpointState::DATA_TX;
    ack_mode = AckMode::DATA_IN;

    device->process_control_event(&transfer);

    return;
  }

  if (*(transfer.setup + 6) == 0 &&
      *(transfer.setup + 7) == 0)
  {
    direction = UX_ENDPOINT_IN;

    ack_mode = AckMode::SETUP;

    device->process_control_event(&transfer);

    return;
  }

  direction  = UX_ENDPOINT_OUT;

  transfer.requested_length = usb_get_short(transfer.setup + 6);

  if (transfer.requested_length > UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH)
  {
    stall();

    state =  ControlEndpointState::IDLE;

    return;
  }


  transfer.actual_length =  0;
  transfer.current_data_pointer =  transfer.data;

  ll_receive(transfer.current_data_pointer,
      transfer.requested_length);

              /* Set the state to RX.  */
  state =  ControlEndpointState::DATA_RX;
}


void STM32::ControlEndpoint::on_data_in()
{
   /* Check if we need to send data again on control endpoint. */
  if (state == ControlEndpointState::DATA_TX)
  {
    ll_receive(0, 0);

    /* Are we done with this transfer ? */
    if (transfer.in_transfer_length <=
        descriptor.wMaxPacketSize)
    {

        /* There is no data to send but we may need to send a Zero Length Packet.  */
        if (transfer. force_zlp ==  UX_TRUE)
        {

            /* Arm a ZLP packet on IN.  */
            ll_transmit(0, 0);

            /* Reset the ZLP condition.  */
            transfer. force_zlp =  UX_FALSE;

        }
        else
        {
            transfer.completion_code =  UX_SUCCESS;
            transfer.status =  TransferStatus::COMPLETED;
            transfer.actual_length = transfer.requested_length;
            if (transfer.completion_function)
                transfer.completion_function (&transfer);

            state = ControlEndpointState::STATUS_RX;
        }
    }
    else
    {
      auto transfer_length = transfer. in_transfer_length - descriptor.wMaxPacketSize;

      if (transfer_length > descriptor.wMaxPacketSize)
      {
        transfer_length =  descriptor.wMaxPacketSize;
      }

      transfer.current_data_pointer += descriptor.wMaxPacketSize;
      transfer.in_transfer_length -= transfer_length;

      ll_transmit(transfer.current_data_pointer,
                  transfer_length);
    }
  }

}

void STM32::ControlEndpoint::on_data_out()
{
  auto hpcd = dcd->get_hpcd();

  /* Check if we have received something on endpoint 0 during data phase .  */
  if (state == ControlEndpointState::DATA_RX)
  {
    auto transfer_length = HAL_PCD_EP_GetRxCount(hpcd, 0);

    transfer.actual_length += transfer_length;

    /* Can we accept this much?  */
    if (transfer.actual_length <= transfer.requested_length)
    {
          /* Are we done with this transfer ? */
          if ((transfer.actual_length == transfer.requested_length) ||
              (transfer_length != descriptor.wMaxPacketSize))
          {
            transfer.complete(UX_SUCCESS);
            direction = UX_ENDPOINT_IN;
            ack_mode = AckMode::DATA_OUT;

            device->process_control_event(&transfer);
          }
          else
          {
              transfer.current_data_pointer += descriptor.wMaxPacketSize;
              ll_receive(transfer.current_data_pointer,
                          descriptor.wMaxPacketSize);
          }
      }
      else
      {
        transfer.completion_code = UX_TRANSFER_BUFFER_OVERFLOW;

        /* We are using a Control endpoint, if there is a callback, invoke it. We are still under ISR.  */
        if (transfer.completion_function)
          transfer.completion_function(&transfer) ;
      }
  }


}

void STM32::ControlEndpoint::on_interrupt()
{
  auto PCD = dcd->get_PCD();
  auto hpcd = dcd->get_hpcd();

  PCD_EPTypeDef *ep;
  auto wIstr = (uint16_t)PCD->ISTR;
  uint16_t wEPVal;

  if ((wIstr & USB_ISTR_DIR) == 0U)
  {
    PCD_CLEAR_TX_EP_CTR(PCD, PCD_ENDP0);
    ep = &hpcd->IN_ep[0];

    ep->xfer_count = PCD_GET_EP_TX_CNT(PCD, ep->num);
    ep->xfer_buff += ep->xfer_count;

    /* TX COMPLETE */
    on_data_in();

    if ((ep->xfer_len == 0U) && !dcd->address_set)
    {
      PCD->DADDR = ((uint16_t)dcd->get_device_address() | USB_DADDR_EF);
      dcd->address_set = true;
    }

    return;
  }

  /* DIR = 1 */

  /* DIR = 1 & CTR_RX => SETUP or OUT int */
  /* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */
  ep = &hpcd->OUT_ep[0];
  wEPVal = (uint16_t)PCD_GET_ENDPOINT(PCD, PCD_ENDP0);

  if ((wEPVal & USB_EP_SETUP) != 0U)
  {
    /* Get SETUP Packet */
    ep->xfer_count = PCD_GET_EP_RX_CNT(PCD, ep->num);

    if (ep->xfer_count != 8U)
    {
      /* Set Stall condition for EP0 IN/OUT */
      PCD_SET_EP_RX_STATUS(PCD, PCD_ENDP0, USB_EP_RX_STALL);
      PCD_SET_EP_TX_STATUS(PCD, PCD_ENDP0, USB_EP_TX_STALL);

      /* SETUP bit kept frozen while CTR_RX = 1 */
      PCD_CLEAR_RX_EP_CTR(PCD, PCD_ENDP0);

      return;
    }

    read_pma(transfer.setup,
                ep->pmaadress, (uint16_t)ep->xfer_count);

    /* SETUP bit kept frozen while CTR_RX = 1 */
    PCD_CLEAR_RX_EP_CTR(PCD, PCD_ENDP0);

    /* Process SETUP Packet*/
    on_setup();

    return;
  }


  if ((wEPVal & USB_EP_VTRX) != 0U)
  {
    PCD_CLEAR_RX_EP_CTR(PCD, PCD_ENDP0);

    /* Get Control Data OUT Packet */
    ep->xfer_count = PCD_GET_EP_RX_CNT(PCD, ep->num);

    if (ep->xfer_count == 0U)
    {
      /* Status phase re-arm for next setup */
      PCD_SET_EP_RX_STATUS(PCD, PCD_ENDP0, USB_EP_RX_VALID);
    }
    else
    {
      if (ep->xfer_buff != 0U)
      {
        read_pma(ep->xfer_buff,
                    ep->pmaadress, (uint16_t)ep->xfer_count);  /* max 64bytes */

        ep->xfer_buff += ep->xfer_count;

        /* Process Control Data OUT Packet */
        on_data_out();
      }
    }
  }
}
