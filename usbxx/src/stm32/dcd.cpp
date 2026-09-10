
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

#include <format>
#include <stdexcept>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
//#include <usbxx/ux_device_stack.h>

#include <usbxx/device.hpp>
#include <usbxx/event_log.hpp>

#include <usbxx/stm32/pcd.hpp>


using namespace USBXX;

USBXX::STM32::DCD *USBXX::STM32::gDCD;

STM32::DCD::DCD(PCD_TypeDef *_pcd) : pcd(_pcd)
{
  STM32::gDCD = this;
  desired_speed = DeviceSpeed::FS;
  lpm_enable = false;
};

void STM32::DCD::low_level_init()
{

}

uint32_t STM32::DCD::initialize()
{
  pcd_handle = &hpcd;

#if 0
  hpcd.Init.dev_endpoints = 8;
  hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd.Init.Sof_enable = DISABLE;
  hpcd.Init.low_power_enable = DISABLE;
  hpcd.Init.lpm_enable = DISABLE;
  hpcd.Init.battery_charging_enable = DISABLE;
  hpcd.Init.vbus_sensing_enable = DISABLE;
  hpcd.Init.bulk_doublebuffer_enable = DISABLE;
  hpcd.Init.iso_singlebuffer_enable = DISABLE;
#endif

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
      throw USBXX::runtime_error("Unable to setup USB clock");
    }

    HAL_PWREx_EnableVddUSB();
    __HAL_RCC_USB_CLK_ENABLE();

    HAL_NVIC_SetPriority(USB_DRD_FS_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_DRD_FS_IRQn);
  }

  hpcd.State = HAL_PCD_STATE_BUSY;

  /* Disable the Interrupts */
  disable_interrupts();

  int i;

  pcd->CNTR = USB_CNTR_USBRST;

  /* Release Reset */
  pcd->CNTR &= ~USB_CNTR_USBRST;

  pcd->CNTR &= ~USB_CNTR_HOST;

  // Clear Interrupts
  pcd->ISTR = 0U;

  address_set = false;
  hpcd.State = HAL_PCD_STATE_READY;

  /* Activate LPM */
  if (lpm_enable)
  {
    activate_lpm(true);
  }

  disable_pullup();

  control_endpoint = std::make_shared<STM32::ControlEndpoint>(device, this);

  control_endpoint->init();

  endpoints[0x00] = control_endpoint;
  endpoints[0x80] = control_endpoint;

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
  STM32::Endpoint::ptr retval;

  if (epaddr & 0x80)
    retval = std::make_shared<STM32::InEndpoint>(device, this, epaddr);
  else
    retval = std::make_shared<STM32::OutEndpoint>(device, this, epaddr);

  retval->set_interface(iface);
  retval->set_descriptor(desc);

  retval->init();

  endpoints[epaddr] = retval;

  return retval;
}

UINT  STM32::DCD::complete_initialization()
{
  device->descriptor = read_in_descriptor<DeviceDescriptor>(device->get_current_descriptor().get_desc());

  control_endpoint->descriptor.wMaxPacketSize =
      device->descriptor.bMaxPacketSize0;

  auto transfer = control_endpoint->get_transfer();

  transfer->requested_length = device->descriptor.bMaxPacketSize0;

  control_endpoint->create();

  control_endpoint->open();



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

void STM32::DCD::activate_lpm(bool v)
{
  if (v) {
    hpcd.lpm_active = 1U;
    hpcd.LPM_State = LPM_L0;

    pcd->LPMCSR |= USB_LPMCSR_LMPEN;
    pcd->LPMCSR |= USB_LPMCSR_LPMACK;
  } else {
    hpcd.lpm_active = 0U;

    pcd->LPMCSR &= ~(USB_LPMCSR_LMPEN);
    pcd->LPMCSR &= ~(USB_LPMCSR_LPMACK);

  }
}

