
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

#include <format>
#include <stdexcept>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/ux_device_stack.h>

#include <usbxx/device.hpp>
#include <usbxx/event_log.hpp>

#include <usbxx/stm32/pcd.hpp>


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

  for (int i = 0; i < 8; i++) {
    (USB_DRD_PMA_BUFF + i)->TXBD = 0;
    (USB_DRD_PMA_BUFF + i)->RXBD = 0;
  }

  if (HAL_PCD_Init(&hpcd) != HAL_OK)
  {
    throw std::runtime_error("Unable to initialize USB stack");
  }

  control_endpoint = std::make_shared<STM32::ControlEndpoint>(device, this, 0);

  endpoints[0x00] = control_endpoint;
  endpoints[0x80] = control_endpoint;

  control_endpoint->transfer.endpoint = control_endpoint;
  control_endpoint->transfer.timeout =  UX_MS_TO_TICK(UX_CONTROL_TRANSFER_TIMEOUT);

  /* Adjust the current data pointer as well.  */
  control_endpoint->transfer.current_data_pointer = control_endpoint->transfer.data;


  HAL_PCDEx_PMAConfig(&hpcd, 0x00, PCD_SNG_BUF, 0x40);
  HAL_PCDEx_PMAConfig(&hpcd, 0x80, PCD_SNG_BUF, 0x80);
  HAL_PCDEx_PMAConfig(&hpcd, 0x81, PCD_SNG_BUF, 0x100);
  HAL_PCDEx_PMAConfig(&hpcd, 0x82, PCD_SNG_BUF, 0x140);
  HAL_PCDEx_PMAConfig(&hpcd, 0x03, PCD_SNG_BUF, 0xC0);

  status =  UX_DCD_STATUS_OPERATIONAL;

  hpcd.Lock = HAL_LOCKED;

  USB_EnableGlobalInt(hpcd.Instance);
  USB_DevConnect(hpcd.Instance);

  hpcd.Lock = HAL_UNLOCKED;

  /* Return successful completion.  */
  return 0;
}

USBXX::Endpoint::ptr STM32::DCD::allocate_endpoint(Interface::ptr iface, const EndpointDescriptor &desc)
{

  uint8_t epaddr = desc.bEndpointAddress;

  STM32::Endpoint::ptr retval = std::make_shared<STM32::Endpoint>(device, this, epaddr);

  retval->transfer.endpoint = retval;
  retval->descriptor = desc;
  retval->device = device;
  retval->interface = iface;
  retval->direction = (epaddr & 0x80) ? true : false;

  endpoints[epaddr] = retval;

  return retval;
}

UINT  STM32::DCD::complete_initialization()
{
  device->descriptor = read_in_descriptor<DeviceDescriptor>(device->get_current_descriptor().get_desc());

  control_endpoint->descriptor.wMaxPacketSize =
      device->descriptor.bMaxPacketSize0;

  control_endpoint->transfer.requested_length = device->descriptor.bMaxPacketSize0;

  control_endpoint->create();

  /* Open Control OUT endpoint.  */
  HAL_PCD_EP_Flush(pcd_handle, 0x00U);
  HAL_PCD_EP_Open(pcd_handle, 0x00U, device -> descriptor.bMaxPacketSize0, UX_CONTROL_ENDPOINT);

  /* Open Control IN endpoint.  */
  HAL_PCD_EP_Flush(pcd_handle, 0x80U);
  HAL_PCD_EP_Open(pcd_handle, 0x80U, device -> descriptor.bMaxPacketSize0, UX_CONTROL_ENDPOINT);

  /* Ensure the control endpoint is properly reset.  */
  control_endpoint->state = EndpointState::RESET;

  /* Mark the phase as SETUP.  */
  control_endpoint->transfer.type =  TransferType::SETUP;

  /* Mark this transfer request as pending.  */
  control_endpoint->transfer.set_pending();

  /* Ask for 8 bytes of the SETUP packet.  */
  control_endpoint->transfer.requested_length =    UX_SETUP_SIZE;
  control_endpoint->transfer. in_transfer_length =  UX_SETUP_SIZE;

  /* Reset the number of bytes sent/received.  */
  control_endpoint->transfer.actual_length =  0;

  /* Check the status change callback.  */
  device->on_attached();

  return 0;
}

UINT  STM32::DCD::uninitialize()
{
    /* Set the state of the controller to HALTED now.  */
  status =  UX_DCD_STATUS_HALTED;

  return 0;
}



void STM32::DCD::set_device_address(uint8_t addr)
{
  device_address = addr;

  HAL_PCD_SetAddress(pcd_handle, addr);
}

void STM32::DCD::endpoint_IRQ()
{
  uint16_t wIstr;
  uint16_t wEPVal;
  uint8_t epindex;

  /* stay in loop while pending interrupts */
  while ((hpcd.Instance->ISTR & USB_ISTR_CTR) != 0U)
  {
    wIstr = (uint16_t)hpcd.Instance->ISTR;

    epindex = (uint8_t)(wIstr & USB_ISTR_IDN);

    wEPVal = (uint16_t)PCD_GET_ENDPOINT(hpcd.Instance, epindex);

    event_log.push_event(UsbEvent::ENDPOINT_IRQ, epindex);



    if (wEPVal & USB_EP_VTRX)
    {
      auto ep = endpoints[epindex];

      ep->on_interrupt();
    }

    if (wEPVal & USB_EP_VTTX)
    {
      auto ep = endpoints[0x80 | epindex];

      ep->on_interrupt();
    }
  }
    /* Decode and service non control endpoints interrupt */
    /* process related endpoint register */


  return;
}


void STM32::DCD::handle_IRQ()
{
  uint32_t wIstr = USB_ReadInterrupts(pcd);

  if ((wIstr & USB_ISTR_CTR) == USB_ISTR_CTR)
  {
    /* servicing of the endpoint correct transfer interrupt */
    /* clear of the CTR flag into the sub */
    endpoint_IRQ();

    //event_log.push_event(UsbEvent::END_IRQ);

    return;
  }

  if ((wIstr & USB_ISTR_RESET) == USB_ISTR_RESET)
  {
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_RESET);

    reset();

    (void)HAL_PCD_SetAddress(&hpcd, 0U);

    //event_log.push_event(UsbEvent::END_IRQ);

    return;
  }

  if ((wIstr & USB_ISTR_PMAOVR) == USB_ISTR_PMAOVR)
  {
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_PMAOVR);

    //event_log.push_event(UsbEvent::END_IRQ);

    return;
  }

  if ((wIstr & USB_ISTR_ERR) == USB_ISTR_ERR)
  {
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_ERR);

    //event_log.push_event(UsbEvent::END_IRQ);

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

    //event_log.push_event(UsbEvent::END_IRQ);

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

    //event_log.push_event(UsbEvent::END_IRQ);

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

    //event_log.push_event(UsbEvent::END_IRQ);

    return;
  }

  if ((wIstr & USB_ISTR_SOF) == USB_ISTR_SOF)
  {
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_SOF);

    STM32::gDCD->on_sof();

    //event_log.push_event(UsbEvent::END_IRQ);

    return;
  }

  if ((wIstr & USB_ISTR_ESOF) == USB_ISTR_ESOF)
  {
    /* clear ESOF flag in ISTR */
    __HAL_PCD_CLEAR_FLAG(&hpcd, USB_ISTR_ESOF);

    //event_log.push_event(UsbEvent::END_IRQ);

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
