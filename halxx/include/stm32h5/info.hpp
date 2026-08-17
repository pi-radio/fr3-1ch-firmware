#pragma once

#include <cstdint>
#include <string>

struct device_info
{
  uint32_t uid0;
  uint32_t uid1;
  uint32_t uid2;
  uint32_t uid3;

  uint32_t cpuid;
  uint32_t revid;

  device_info();

  uint32_t get_x();
  uint32_t get_y();

  std::string get_lot();
  uint32_t get_wafer();

  uint32_t get_flash_size();
  uint32_t get_package();

};

extern device_info devinfo;
