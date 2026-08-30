
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

#include <stdexcept>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/ux_device_stack.h>

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

  // MOVE ME
  HAL_PCDEx_PMAConfig(&hpcd, 0x00 , PCD_SNG_BUF, 0x40);
  HAL_PCDEx_PMAConfig(&hpcd, 0x80 , PCD_SNG_BUF, 0x80);
  HAL_PCDEx_PMAConfig(&hpcd, 0x01, PCD_SNG_BUF, 0xC0);
  HAL_PCDEx_PMAConfig(&hpcd, 0x81, PCD_SNG_BUF, 0x100);
  HAL_PCDEx_PMAConfig(&hpcd, 0x82, PCD_SNG_BUF, 0x140);

  ux_slave_dcd_status =  UX_DCD_STATUS_OPERATIONAL;

  HAL_PCD_Start(&hpcd);

  /* Return successful completion.  */
  return(UX_SUCCESS);
}




void STM32::DCD::set_device_address(uint8_t addr)
{
  HAL_PCD_SetAddress(pcd_handle, addr);
}

void STM32::DCD::handle_IRQ()
{
  HAL_PCD_IRQHandler(&hpcd);
}

uint32_t STM32::DCD::get_frame_number()
{

    /* This function never fails. */
    return(UX_SUCCESS);
}


extern "C" void USB_DRD_FS_IRQHandler(void)
{
  STM32::gDCD->handle_IRQ();
}
