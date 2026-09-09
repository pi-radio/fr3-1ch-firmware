
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

  assert(pcd==USB_DRD_FS);

  if (hpcd.State == HAL_PCD_STATE_RESET)
  {
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /* Allocate lock resource and initialize it */
    hpcd.Lock = HAL_UNLOCKED;

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      throw std::runtime_error("Unable to setup USB clock");
    }

    HAL_PWREx_EnableVddUSB();
    __HAL_RCC_USB_CLK_ENABLE();

    HAL_NVIC_SetPriority(USB_DRD_FS_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_DRD_FS_IRQn);
  }

  hpcd.State = HAL_PCD_STATE_BUSY;

  /* Disable the Interrupts */
  disable_interrupts();

  /*Init the Core (common init.) */
  if (USB_CoreInit(pcd, hpcd.Init) != HAL_OK)
  {
    hpcd.State = HAL_PCD_STATE_ERROR;
    return HAL_ERROR;
  }

  /* Force Device Mode */
  if (USB_SetCurrentMode(pcd, USB_DEVICE_MODE) != HAL_OK)
  {
    hpcd.State = HAL_PCD_STATE_ERROR;
    return HAL_ERROR;
  }

  int i;

  /* Init endpoints structures */
  for (i = 0U; i < hpcd.Init.dev_endpoints; i++)
  {
    /* Init ep structure */
    hpcd.IN_ep[i].is_in = 1U;
    hpcd.IN_ep[i].num = i;
    /* Control until ep is activated */
    hpcd.IN_ep[i].type = EP_TYPE_CTRL;
    hpcd.IN_ep[i].maxpacket = 0U;
    hpcd.IN_ep[i].xfer_buff = 0U;
    hpcd.IN_ep[i].xfer_len = 0U;
  }

  for (i = 0U; i < hpcd.Init.dev_endpoints; i++)
  {
    hpcd.OUT_ep[i].is_in = 0U;
    hpcd.OUT_ep[i].num = i;
    /* Control until ep is activated */
    hpcd.OUT_ep[i].type = EP_TYPE_CTRL;
    hpcd.OUT_ep[i].maxpacket = 0U;
    hpcd.OUT_ep[i].xfer_buff = 0U;
    hpcd.OUT_ep[i].xfer_len = 0U;
  }

  /* Init Device */
  if (USB_DevInit(pcd, hpcd.Init) != HAL_OK)
  {
    hpcd.State = HAL_PCD_STATE_ERROR;
    throw std::runtime_error("Failed in USB_DevInit");
  }

  address_set = false;
  hpcd.State = HAL_PCD_STATE_READY;

  /* Activate LPM */
  if (hpcd.Init.lpm_enable == 1U)
  {
    (void)HAL_PCDEx_ActivateLPM(&hpcd);
  }

  disable_pullup();

  control_endpoint = std::make_shared<STM32::ControlEndpoint>(device, this);

  endpoints[0x00] = control_endpoint;
  endpoints[0x80] = control_endpoint;

  control_endpoint->transfer.endpoint = control_endpoint;
  control_endpoint->transfer.timeout =  UX_MS_TO_TICK(UX_CONTROL_TRANSFER_TIMEOUT);

  /* Adjust the current data pointer as well.  */
  control_endpoint->transfer.current_data_pointer = control_endpoint->transfer.data;



  HAL_PCDEx_PMAConfig(&hpcd, 0x81, PCD_SNG_BUF, 0x100);
  HAL_PCDEx_PMAConfig(&hpcd, 0x82, PCD_SNG_BUF, 0x140);
  HAL_PCDEx_PMAConfig(&hpcd, 0x03, PCD_SNG_BUF, 0xC0);

  status =  UX_DCD_STATUS_OPERATIONAL;

  hpcd.Lock = HAL_LOCKED;

  enable_interrupts();
  enable_pullup();

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

  control_endpoint->open();

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
  assert(hpcd.Lock == HAL_UNLOCKED);
  hpcd.Lock = HAL_LOCKED;

  if (addr == 0) {
    device_address = 0;
    pcd->DADDR = USB_DADDR_EF;
    address_set = false;
  } else {
    assert(device_address == 0);
    device_address = addr;
  }

  hpcd.Lock = HAL_UNLOCKED;
}

void STM32::DCD::enable_pullup()
{
  pcd->BCDR |= USB_BCDR_DPPU;
}

void STM32::DCD::disable_pullup()
{
  pcd->BCDR &= ~(USB_BCDR_DPPU);
}


void STM32::DCD::stop()
{
  assert(hpcd.Lock == HAL_UNLOCKED);
  hpcd.Lock = HAL_LOCKED;
  disable_interrupts();
  disable_pullup();

  hpcd.Lock = HAL_UNLOCKED;
}

