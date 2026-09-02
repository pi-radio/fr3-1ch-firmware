
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

#include <format>
#include <stdexcept>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/ux_device_stack.h>

#include <usbxx/device.hpp>

using namespace USBXX;

USBXX::STM32::DCD *USBXX::STM32::gDCD;

STM32::DCD::DCD(PCD_TypeDef *_pcd) : pcd(_pcd)
{
  STM32::gDCD = this;
};

void STM32::DCD::low_level_init()
{

}

uint32_t STM32::DCD::initialize()
{
  pcd_handle = &hpcd;

  hpcd.Instance = pcd;
  hpcd.Init.dev_endpoints = 8;
  hpcd.Init.speed = USBD_FS_SPEED;
  hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd.Init.Sof_enable = DISABLE;
  hpcd.Init.low_power_enable = DISABLE;
  hpcd.Init.lpm_enable = DISABLE;
  hpcd.Init.battery_charging_enable = DISABLE;
  hpcd.Init.vbus_sensing_enable = DISABLE;
  hpcd.Init.bulk_doublebuffer_enable = DISABLE;
  hpcd.Init.iso_singlebuffer_enable = DISABLE;

  if (HAL_PCD_Init(&hpcd) != HAL_OK)
  {
    throw std::runtime_error("Unable to initialize USB stack");
  }

  for (int i = 0; i < 8; i++) {
    ep_in[i].index = i;
    ep_out[i].index = i;
    ep_in[i].dcd = this;
    ep_out[i].dcd = this;

    ep_in[i].ux_slave_endpoint_device = device;
    ep_out[i].ux_slave_endpoint_device = device;

    /* Create the semaphore for the endpoint.  */
    if (_ux_device_semaphore_create(&ep_in[i].ux_slave_endpoint_transfer_request.semaphore,
                                        (char *)"ux_transfer_request_semaphore", 0) != 0) {
      throw std::runtime_error("Failed to create semaphore for in endpoint");
    }

    ep_in[i].ux_slave_endpoint_transfer_request.data =
                    (UCHAR *)::malloc(UX_SLAVE_REQUEST_DATA_MAX_LENGTH);


    if(_ux_device_semaphore_create(&ep_out[i].ux_slave_endpoint_transfer_request.semaphore,
        (char *)"ux_transfer_request_semaphore", 0) != 0) {
      throw std::runtime_error("Failed to create semaphore for in endpoint");
    }

    ep_out[i].ux_slave_endpoint_transfer_request.data =
                    (UCHAR *)::malloc(UX_SLAVE_REQUEST_DATA_MAX_LENGTH);

  }

  ep_in[0].used = true;
  ep_out[0].used = true;

  HAL_PCDEx_PMAConfig(&hpcd, 0x00 , PCD_SNG_BUF, 0x40);
  HAL_PCDEx_PMAConfig(&hpcd, 0x80 , PCD_SNG_BUF, 0x80);
  HAL_PCDEx_PMAConfig(&hpcd, 0x01, PCD_SNG_BUF, 0xC0);
  HAL_PCDEx_PMAConfig(&hpcd, 0x81, PCD_SNG_BUF, 0x100);
  HAL_PCDEx_PMAConfig(&hpcd, 0x82, PCD_SNG_BUF, 0x140);

  ux_slave_dcd_status =  UX_DCD_STATUS_OPERATIONAL;

  hpcd.Lock = HAL_LOCKED;

  USB_EnableGlobalInt(hpcd.Instance);
  USB_DevConnect(hpcd.Instance);

  hpcd.Lock = HAL_UNLOCKED;

  /* Return successful completion.  */
  return 0;
}

USBXX::Endpoint *STM32::DCD::allocate_endpoint(const EndpointDescriptor &desc)
{
  Endpoint *retval;

  uint8_t epaddr = desc.bEndpointAddress;

  bool is_in = epaddr & 0x80;
  uint8_t epidx = epaddr & 0xF;

  if (is_in) {
    retval = &ep_in[epidx];
  } else {
    retval = &ep_out[epidx];
    if (ep_out[epidx].used) {
      throw std::runtime_error("Unable to allocate out endpoint");
    }

    retval = &ep_out[epidx];
  }

  if (retval->used) {
    throw std::runtime_error(std::format("Unable to allocate endpoint {}", epaddr));
  }

  if ((desc.bEndpointAddress & 0x7F) != 0) {
    int a = 0;
  }
  retval->ux_slave_endpoint_descriptor = desc;
  retval->used = true;

  return retval;
}

