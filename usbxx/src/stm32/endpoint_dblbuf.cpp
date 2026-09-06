#if 0
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

STM32::Endpoint::Endpoint(DeviceBase *_device, DCD *_dcd, uint8_t _index) :
  USBXX::Endpoint(_device),
  transfer(),
  state(EndpointState::IDLE),
  index(_index),
  direction(0),
  dcd(_dcd)
{
  if (index != 0) {
    int a = 0;
  }
  reset_flags();
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

  /* Attach the interface to the endpoint.  */


  /* Attach the device to the endpoint.  */


  direction = descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION;

  if (index != 0)
  {
    HAL_PCD_EP_Open(dcd->get_pcd_handle(), descriptor.bEndpointAddress,
                    descriptor.wMaxPacketSize,
                    descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE);
  }

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
  HAL_PCD_EP_Flush(dcd->get_pcd_handle(), descriptor.bEndpointAddress);
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

  /* Flush buffer.  */
  HAL_PCD_EP_Flush(pcd_handle, descriptor.bEndpointAddress);

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
  auto hpcd = dcd->get_hpcd();

  /* Check if a ZLP should be armed.  */
  if (transfer.force_zlp &&
      transfer.requested_length)
  {
    transfer.force_zlp = UX_FALSE;
    transfer.in_transfer_length = 0;

    /* Arm a ZLP packet on IN.  */
    HAL_PCD_EP_Transmit(hpcd, epindex(), 0, 0);
  }
  else
  {
    transfer.actual_length = transfer.requested_length;

  /* Non control endpoint operation, use semaphore.  */
    transfer.complete(UX_SUCCESS);
  }

}

void STM32::Endpoint::on_data_out()
{
  auto hpcd = dcd->get_hpcd();

  transfer.actual_length = HAL_PCD_EP_GetRxCount(hpcd, epindex());

  transfer.complete(UX_SUCCESS);
}


void STM32::Endpoint::on_interrupt()
{
  uint32_t count;
  auto PCD = dcd->get_PCD();
  auto hpcd = dcd->get_hpcd();

  auto wEPVal = (uint16_t)PCD_GET_ENDPOINT(PCD, epindex());

  if ((wEPVal & USB_EP_VTRX) != 0U)
  {
    /* clear int flag */
    PCD_CLEAR_RX_EP_CTR(PCD, epindex());
    auto ep = &hpcd->OUT_ep[epindex()];

    /* OUT Single Buffering */
    if (ep->doublebuffer == 0U)
    {
      count = (uint16_t)PCD_GET_EP_RX_CNT(PCD, ep->num);

      if (count != 0U)
      {
        USB_ReadPMA(PCD, ep->xfer_buff, ep->pmaadress, count);
      }
    }
    else
    {
      /* manage double buffer bulk out */
      if (ep->type == EP_TYPE_BULK)
      {
        count = receive(ep, wEPVal);
      }
      else /* manage double buffer iso out */
      {
        /* free EP OUT Buffer */
        PCD_FREE_USER_BUFFER(PCD, ep->num, 0U);

        if ((PCD_GET_ENDPOINT(PCD, ep->num) & USB_EP_DTOG_RX) != 0U)
        {
          /* read from endpoint BUF0Addr buffer */
          count = (uint16_t)PCD_GET_EP_DBUF0_CNT(PCD, ep->num);

          if (count != 0U)
          {
            USB_ReadPMA(PCD, ep->xfer_buff, ep->pmaaddr0, count);
          }
        }
        else
        {
          /* read from endpoint BUF1Addr buffer */
          count = (uint16_t)PCD_GET_EP_DBUF1_CNT(PCD, ep->num);

          if (count != 0U)
          {
            USB_ReadPMA(PCD, ep->xfer_buff, ep->pmaaddr1, count);
          }
        }
      }
    }

    /* multi-packet on the NON control OUT endpoint */
    ep->xfer_count += count;

    if ((ep->xfer_len == 0U) || (count < ep->maxpacket))
    {
      /* RX COMPLETE */
      on_data_out();
    }
    else
    {
       ep->xfer_buff += count;
      (void)USB_EPStartXfer(PCD, ep);
    }
  }

  if ((wEPVal & USB_EP_VTTX) != 0U)
  {
    auto ep = &hpcd->IN_ep[epindex()];

    /* clear int flag */
    PCD_CLEAR_TX_EP_CTR(PCD, epindex());

    if (ep->type == EP_TYPE_ISOC)
    {
      ep->xfer_len = 0U;

#if (USE_USB_DOUBLE_BUFFER == 1U)
      if (ep->doublebuffer != 0U)
      {
        if ((wEPVal & USB_EP_DTOG_TX) != 0U)
        {
          PCD_SET_EP_DBUF0_CNT(PCD, ep->num, ep->is_in, 0U);
        }
        else
        {
          PCD_SET_EP_DBUF1_CNT(PCD, ep->num, ep->is_in, 0U);
        }
      }
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */

      /* TX COMPLETE */
      on_data_in();
    }
    else
    {
      /* Manage Single Buffer Transaction */
      if ((wEPVal & USB_EP_KIND) == 0U)
      {
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
          /* TX COMPLETE */
          on_data_in();
        }
        else
        {
          /* Transfer is not yet Done */
          ep->xfer_buff += TxPctSize;
          ep->xfer_count += TxPctSize;
          (void)USB_EPStartXfer(PCD, ep);
        }

      }
      else
      {
        transmit(ep, wEPVal);
      }
    }
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
      USB_ReadPMA(PCD, ep->xfer_buff, ep->pmaaddr0, count);
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
      USB_ReadPMA(PCD, ep->xfer_buff, ep->pmaaddr1, count);
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
        USB_WritePMA(PCD, ep->xfer_buff,  ep->pmaaddr0, (uint16_t)len);
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
        USB_WritePMA(PCD, ep->xfer_buff,  ep->pmaaddr1, (uint16_t)len);
      }
    }
  }

  /* Enable endpoint IN */
  PCD_SET_EP_TX_STATUS(PCD, ep->num, USB_EP_TX_VALID);

  return HAL_OK;
}


#endif
