#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct sai
  {
    uint32_t GCR;          /*!< SAI global configuration register,        Address offset: 0x00 */
    uint32_t RESERVED[16]; /*!< Reserved,                         Address offset: 0x04 to 0x40 */
    uint32_t PDMCR;        /*!< SAI PDM control register,                 Address offset: 0x44 */
    uint32_t PDMDLY;       /*!< SAI PDM delay register,                   Address offset: 0x48 */
  };
  
  struct sai_block
  {
    uint32_t CR1;         /*!< SAI block x configuration register 1,     Address offset: 0x04 */
    uint32_t CR2;         /*!< SAI block x configuration register 2,     Address offset: 0x08 */
    uint32_t FRCR;        /*!< SAI block x frame configuration register, Address offset: 0x0C */
    uint32_t SLOTR;       /*!< SAI block x slot register,                Address offset: 0x10 */
    uint32_t IMR;         /*!< SAI block x interrupt mask register,      Address offset: 0x14 */
    uint32_t SR;          /*!< SAI block x status register,              Address offset: 0x18 */
    uint32_t CLRFR;       /*!< SAI block x clear flag register,          Address offset: 0x1C */
    uint32_t DR;          /*!< SAI block x data register,                Address offset: 0x20 */
  };
};
