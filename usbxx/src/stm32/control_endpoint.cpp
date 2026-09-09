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
  ack_mode(AckMode::NONE)
{
  HAL_PCDEx_PMAConfig(dcd->get_hpcd(), 0x00, PCD_SNG_BUF, 0x40);
  HAL_PCDEx_PMAConfig(dcd->get_hpcd(), 0x80, PCD_SNG_BUF, 0x80);
}


void STM32::ControlEndpoint::ack_ctrl()
{
  switch(ack_mode)
  {
  case AckMode::NONE:
    assert(0);
    break;

  case AckMode::SETUP:
    state = STM32::EndpointState::STATUS_RX;
    break;

  case AckMode::DATA_OUT:
    state = STM32::EndpointState::STATUS_TX;
    break;

  case AckMode::DATA_IN:
    ack_mode = AckMode::NONE;
    return;
  }

  ll_transmit(nullptr, 0U);

  ack_mode = AckMode::NONE;
}

void STM32::ControlEndpoint::on_setup()
{
  auto hpcd = dcd->get_hpcd();

  /* Clear the length of the data received.  */
  transfer.actual_length = 0;

  /* Mark the phase as SETUP.  */
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
    state = STM32::EndpointState::DATA_TX;
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

    state =  EndpointState::IDLE;

    return;
  }


  transfer.actual_length =  0;
  transfer.current_data_pointer =  transfer.data;

  HAL_PCD_EP_Receive(hpcd,
      descriptor.bEndpointAddress,
      transfer.current_data_pointer,
      transfer.requested_length);

              /* Set the state to RX.  */
  state =  EndpointState::DATA_RX;
}


void STM32::ControlEndpoint::on_data_in()
{
  auto hpcd = dcd->get_hpcd();

  ULONG             transfer_length;

   /* Check if we need to send data again on control endpoint. */
  if (state == EndpointState::DATA_TX)
  {
    HAL_PCD_EP_Receive(hpcd, 0, 0, 0);

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

            state = EndpointState::STATUS_RX;
        }
    }
    else
    {

        /* Get the size of the transfer.  */
        transfer_length = transfer. in_transfer_length - descriptor.wMaxPacketSize;

        /* Check if the endpoint size is bigger that data requested. */
        if (transfer_length > descriptor.wMaxPacketSize)
        {

            /* Adjust the transfer size.  */
            transfer_length =  descriptor.wMaxPacketSize;
        }

        /* Adjust the data pointer.  */
        transfer. current_data_pointer += descriptor.wMaxPacketSize;

        /* Adjust the transfer length remaining.  */
        transfer. in_transfer_length -= transfer_length;

        /* Transmit data.  */
        ll_transmit(transfer.current_data_pointer,
                    transfer_length);
    }
  }

}

void STM32::ControlEndpoint::on_data_out()
{
  auto hpcd = dcd->get_hpcd();

  /* Check if we have received something on endpoint 0 during data phase .  */
  if (state == EndpointState::DATA_RX)
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
              HAL_PCD_EP_Receive(hpcd,
                          descriptor.bEndpointAddress,
                          transfer.current_data_pointer,
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

    if ((hpcd->USB_Address > 0U) && (ep->xfer_len == 0U))
    {
      PCD->DADDR = ((uint16_t)hpcd->USB_Address | USB_DADDR_EF);
      hpcd->USB_Address = 0U;
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

    USB_ReadPMA(PCD, transfer.setup,
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
        USB_ReadPMA(PCD, ep->xfer_buff,
                    ep->pmaadress, (uint16_t)ep->xfer_count);  /* max 64bytes */

        ep->xfer_buff += ep->xfer_count;

        /* Process Control Data OUT Packet */
        on_data_out();
      }
    }
  }
}