UINT  STM32::DCD::complete_initialization()
{
  UX_SLAVE_TRANSFER       *xfer;

#if 0
  /* Are we in DFU mode ? If so, check if we are in a Reset mode.  */
  if (_ux_system_slave->ux_system_slave_device_dfu_state_machine == UX_SYSTEM_DFU_STATE_APP_DETACH)
  {
    /* The device is now in DFU reset mode. Switch to the DFU device framework.  */
    _ux_system_slave->ux_system_slave_device_framework =  _ux_system_slave -> ux_system_slave_dfu_framework;
    _ux_system_slave->ux_system_slave_device_framework_length =  _ux_system_slave -> ux_system_slave_dfu_framework_length;
  }
  else
  {

  /* Set State to App Idle. */
    _ux_system_slave -> ux_system_slave_device_dfu_state_machine = UX_SYSTEM_DFU_STATE_APP_IDLE;

    /* Check the speed and set the correct descriptor.  */
    if (_ux_system_slave -> ux_system_slave_speed ==  UX_FULL_SPEED_DEVICE)
    {

      /* The device is operating at full speed.  */
      _ux_system_slave -> ux_system_slave_device_framework =  _ux_system_slave -> ux_system_slave_device_framework_full_speed;
      _ux_system_slave -> ux_system_slave_device_framework_length =  _ux_system_slave -> ux_system_slave_device_framework_length_full_speed;
    }
    else
    {

      /* The device is operating at high speed.  */
      _ux_system_slave -> ux_system_slave_device_framework =  _ux_system_slave -> ux_system_slave_device_framework_high_speed;
      _ux_system_slave -> ux_system_slave_device_framework_length =  _ux_system_slave -> ux_system_slave_device_framework_length_high_speed;
    }
  }
#endif


  device->descriptor = read_in_descriptor<DeviceDescriptor>(device->get_current_descriptor().get_desc());
#if 0
  /* And create the decompressed device descriptor structure.  */
  _ux_utility_descriptor_parse(device_framework,
                          _ux_system_device_descriptor_structure,
                          UX_DEVICE_DESCRIPTOR_ENTRIES,
                          (UCHAR *) &device -> descriptor);
#endif

  /* Now we create a transfer request to accept the first SETUP packet
  and get the ball running. First get the address of the endpoint
  transfer request container.  */
  xfer = device->get_control_transfer();

  /* Set the timeout to be for Control Endpoint.  */
  xfer->timeout =  UX_MS_TO_TICK(UX_CONTROL_TRANSFER_TIMEOUT);

  /* Adjust the current data pointer as well.  */
  xfer -> current_data_pointer =
                      xfer -> data;

  /* Update the transfer request endpoint pointer with the default endpoint.  */

  auto control_endpoint = get_control_endpoint();

  xfer -> endpoint =  control_endpoint;

  /* The control endpoint max packet size needs to be filled manually in its descriptor.  */
  xfer -> endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize =
                          device -> descriptor.bMaxPacketSize0;

  /* On the control endpoint, always expect the maximum.  */
  xfer -> requested_length =
                          device -> descriptor.bMaxPacketSize0;

  /* Create the default control endpoint attached to the device.
  Once this endpoint is enabled, the host can then send a setup packet
  The device controller will receive it and will call the setup function
  module.  */
  control_endpoint->create();

  /* Open Control OUT endpoint.  */
  HAL_PCD_EP_Flush(pcd_handle, 0x00U);
  HAL_PCD_EP_Open(pcd_handle, 0x00U, device -> descriptor.bMaxPacketSize0, UX_CONTROL_ENDPOINT);

  /* Open Control IN endpoint.  */
  HAL_PCD_EP_Flush(pcd_handle, 0x80U);
  HAL_PCD_EP_Open(pcd_handle, 0x80U, device -> descriptor.bMaxPacketSize0, UX_CONTROL_ENDPOINT);

  /* Ensure the control endpoint is properly reset.  */
  control_endpoint->ux_slave_endpoint_state = UX_ENDPOINT_RESET;

  /* Mark the phase as SETUP.  */
  xfer -> type =  TransferType::SETUP;

  /* Mark this transfer request as pending.  */
  xfer -> status =  UX_TRANSFER_STATUS_PENDING;

  /* Ask for 8 bytes of the SETUP packet.  */
  xfer -> requested_length =    UX_SETUP_SIZE;
  xfer -> in_transfer_length =  UX_SETUP_SIZE;

  /* Reset the number of bytes sent/received.  */
  xfer -> actual_length =  0;

  /* Check the status change callback.  */
  device->on_attached();

  return 0;
}

