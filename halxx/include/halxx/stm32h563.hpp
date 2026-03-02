#pragma once

#include <halxx/stm32.hpp>
#include <cortex/mpu.hpp>

namespace halxx
{
  class STM32H563 {
    uniqid id;
    uint16_t flash_code;
    uint16_t package_code;
    
  public:
    halxx::cortex::mpu::MPU mpu;

    const uniqid &get_unique_id() const { return id; }
    uint16_t get_flash_code() const { return flash_code; }
    uint16_t get_package_code() const { return package_code; }
    uint32_t get_flash_size() const { return flash_code * 1024; }
    
    
    STM32H563();
  };
}

