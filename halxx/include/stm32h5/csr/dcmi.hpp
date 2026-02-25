#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct dcmi
  {
    uint32_t CR;       /*!< DCMI control register 1,                       Address offset: 0x00 */
    uint32_t SR;       /*!< DCMI status register,                          Address offset: 0x04 */
    uint32_t RISR;     /*!< DCMI raw interrupt status register,            Address offset: 0x08 */
    uint32_t IER;      /*!< DCMI interrupt enable register,                Address offset: 0x0C */
    uint32_t MISR;     /*!< DCMI masked interrupt status register,         Address offset: 0x10 */
    uint32_t ICR;      /*!< DCMI interrupt clear register,                 Address offset: 0x14 */
    uint32_t ESCR;     /*!< DCMI embedded synchronization code register,   Address offset: 0x18 */
    uint32_t ESUR;     /*!< DCMI embedded synchronization unmask register, Address offset: 0x1C */
    uint32_t CWSTRTR;  /*!< DCMI crop window start,                        Address offset: 0x20 */
    uint32_t CWSIZER;  /*!< DCMI crop window size,                         Address offset: 0x24 */
    uint32_t DR;       /*!< DCMI data register,                            Address offset: 0x28 */
  };
};
