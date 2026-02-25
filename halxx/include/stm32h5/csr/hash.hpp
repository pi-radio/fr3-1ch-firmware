#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct hash
  {
    uint32_t CR;               /*!< HASH control register,          Address offset: 0x00        */
    uint32_t DIN;              /*!< HASH data input register,       Address offset: 0x04        */
    uint32_t STR;              /*!< HASH start register,            Address offset: 0x08        */
    uint32_t HR[5];            /*!< HASH digest registers,          Address offset: 0x0C-0x1C   */
    uint32_t IMR;              /*!< HASH interrupt enable register, Address offset: 0x20        */
    uint32_t SR;               /*!< HASH status register,           Address offset: 0x24        */
    uint32_t RESERVED[52];     /*!< Reserved, 0x28-0xF4                                         */
    uint32_t CSR[103];         /*!< HASH context swap registers,    Address offset: 0x0F8-0x290 */
  };

  struct hash_digest
  {
    uint32_t HR[16];     /*!< HASH digest registers,          Address offset: 0x310-0x34C */
  };
};
