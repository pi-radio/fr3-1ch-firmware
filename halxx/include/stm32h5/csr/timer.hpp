#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct timer
  {
    uint32_t CR1;         /*!< TIM control register 1,                   Address offset: 0x00 */
    uint32_t CR2;         /*!< TIM control register 2,                   Address offset: 0x04 */
    uint32_t SMCR;        /*!< TIM slave mode control register,          Address offset: 0x08 */
    uint32_t DIER;        /*!< TIM DMA/interrupt enable register,        Address offset: 0x0C */
    uint32_t SR;          /*!< TIM status register,                      Address offset: 0x10 */
    uint32_t EGR;         /*!< TIM event generation register,            Address offset: 0x14 */
    uint32_t CCMR1;       /*!< TIM capture/compare mode register 1,      Address offset: 0x18 */
    uint32_t CCMR2;       /*!< TIM capture/compare mode register 2,      Address offset: 0x1C */
    uint32_t CCER;        /*!< TIM capture/compare enable register,      Address offset: 0x20 */
    uint32_t CNT;         /*!< TIM counter register,                     Address offset: 0x24 */
    uint32_t PSC;         /*!< TIM prescaler,                            Address offset: 0x28 */
    uint32_t ARR;         /*!< TIM auto-reload register,                 Address offset: 0x2C */
    uint32_t RCR;         /*!< TIM repetition counter register,          Address offset: 0x30 */
    uint32_t CCR1;        /*!< TIM capture/compare register 1,           Address offset: 0x34 */
    uint32_t CCR2;        /*!< TIM capture/compare register 2,           Address offset: 0x38 */
    uint32_t CCR3;        /*!< TIM capture/compare register 3,           Address offset: 0x3C */
    uint32_t CCR4;        /*!< TIM capture/compare register 4,           Address offset: 0x40 */
    uint32_t BDTR;        /*!< TIM break and dead-time register,         Address offset: 0x44 */
    uint32_t CCR5;        /*!< TIM capture/compare register 5,           Address offset: 0x48 */
    uint32_t CCR6;        /*!< TIM capture/compare register 6,           Address offset: 0x4C */
    uint32_t CCMR3;       /*!< TIM capture/compare mode register 3,      Address offset: 0x50 */
    uint32_t DTR2;        /*!< TIM deadtime register 2,                  Address offset: 0x54 */
    uint32_t ECR;         /*!< TIM encoder control register,             Address offset: 0x58 */
    uint32_t TISEL;       /*!< TIM Input Selection register,             Address offset: 0x5C */
    uint32_t AF1;         /*!< TIM alternate function option register 1, Address offset: 0x60 */
    uint32_t AF2;         /*!< TIM alternate function option register 2, Address offset: 0x64 */
    uint32_t OR1 ;        /*!< TIM option register,                      Address offset: 0x68 */
    uint32_t RESERVED0[220];/*!< Reserved,                               Address offset: 0x6C */
    uint32_t DCR;         /*!< TIM DMA control register,                 Address offset: 0x3DC */
    uint32_t DMAR;        /*!< TIM DMA address for full transfer,        Address offset: 0x3E0 */
  };

  struct lptimer
  {
    uint32_t ISR;            /*!< LPTIM Interrupt and Status register,    Address offset: 0x00 */
    uint32_t ICR;            /*!< LPTIM Interrupt Clear register,         Address offset: 0x04 */
    uint32_t DIER;           /*!< LPTIM Interrupt Enable register,        Address offset: 0x08 */
    uint32_t CFGR;           /*!< LPTIM Configuration register,           Address offset: 0x0C */
    uint32_t CR;             /*!< LPTIM Control register,                 Address offset: 0x10 */
    uint32_t CCR1;           /*!< LPTIM Capture/Compare register 1,       Address offset: 0x14 */
    uint32_t ARR;            /*!< LPTIM Autoreload register,              Address offset: 0x18 */
    uint32_t CNT;            /*!< LPTIM Counter register,                 Address offset: 0x1C */
    uint32_t RESERVED0;      /*!< Reserved,                               Address offset: 0x20 */
    uint32_t CFGR2;          /*!< LPTIM Configuration register 2,         Address offset: 0x24 */
    uint32_t RCR;            /*!< LPTIM Repetition register,              Address offset: 0x28 */
    uint32_t CCMR1;          /*!< LPTIM Capture/Compare mode register,    Address offset: 0x2C */
    uint32_t RESERVED1;      /*!< Reserved,                               Address offset: 0x30 */
    uint32_t CCR2;           /*!< LPTIM Capture/Compare register 2,       Address offset: 0x34 */
  };
};
