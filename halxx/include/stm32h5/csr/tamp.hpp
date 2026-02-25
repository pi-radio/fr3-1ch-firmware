#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct tamp
  {
    uint32_t CR1;         /*!< TAMP control register 1,                  Address offset: 0x00 */
    uint32_t CR2;         /*!< TAMP control register 2,                  Address offset: 0x04 */
    uint32_t CR3;         /*!< TAMP control register 3,                  Address offset: 0x08 */
    uint32_t FLTCR;       /*!< TAMP filter control register,             Address offset: 0x0C */
    uint32_t ATCR1;       /*!< TAMP filter control register 1            Address offset: 0x10 */
    uint32_t ATSEEDR;     /*!< TAMP active tamper seed register,         Address offset: 0x14 */
    uint32_t ATOR;        /*!< TAMP active tamper output register,       Address offset: 0x18 */
    uint32_t ATCR2;       /*!< TAMP filter control register 2,           Address offset: 0x1C */
    uint32_t SECCFGR;     /*!< TAMP secure mode control register,        Address offset: 0x20 */
    uint32_t PRIVCFGR;    /*!< TAMP privilege mode control register,     Address offset: 0x24 */
    uint32_t RESERVED0;   /*!< Reserved,                                 Address offset: 0x28 */
    uint32_t IER;         /*!< TAMP interrupt enable register,           Address offset: 0x2C */
    uint32_t SR;          /*!< TAMP status register,                     Address offset: 0x30 */
    uint32_t MISR;        /*!< TAMP masked interrupt status register,    Address offset: 0x34 */
    uint32_t SMISR;       /*!< TAMP secure masked interrupt status register, Address offset: 0x38 */
    uint32_t SCR;         /*!< TAMP status clear register,               Address offset: 0x3C */
    uint32_t COUNT1R;     /*!< TAMP monotonic counter register,          Address offset: 0x40 */
    uint32_t RESERVED1[3];/*!< Reserved,                                 Address offset: 0x44 -- 0x4C */
    uint32_t OR;          /*!< TAMP option register,                     Address offset: 0x50 */
    uint32_t ERCFGR;      /*!< TAMP erase configuration register,        Address offset: 0x54 */
    uint32_t RESERVED2[42];/*!< Reserved,                                Address offset: 0x58 -- 0xFC */
    uint32_t BKP0R;       /*!< TAMP backup register 0,                   Address offset: 0x100 */
    uint32_t BKP1R;       /*!< TAMP backup register 1,                   Address offset: 0x104 */
    uint32_t BKP2R;       /*!< TAMP backup register 2,                   Address offset: 0x108 */
    uint32_t BKP3R;       /*!< TAMP backup register 3,                   Address offset: 0x10C */
    uint32_t BKP4R;       /*!< TAMP backup register 4,                   Address offset: 0x110 */
    uint32_t BKP5R;       /*!< TAMP backup register 5,                   Address offset: 0x114 */
    uint32_t BKP6R;       /*!< TAMP backup register 6,                   Address offset: 0x118 */
    uint32_t BKP7R;       /*!< TAMP backup register 7,                   Address offset: 0x11C */
    uint32_t BKP8R;       /*!< TAMP backup register 8,                   Address offset: 0x120 */
    uint32_t BKP9R;       /*!< TAMP backup register 9,                   Address offset: 0x124 */
    uint32_t BKP10R;      /*!< TAMP backup register 10,                  Address offset: 0x128 */
    uint32_t BKP11R;      /*!< TAMP backup register 11,                  Address offset: 0x12C */
    uint32_t BKP12R;      /*!< TAMP backup register 12,                  Address offset: 0x130 */
    uint32_t BKP13R;      /*!< TAMP backup register 13,                  Address offset: 0x134 */
    uint32_t BKP14R;      /*!< TAMP backup register 14,                  Address offset: 0x138 */
    uint32_t BKP15R;      /*!< TAMP backup register 15,                  Address offset: 0x13C */
    uint32_t BKP16R;      /*!< TAMP backup register 16,                  Address offset: 0x140 */
    uint32_t BKP17R;      /*!< TAMP backup register 17,                  Address offset: 0x144 */
    uint32_t BKP18R;      /*!< TAMP backup register 18,                  Address offset: 0x148 */
    uint32_t BKP19R;      /*!< TAMP backup register 19,                  Address offset: 0x14C */
    uint32_t BKP20R;      /*!< TAMP backup register 20,                  Address offset: 0x150 */
    uint32_t BKP21R;      /*!< TAMP backup register 21,                  Address offset: 0x154 */
    uint32_t BKP22R;      /*!< TAMP backup register 22,                  Address offset: 0x158 */
    uint32_t BKP23R;      /*!< TAMP backup register 23,                  Address offset: 0x15C */
    uint32_t BKP24R;      /*!< TAMP backup register 24,                  Address offset: 0x160 */
    uint32_t BKP25R;      /*!< TAMP backup register 25,                  Address offset: 0x164 */
    uint32_t BKP26R;      /*!< TAMP backup register 26,                  Address offset: 0x168 */
    uint32_t BKP27R;      /*!< TAMP backup register 27,                  Address offset: 0x16C */
    uint32_t BKP28R;      /*!< TAMP backup register 28,                  Address offset: 0x170 */
    uint32_t BKP29R;      /*!< TAMP backup register 29,                  Address offset: 0x174 */
    uint32_t BKP30R;      /*!< TAMP backup register 30,                  Address offset: 0x178 */
    uint32_t BKP31R;      /*!< TAMP backup register 31,                  Address offset: 0x17C */
  };
};
