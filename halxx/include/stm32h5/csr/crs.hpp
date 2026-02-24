#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct crs
  {
    uint32_t CR;            /*!< CRS ccontrol register,              Address offset: 0x00 */
    uint32_t CFGR;          /*!< CRS configuration register,         Address offset: 0x04 */
    uint32_t ISR;           /*!< CRS interrupt and status register,  Address offset: 0x08 */
    uint32_t ICR;           /*!< CRS interrupt flag clear register,  Address offset: 0x0C */
  };
};
