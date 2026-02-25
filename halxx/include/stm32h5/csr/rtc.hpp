#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct rtc
  {
    uint32_t TR;          /*!< RTC time register,                              Address offset: 0x00 */
    uint32_t DR;          /*!< RTC date register,                              Address offset: 0x04 */
    uint32_t SSR;         /*!< RTC sub second register,                        Address offset: 0x08 */
    uint32_t ICSR;        /*!< RTC initialization control and status register, Address offset: 0x0C */
    uint32_t PRER;        /*!< RTC prescaler register,                         Address offset: 0x10 */
    uint32_t WUTR;        /*!< RTC wakeup timer register,                      Address offset: 0x14 */
    uint32_t CR;          /*!< RTC control register,                           Address offset: 0x18 */
    uint32_t PRIVCFGR;    /*!< RTC privilege mode control register,            Address offset: 0x1C */
    uint32_t SECCFGR;     /*!< RTC secure mode control register,               Address offset: 0x20 */
    uint32_t WPR;         /*!< RTC write protection register,                  Address offset: 0x24 */
    uint32_t CALR;        /*!< RTC calibration register,                       Address offset: 0x28 */
    uint32_t SHIFTR;      /*!< RTC shift control register,                     Address offset: 0x2C */
    uint32_t TSTR;        /*!< RTC time stamp time register,                   Address offset: 0x30 */
    uint32_t TSDR;        /*!< RTC time stamp date register,                   Address offset: 0x34 */
    uint32_t TSSSR;       /*!< RTC time-stamp sub second register,             Address offset: 0x38 */
    uint32_t RESERVED0;   /*!< Reserved,                                       Address offset: 0x3C */
    uint32_t ALRMAR;      /*!< RTC alarm A register,                           Address offset: 0x40 */
    uint32_t ALRMASSR;    /*!< RTC alarm A sub second register,                Address offset: 0x44 */
    uint32_t ALRMBR;      /*!< RTC alarm B register,                           Address offset: 0x48 */
    uint32_t ALRMBSSR;    /*!< RTC alarm B sub second register,                Address offset: 0x4C */
    uint32_t SR;          /*!< RTC Status register,                            Address offset: 0x50 */
    uint32_t MISR;        /*!< RTC masked interrupt status register,           Address offset: 0x54 */
    uint32_t SMISR;       /*!< RTC secure masked interrupt status register,    Address offset: 0x58 */
    uint32_t SCR;         /*!< RTC status Clear register,                      Address offset: 0x5C */
    uint32_t OR;          /*!< RTC option register,                            Address offset: 0x60 */
    uint32_t RESERVED1[3];/*!< Reserved,                                       Address offset: 0x64 */
    uint32_t ALRABINR;    /*!< RTC alarm A binary mode register,               Address offset: 0x70 */
    uint32_t ALRBBINR;    /*!< RTC alarm B binary mode register,               Address offset: 0x74 */
  };
};
