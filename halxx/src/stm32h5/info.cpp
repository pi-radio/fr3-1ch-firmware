#include <stm32h5/info.hpp>

extern "C" {
#include <stm32h573xx.h>
#include <stm32h5xx_hal.h>
#include <stm32h5xx_ll_system.h>
}

device_info::device_info()
{
  uint32_t *p_uid = (uint32_t *)UID_BASE;

  uid0 = *p_uid++;
  uid1 = *p_uid++;
  uid2 = *p_uid++;
  uid3 = *p_uid++;

  cpuid = LL_DBGMCU_GetDeviceID();
  revid = LL_DBGMCU_GetRevisionID();
}

template <int n>
uint8_t get_byte(uint32_t x)
{
  return (uint8_t)((x >> (n * 8)) & 0xFF);
}

uint32_t device_info::get_x()
{
  return uid0 & 0xFFFF;
}

uint32_t device_info::get_y()
{
  return uid0 >> 16;
}

std::string device_info::get_lot()
{
  return std::string({get_byte<3>(uid2), get_byte<2>(uid2), get_byte<1>(uid2), get_byte<0>(uid2), get_byte<3>(uid1), get_byte<2>(uid1)});

}

uint32_t device_info::get_wafer()
{
  return uid1 & 0xFFFF;
}

uint32_t device_info::get_flash_size()
{
  return uid3 & 0xFFFF;
}

uint32_t device_info::get_package()
{
  return uid3 >> 16;
}

device_info devinfo;