UINT  STM32::DCD::uninitialize()
{
    /* Set the state of the controller to HALTED now.  */
  ux_slave_dcd_status =  UX_DCD_STATUS_HALTED;

  return 0;
}



void STM32::DCD::set_device_address(uint8_t addr)
{
  HAL_PCD_SetAddress(pcd_handle, addr);
}

uint16_t STM32::DCD::receive(PCD_EPTypeDef *ep, uint16_t wEPVal)
{
  uint16_t count;

  /* Manage Buffer0 OUT */
  if ((wEPVal & USB_EP_DTOG_RX) != 0U)
  {
    /* Get count of received Data on buffer0 */
    count = (uint16_t)PCD_GET_EP_DBUF0_CNT(hpcd.Instance, ep->num);

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
      PCD_SET_EP_RX_STATUS(hpcd.Instance, ep->num, USB_EP_RX_NAK);
    }

    /* Check if Buffer1 is in blocked state which requires to toggle */
    if ((wEPVal & USB_EP_DTOG_TX) != 0U)
    {
      PCD_FREE_USER_BUFFER(hpcd.Instance, ep->num, 0U);
    }

    if (count != 0U)
    {
      USB_ReadPMA(hpcd.Instance, ep->xfer_buff, ep->pmaaddr0, count);
    }
  }
  /* Manage Buffer 1 DTOG_RX=0 */
  else
  {
    /* Get count of received data */
    count = (uint16_t)PCD_GET_EP_DBUF1_CNT(hpcd.Instance, ep->num);

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
      PCD_SET_EP_RX_STATUS(hpcd.Instance, ep->num, USB_EP_RX_NAK);
    }

    /* Need to FreeUser Buffer */
    if ((wEPVal & USB_EP_DTOG_TX) == 0U)
    {
      PCD_FREE_USER_BUFFER(hpcd.Instance, ep->num, 0U);
    }

    if (count != 0U)
    {
      USB_ReadPMA(hpcd.Instance, ep->xfer_buff, ep->pmaaddr1, count);
    }
  }

  return count;
}


/**
  * @brief  Manage double buffer bulk IN transaction from ISR
  * @param  hpcd PCD handle
  * @param  ep current endpoint handle
  * @param  wEPVal Last snapshot of EPRx register value taken in ISR
  * @retval HAL status
  */
HAL_StatusTypeDef STM32::DCD::transmit(PCD_EPTypeDef *ep, uint16_t wEPVal)
{
  uint32_t len;
  uint16_t TxPctSize;

  /* Data Buffer0 ACK received */
  if ((wEPVal & USB_EP_DTOG_TX) != 0U)
  {
    /* multi-packet on the NON control IN endpoint */
    TxPctSize = (uint16_t)PCD_GET_EP_DBUF0_CNT(hpcd.Instance, ep->num);

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
      PCD_SET_EP_DBUF0_CNT(hpcd.Instance, ep->num, ep->is_in, 0U);
      PCD_SET_EP_DBUF1_CNT(hpcd.Instance, ep->num, ep->is_in, 0U);

      if (ep->type == EP_TYPE_BULK)
      {
        /* Set Bulk endpoint in NAK state */
        PCD_SET_EP_TX_STATUS(hpcd.Instance, ep->num, USB_EP_TX_NAK);
      }

      /* TX COMPLETE */
      on_data_in(ep->num);

      if ((wEPVal & USB_EP_DTOG_RX) != 0U)
      {
        PCD_FREE_USER_BUFFER(hpcd.Instance, ep->num, 1U);
      }

      return HAL_OK;
    }
    else /* Transfer is not yet Done */
    {
      /* Need to Free USB Buffer */
      if ((wEPVal & USB_EP_DTOG_RX) != 0U)
      {
        PCD_FREE_USER_BUFFER(hpcd.Instance, ep->num, 1U);
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
        PCD_SET_EP_DBUF0_CNT(hpcd.Instance, ep->num, ep->is_in, len);

        /* Copy user buffer to USB PMA */
        USB_WritePMA(hpcd.Instance, ep->xfer_buff,  ep->pmaaddr0, (uint16_t)len);
      }
    }
  }
  else /* Data Buffer1 ACK received */
  {
    /* multi-packet on the NON control IN endpoint */
    TxPctSize = (uint16_t)PCD_GET_EP_DBUF1_CNT(hpcd.Instance, ep->num);

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
      PCD_SET_EP_DBUF0_CNT(hpcd.Instance, ep->num, ep->is_in, 0U);
      PCD_SET_EP_DBUF1_CNT(hpcd.Instance, ep->num, ep->is_in, 0U);

      if (ep->type == EP_TYPE_BULK)
      {
        /* Set Bulk endpoint in NAK state */
        PCD_SET_EP_TX_STATUS(hpcd.Instance, ep->num, USB_EP_TX_NAK);
      }

      /* TX COMPLETE */
      on_data_in(ep->num);

      /* need to Free USB Buff */
      if ((wEPVal & USB_EP_DTOG_RX) == 0U)
      {
        PCD_FREE_USER_BUFFER(hpcd.Instance, ep->num, 1U);
      }

      return HAL_OK;
    }
    else /* Transfer is not yet Done */
    {
      /* Need to Free USB Buffer */
      if ((wEPVal & USB_EP_DTOG_RX) == 0U)
      {
        PCD_FREE_USER_BUFFER(hpcd.Instance, ep->num, 1U);
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
        PCD_SET_EP_DBUF1_CNT(hpcd.Instance, ep->num, ep->is_in, len);

        /* Copy the user buffer to USB PMA */
        USB_WritePMA(hpcd.Instance, ep->xfer_buff,  ep->pmaaddr1, (uint16_t)len);
      }
    }
  }

  /* Enable endpoint IN */
  PCD_SET_EP_TX_STATUS(hpcd.Instance, ep->num, USB_EP_TX_VALID);

  return HAL_OK;
}

