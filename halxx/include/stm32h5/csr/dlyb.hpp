#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct dlyblk
  {
    uint32_t CR;          /*!< DELAY BLOCK control register,  Address offset: 0x00 */
    uint32_t CFGR;        /*!< DELAY BLOCK configuration register,  Address offset: 0x04 */
  };
};
