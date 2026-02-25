#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct aes
  {
    uint32_t CR;          /*!< AES control register,                        Address offset: 0x00 */
    uint32_t SR;          /*!< AES status register,                         Address offset: 0x04 */
    uint32_t DINR;        /*!< AES data input register,                     Address offset: 0x08 */
    uint32_t DOUTR;       /*!< AES data output register,                    Address offset: 0x0C */
    uint32_t KEYR0;       /*!< AES key register 0,                          Address offset: 0x10 */
    uint32_t KEYR1;       /*!< AES key register 1,                          Address offset: 0x14 */
    uint32_t KEYR2;       /*!< AES key register 2,                          Address offset: 0x18 */
    uint32_t KEYR3;       /*!< AES key register 3,                          Address offset: 0x1C */
    uint32_t IVR0;        /*!< AES initialization vector register 0,        Address offset: 0x20 */
    uint32_t IVR1;        /*!< AES initialization vector register 1,        Address offset: 0x24 */
    uint32_t IVR2;        /*!< AES initialization vector register 2,        Address offset: 0x28 */
    uint32_t IVR3;        /*!< AES initialization vector register 3,        Address offset: 0x2C */
    uint32_t KEYR4;       /*!< AES key register 4,                          Address offset: 0x30 */
    uint32_t KEYR5;       /*!< AES key register 5,                          Address offset: 0x34 */
    uint32_t KEYR6;       /*!< AES key register 6,                          Address offset: 0x38 */
    uint32_t KEYR7;       /*!< AES key register 7,                          Address offset: 0x3C */
    uint32_t SUSP0R;      /*!< AES Suspend register 0,                      Address offset: 0x40 */
    uint32_t SUSP1R;      /*!< AES Suspend register 1,                      Address offset: 0x44 */
    uint32_t SUSP2R;      /*!< AES Suspend register 2,                      Address offset: 0x48 */
    uint32_t SUSP3R;      /*!< AES Suspend register 3,                      Address offset: 0x4C */
    uint32_t SUSP4R;      /*!< AES Suspend register 4,                      Address offset: 0x50 */
    uint32_t SUSP5R;      /*!< AES Suspend register 5,                      Address offset: 0x54 */
    uint32_t SUSP6R;      /*!< AES Suspend register 6,                      Address offset: 0x58 */
    uint32_t SUSP7R;      /*!< AES Suspend register 7,                      Address offset: 0x5C */
    uint32_t RESERVED1[168];/*!< Reserved,                                  Address offset: 0x60 -- 0x2FC */
    uint32_t IER;          /*!< AES Interrupt Enable Register,              Address offset: 0x300 */
    uint32_t ISR;          /*!< AES Interrupt Status Register,              Address offset: 0x304 */
    uint32_t ICR;          /*!< AES Interrupt Clear Register,               Address offset: 0x308 */
  };
};