void STM32::DCD::control_IRQ()
{
  PCD_EPTypeDef *ep;
  auto wIstr = (uint16_t)hpcd.Instance->ISTR;
  uint16_t wEPVal;

  if ((wIstr & USB_ISTR_DIR) == 0U)
  {
    PCD_CLEAR_TX_EP_CTR(hpcd.Instance, PCD_ENDP0);
    ep = &hpcd.IN_ep[0];

    ep->xfer_count = PCD_GET_EP_TX_CNT(hpcd.Instance, ep->num);
    ep->xfer_buff += ep->xfer_count;

    /* TX COMPLETE */
    on_data_in(0);

    if ((hpcd.USB_Address > 0U) && (ep->xfer_len == 0U))
    {
      hpcd.Instance->DADDR = ((uint16_t)hpcd.USB_Address | USB_DADDR_EF);
      hpcd.USB_Address = 0U;
    }
  }
  else
  {
    /* DIR = 1 */

    /* DIR = 1 & CTR_RX => SETUP or OUT int */
    /* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */
    ep = &hpcd.OUT_ep[0];
    wEPVal = (uint16_t)PCD_GET_ENDPOINT(hpcd.Instance, PCD_ENDP0);

    if ((wEPVal & USB_EP_SETUP) != 0U)
    {
      /* Get SETUP Packet */
      ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd.Instance, ep->num);

      if (ep->xfer_count != 8U)
      {
        /* Set Stall condition for EP0 IN/OUT */
        PCD_SET_EP_RX_STATUS(hpcd.Instance, PCD_ENDP0, USB_EP_RX_STALL);
        PCD_SET_EP_TX_STATUS(hpcd.Instance, PCD_ENDP0, USB_EP_TX_STALL);

        /* SETUP bit kept frozen while CTR_RX = 1 */
        PCD_CLEAR_RX_EP_CTR(hpcd.Instance, PCD_ENDP0);

        return;
      }

      USB_ReadPMA(hpcd.Instance, (uint8_t *)hpcd.Setup,
                  ep->pmaadress, (uint16_t)ep->xfer_count);

      /* SETUP bit kept frozen while CTR_RX = 1 */
      PCD_CLEAR_RX_EP_CTR(hpcd.Instance, PCD_ENDP0);

      /* Process SETUP Packet*/
      setup();
    }
    else if ((wEPVal & USB_EP_VTRX) != 0U)
    {
      PCD_CLEAR_RX_EP_CTR(hpcd.Instance, PCD_ENDP0);

      /* Get Control Data OUT Packet */
      ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd.Instance, ep->num);

      if (ep->xfer_count == 0U)
      {
        /* Status phase re-arm for next setup */
        PCD_SET_EP_RX_STATUS(hpcd.Instance, PCD_ENDP0, USB_EP_RX_VALID);
      }
      else
      {
        if (ep->xfer_buff != 0U)
        {
          USB_ReadPMA(hpcd.Instance, ep->xfer_buff,
                      ep->pmaadress, (uint16_t)ep->xfer_count);  /* max 64bytes */

          ep->xfer_buff += ep->xfer_count;

          /* Process Control Data OUT Packet */
          on_data_out(0);
        }
      }
    }
  }

}

