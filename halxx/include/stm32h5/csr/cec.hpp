#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct cec
  {
    uint32_t CR;           /*!< CEC control register,                                              Address offset:0x00 */
    uint32_t CFGR;         /*!< CEC configuration register,                                        Address offset:0x04 */
    uint32_t TXDR;         /*!< CEC Tx data register ,                                             Address offset:0x08 */
    uint32_t RXDR;         /*!< CEC Rx Data Register,                                              Address offset:0x0C */
    uint32_t ISR;          /*!< CEC Interrupt and Status Register,                                 Address offset:0x10 */
    uint32_t IER;          /*!< CEC interrupt enable register,                                     Address offset:0x14 */
  };
};
