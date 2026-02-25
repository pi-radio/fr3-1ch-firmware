#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct gpio
  {
    uint32_t MODER;       /*!< GPIO port mode register,               Address offset: 0x00      */
    uint32_t OTYPER;      /*!< GPIO port output type register,        Address offset: 0x04      */
    uint32_t OSPEEDR;     /*!< GPIO port output speed register,       Address offset: 0x08      */
    uint32_t PUPDR;       /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    uint32_t IDR;         /*!< GPIO port input data register,         Address offset: 0x10      */
    uint32_t ODR;         /*!< GPIO port output data register,        Address offset: 0x14      */
    uint32_t BSRR;        /*!< GPIO port bit set/reset  register,     Address offset: 0x18      */
    uint32_t LCKR;        /*!< GPIO port configuration lock register, Address offset: 0x1C      */
    uint32_t AFR[2];      /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
    uint32_t BRR;         /*!< GPIO Bit Reset register,               Address offset: 0x28      */
    uint32_t HSLVR;       /*!< GPIO high-speed low voltage register,  Address offset: 0x2C      */
    uint32_t SECCFGR;     /*!< GPIO secure configuration register,    Address offset: 0x30      */
  };
};
