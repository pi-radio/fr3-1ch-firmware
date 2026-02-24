#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct dts
  {
    uint32_t CFGR1;         /*!< DTS configuration register,                Address offset: 0x00 */
    uint32_t RESERVED0;          /*!< Reserved,                                  Address offset: 0x04 */
    uint32_t T0VALR1;       /*!< DTS T0 Value register,                     Address offset: 0x08 */
    uint32_t RESERVED1;          /*!< Reserved,                                  Address offset: 0x0C */
    uint32_t RAMPVALR;      /*!< DTS Ramp value register,                   Address offset: 0x10 */
    uint32_t ITR1;          /*!< DTS Interrupt threshold register,          Address offset: 0x14 */
    uint32_t RESERVED2;          /*!< Reserved,                                  Address offset: 0x18 */
    uint32_t DR;            /*!< DTS data register,                         Address offset: 0x1C */
    uint32_t SR;            /*!< DTS status register                        Address offset: 0x20 */
    uint32_t ITENR;         /*!< DTS Interrupt enable register,             Address offset: 0x24 */
    uint32_t ICIFR;         /*!< DTS Clear Interrupt flag register,         Address offset: 0x28 */
    uint32_t OR;            /*!< DTS option register 1,                     Address offset: 0x2C */
  };
};
