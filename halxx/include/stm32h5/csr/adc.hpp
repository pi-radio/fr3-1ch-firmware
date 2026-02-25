#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct adc
  {
    uint32_t ISR;          /*!< ADC interrupt and status register,             Address offset: 0x00 */
    uint32_t IER;          /*!< ADC interrupt enable register,                 Address offset: 0x04 */
    uint32_t CR;           /*!< ADC control register,                          Address offset: 0x08 */
    uint32_t CFGR;         /*!< ADC configuration register 1,                  Address offset: 0x0C */
    uint32_t CFGR2;        /*!< ADC configuration register 2,                  Address offset: 0x10 */
    uint32_t SMPR1;        /*!< ADC sampling time register 1,                  Address offset: 0x14 */
    uint32_t SMPR2;        /*!< ADC sampling time register 2,                  Address offset: 0x18 */
    uint32_t RESERVED1;    /*!< Reserved,                                                      0x1C */
    uint32_t TR1;          /*!< ADC analog watchdog 1 threshold register,      Address offset: 0x20 */
    uint32_t TR2;          /*!< ADC analog watchdog 2 threshold register,      Address offset: 0x24 */
    uint32_t TR3;          /*!< ADC analog watchdog 3 threshold register,      Address offset: 0x28 */
    uint32_t RESERVED2;    /*!< Reserved,                                                      0x2C */
    uint32_t SQR1;         /*!< ADC group regular sequencer register 1,        Address offset: 0x30 */
    uint32_t SQR2;         /*!< ADC group regular sequencer register 2,        Address offset: 0x34 */
    uint32_t SQR3;         /*!< ADC group regular sequencer register 3,        Address offset: 0x38 */
    uint32_t SQR4;         /*!< ADC group regular sequencer register 4,        Address offset: 0x3C */
    uint32_t DR;           /*!< ADC group regular data register,               Address offset: 0x40 */
    uint32_t RESERVED3;    /*!< Reserved,                                                      0x44 */
    uint32_t RESERVED4;    /*!< Reserved,                                                      0x48 */
    uint32_t JSQR;         /*!< ADC group injected sequencer register,         Address offset: 0x4C */
    uint32_t RESERVED5[4]; /*!< Reserved,                                               0x50 - 0x5C */
    uint32_t OFR1;         /*!< ADC offset register 1,                         Address offset: 0x60 */
    uint32_t OFR2;         /*!< ADC offset register 2,                         Address offset: 0x64 */
    uint32_t OFR3;         /*!< ADC offset register 3,                         Address offset: 0x68 */
    uint32_t OFR4;         /*!< ADC offset register 4,                         Address offset: 0x6C */
    uint32_t RESERVED6[4]; /*!< Reserved,                                               0x70 - 0x7C */
    uint32_t JDR1;         /*!< ADC group injected rank 1 data register,       Address offset: 0x80 */
    uint32_t JDR2;         /*!< ADC group injected rank 2 data register,       Address offset: 0x84 */
    uint32_t JDR3;         /*!< ADC group injected rank 3 data register,       Address offset: 0x88 */
    uint32_t JDR4;         /*!< ADC group injected rank 4 data register,       Address offset: 0x8C */
    uint32_t RESERVED7[4]; /*!< Reserved,                                             0x090 - 0x09C */
    uint32_t AWD2CR;       /*!< ADC analog watchdog 2 configuration register,  Address offset: 0xA0 */
    uint32_t AWD3CR;       /*!< ADC analog watchdog 3 Configuration Register,  Address offset: 0xA4 */
    uint32_t RESERVED8;    /*!< Reserved,                                                     0x0A8 */
    uint32_t RESERVED9;    /*!< Reserved,                                                     0x0AC */
    uint32_t DIFSEL;       /*!< ADC differential mode selection register,      Address offset: 0xB0 */
    uint32_t CALFACT;      /*!< ADC calibration factors,                       Address offset: 0xB4 */
    uint32_t RESERVED10[4];/*!< Reserved,                                             0x0B8 - 0x0C4 */
    uint32_t OR;           /*!< ADC option register,                           Address offset: 0xC8 */
  };

  struct adc_common
  {
    uint32_t CSR;          /*!< ADC common status register,            Address offset: 0x300 + 0x00 */
    uint32_t      RESERVED1;    /*!< Reserved,                              Address offset: 0x300 + 0x04 */
    uint32_t CCR;          /*!< ADC common configuration register,     Address offset: 0x300 + 0x08 */
    uint32_t CDR;          /*!< ADC common group regular data register Address offset: 0x300 + 0x0C */
  };
};
