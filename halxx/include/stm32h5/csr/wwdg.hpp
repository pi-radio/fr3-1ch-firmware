#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct wwdg
  {
    uint32_t CR;          /*!< WWDG Control register,       Address offset: 0x00 */
    uint32_t CFR;         /*!< WWDG Configuration register, Address offset: 0x04 */
    uint32_t SR;          /*!< WWDG Status register,        Address offset: 0x08 */
  };
};