void STM32::DCD::endpoint_IRQ()
{
  PCD_EPTypeDef *ep;
  uint16_t count;
  uint16_t wIstr;
  uint16_t wEPVal;
  uint16_t TxPctSize;
  uint8_t epindex;

#if (USE_USB_DOUBLE_BUFFER != 1U)
  count = 0U;
#endif /* USE_USB_DOUBLE_BUFFER */

  /* stay in loop while pending interrupts */
  while ((hpcd.Instance->ISTR & USB_ISTR_CTR) != 0U)
  {
    wIstr = (uint16_t)hpcd.Instance->ISTR;

    /* extract highest priority endpoint number */
    epindex = (uint8_t)(wIstr & USB_ISTR_IDN);

    if (epindex == 0U)
    {
      control_IRQ();
      continue;
    }

    /* Decode and service non control endpoints interrupt */
    /* process related endpoint register */
    wEPVal = (uint16_t)PCD_GET_ENDPOINT(hpcd.Instance, epindex);

    if ((wEPVal & USB_EP_VTRX) != 0U)
    {
      /* clear int flag */
      PCD_CLEAR_RX_EP_CTR(hpcd.Instance, epindex);
      ep = &hpcd.OUT_ep[epindex];

      /* OUT Single Buffering */
      if (ep->doublebuffer == 0U)
      {
        count = (uint16_t)PCD_GET_EP_RX_CNT(hpcd.Instance, ep->num);

        if (count != 0U)
        {
          USB_ReadPMA(hpcd.Instance, ep->xfer_buff, ep->pmaadress, count);
        }
      }
#if (USE_USB_DOUBLE_BUFFER == 1U)
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
          PCD_FREE_USER_BUFFER(hpcd.Instance, ep->num, 0U);

          if ((PCD_GET_ENDPOINT(hpcd.Instance, ep->num) & USB_EP_DTOG_RX) != 0U)
          {
            /* read from endpoint BUF0Addr buffer */
            count = (uint16_t)PCD_GET_EP_DBUF0_CNT(hpcd.Instance, ep->num);

            if (count != 0U)
            {
              USB_ReadPMA(hpcd.Instance, ep->xfer_buff, ep->pmaaddr0, count);
            }
          }
          else
          {
            /* read from endpoint BUF1Addr buffer */
            count = (uint16_t)PCD_GET_EP_DBUF1_CNT(hpcd.Instance, ep->num);

            if (count != 0U)
            {
              USB_ReadPMA(hpcd.Instance, ep->xfer_buff, ep->pmaaddr1, count);
            }
          }
        }
      }
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */

      /* multi-packet on the NON control OUT endpoint */
      ep->xfer_count += count;

      if ((ep->xfer_len == 0U) || (count < ep->maxpacket))
      {
        /* RX COMPLETE */
        on_data_out(ep->num);
      }
      else
      {
         ep->xfer_buff += count;
        (void)USB_EPStartXfer(hpcd.Instance, ep);
      }
    }

    if ((wEPVal & USB_EP_VTTX) != 0U)
    {
      ep = &hpcd.IN_ep[epindex];

      /* clear int flag */
      PCD_CLEAR_TX_EP_CTR(hpcd.Instance, epindex);

      if (ep->type == EP_TYPE_ISOC)
      {
        ep->xfer_len = 0U;

#if (USE_USB_DOUBLE_BUFFER == 1U)
        if (ep->doublebuffer != 0U)
        {
          if ((wEPVal & USB_EP_DTOG_TX) != 0U)
          {
            PCD_SET_EP_DBUF0_CNT(hpcd.Instance, ep->num, ep->is_in, 0U);
          }
          else
          {
            PCD_SET_EP_DBUF1_CNT(hpcd.Instance, ep->num, ep->is_in, 0U);
          }
        }
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */

        /* TX COMPLETE */
        STM32::gDCD->on_data_in(ep->num);
      }
      else
      {
        /* Manage Single Buffer Transaction */
        if ((wEPVal & USB_EP_KIND) == 0U)
        {
          /* Multi-packet on the NON control IN endpoint */
          TxPctSize = (uint16_t)PCD_GET_EP_TX_CNT(hpcd.Instance, ep->num);

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
            on_data_in(ep->num);
          }
          else
          {
            /* Transfer is not yet Done */
            ep->xfer_buff += TxPctSize;
            ep->xfer_count += TxPctSize;
            (void)USB_EPStartXfer(hpcd.Instance, ep);
          }

        }
#if (USE_USB_DOUBLE_BUFFER == 1U)
        /* Double Buffer bulk IN (bulk transfer Len > Ep_Mps) */
        else
        {
          transmit(ep, wEPVal);
        }
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */
      }
    }
  }

  return;
}


