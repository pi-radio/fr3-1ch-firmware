#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct cordic
  {
    uint32_t CSR;           /*!< CORDIC control and status register,        Address offset: 0x00 */
    uint32_t WDATA;         /*!< CORDIC argument register,                  Address offset: 0x04 */
    uint32_t RDATA;         /*!< CORDIC result register,                    Address offset: 0x08 */
  };
};
