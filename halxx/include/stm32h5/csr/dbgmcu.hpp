#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct dbgmcu
  {
    uint32_t IDCODE;       /*!< MCU device ID code,                 Address offset: 0x00  */
    uint32_t CR;           /*!< Debug MCU configuration register,   Address offset: 0x04  */
    uint32_t APB1FZR1;     /*!< Debug MCU APB1 freeze register 1,   Address offset: 0x08  */
    uint32_t APB1FZR2;     /*!< Debug MCU APB1 freeze register 2,   Address offset: 0x0C  */
    uint32_t APB2FZR;      /*!< Debug MCU APB2 freeze register,     Address offset: 0x10  */
    uint32_t APB3FZR;      /*!< Debug MCU APB3 freeze register,     Address offset: 0x14  */
    uint32_t RESERVED1[2]; /*!< Reserved,                                    0x18 - 0x1C  */
    uint32_t AHB1FZR;      /*!< Debug MCU AHB1 freeze register,     Address offset: 0x20  */
    uint32_t RESERVED2[54]; /*!< Reserved,                                   0x24 - 0xF8  */
    uint32_t SR;           /*!< Debug MCU SR register,              Address offset: 0xFC  */
    uint32_t DBG_AUTH_HOST; /*!< Debug DBG_AUTH_HOST register,      Address offset: 0x100 */
    uint32_t DBG_AUTH_DEV;  /*!< Debug DBG_AUTH_DEV register,       Address offset: 0x104 */
    uint32_t DBG_AUTH_ACK;  /*!< Debug DBG_AUTH_ACK register,       Address offset: 0x108 */
    uint32_t RESERVED3[945]; /*!< Reserved,                                 0x10C - 0xFCC */
    uint32_t PIDR4;       /*!< Debug MCU Peripheral ID register 4,  Address offset: 0xFD0 */
    uint32_t PIDR5;       /*!< Debug MCU Peripheral ID register 5,  Address offset: 0xFD4 */
    uint32_t PIDR6;       /*!< Debug MCU Peripheral ID register 6,  Address offset: 0xFD8 */
    uint32_t PIDR7;       /*!< Debug MCU Peripheral ID register 7,  Address offset: 0xFDC */
    uint32_t PIDR0;       /*!< Debug MCU Peripheral ID register 0,  Address offset: 0xFE0 */
    uint32_t PIDR1;       /*!< Debug MCU Peripheral ID register 1,  Address offset: 0xFE4 */
    uint32_t PIDR2;       /*!< Debug MCU Peripheral ID register 2,  Address offset: 0xFE8 */
    uint32_t PIDR3;       /*!< Debug MCU Peripheral ID register 3,  Address offset: 0xFEC */
    uint32_t CIDR0;       /*!< Debug MCU Component ID register 0,   Address offset: 0xFF0 */
    uint32_t CIDR1;       /*!< Debug MCU Component ID register 1,   Address offset: 0xFF4 */
    uint32_t CIDR2;       /*!< Debug MCU Component ID register 2,   Address offset: 0xFF8 */
    uint32_t CIDR3;       /*!< Debug MCU Component ID register 3,   Address offset: 0xFFC */
  };
};