void STM32::DCD::handle_IRQ()
{
  uint32_t wIstr = USB_ReadInterrupts(hpcd.Instance);

  if ((wIstr & USB_ISTR_CTR) == USB_ISTR_CTR)
  {
    /* servicing of the endpoint correct transfer interrupt */
    /* clear of the CTR flag into the sub */
    endpoint_IRQ();

    return;
  }

  if ((wIstr & USB_ISTR_RESET) == USB_ISTR_RESET)
  {
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_RESET);

    reset();

    (void)HAL_PCD_SetAddress(&hpcd, 0U);

    return;
  }

  if ((wIstr & USB_ISTR_PMAOVR) == USB_ISTR_PMAOVR)
  {
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_PMAOVR);

    return;
  }

  if ((wIstr & USB_ISTR_ERR) == USB_ISTR_ERR)
  {
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_ERR);

    return;
  }

  if ((wIstr & USB_ISTR_WKUP) == USB_ISTR_WKUP)
  {
    hpcd.Instance->CNTR &= ~(USB_CNTR_SUSPRDY);
    hpcd.Instance->CNTR &= ~(USB_CNTR_SUSPEN);

    if (hpcd.LPM_State == LPM_L1)
    {
      hpcd.LPM_State = LPM_L0;

      HAL_PCDEx_LPM_Callback(&hpcd, PCD_LPM_L0_ACTIVE);
    }

    STM32::gDCD->resume();

    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_WKUP);

    return;
  }

  if ((wIstr & USB_ISTR_SUSP) == USB_ISTR_SUSP)
  {
    /* Force low-power mode in the macrocell */
    hpcd.Instance->CNTR |= USB_CNTR_SUSPEN;

    /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_SUSP);

    hpcd.Instance->CNTR |= USB_CNTR_SUSPRDY;

    STM32::gDCD->suspend();

    return;
  }

  /* Handle LPM Interrupt */
  if ((wIstr & USB_ISTR_L1REQ) == USB_ISTR_L1REQ)
  {
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_L1REQ);
    if (hpcd.LPM_State == LPM_L0)
    {
      /* Force suspend and low-power mode before going to L1 state*/
      hpcd.Instance->CNTR |= USB_CNTR_SUSPRDY;
      hpcd.Instance->CNTR |= USB_CNTR_SUSPEN;

      hpcd.LPM_State = LPM_L1;
      hpcd.BESL = ((uint32_t)hpcd.Instance->LPMCSR & USB_LPMCSR_BESL) >> 2;
      HAL_PCDEx_LPM_Callback(&hpcd, PCD_LPM_L1_ACTIVE);
    }
    else
    {
      STM32::gDCD->suspend();
    }

    return;
  }

  if ((wIstr & USB_ISTR_SOF) == USB_ISTR_SOF)
  {
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_SOF);

    STM32::gDCD->on_sof();

    return;
  }

  if ((wIstr & USB_ISTR_ESOF) == USB_ISTR_ESOF)
  {
    /* clear ESOF flag in ISTR */
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_ESOF);

    return;
  }
}

uint32_t STM32::DCD::get_frame_number()
{

    /* This function never fails. */
    return 0;
}


extern "C" void USB_DRD_FS_IRQHandler(void)
{
  try {
    STM32::gDCD->handle_IRQ();

    return;
  } catch (const std::runtime_error &e) {
    const char *what = e.what();
    __asm volatile ("BKPT     %0" : : "i"(0));
  } catch (const std::exception &e) {
    const char *what = e.what();
    __asm volatile ("BKPT     %0" : : "i"(0));
  }
}
