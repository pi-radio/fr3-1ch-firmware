#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct vref
  {
    uint32_t CSR;         /*!< VREFBUF control and status register,         Address offset: 0x00 */
    uint32_t CCR;         /*!< VREFBUF calibration and control register,    Address offset: 0x04 */
  };
};
