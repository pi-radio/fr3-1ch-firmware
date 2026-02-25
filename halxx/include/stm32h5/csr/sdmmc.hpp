#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct sdmmc
  {
    uint32_t POWER;          /*!< SDMMC power control register,             Address offset: 0x00  */
    uint32_t CLKCR;          /*!< SDMMC clock control register,             Address offset: 0x04  */
    uint32_t ARG;            /*!< SDMMC argument register,                  Address offset: 0x08  */
    uint32_t CMD;            /*!< SDMMC command register,                   Address offset: 0x0C  */
    uint32_t  RESPCMD;        /*!< SDMMC command response register,          Address offset: 0x10  */
    uint32_t  RESP1;          /*!< SDMMC response 1 register,                Address offset: 0x14  */
    uint32_t  RESP2;          /*!< SDMMC response 2 register,                Address offset: 0x18  */
    uint32_t  RESP3;          /*!< SDMMC response 3 register,                Address offset: 0x1C  */
    uint32_t  RESP4;          /*!< SDMMC response 4 register,                Address offset: 0x20  */
    uint32_t DTIMER;         /*!< SDMMC data timer register,                Address offset: 0x24  */
    uint32_t DLEN;           /*!< SDMMC data length register,               Address offset: 0x28  */
    uint32_t DCTRL;          /*!< SDMMC data control register,              Address offset: 0x2C  */
    uint32_t  DCOUNT;         /*!< SDMMC data counter register,              Address offset: 0x30  */
    uint32_t  STA;            /*!< SDMMC status register,                    Address offset: 0x34  */
    uint32_t ICR;            /*!< SDMMC interrupt clear register,           Address offset: 0x38  */
    uint32_t MASK;           /*!< SDMMC mask register,                      Address offset: 0x3C  */
    uint32_t ACKTIME;        /*!< SDMMC Acknowledgement timer register,     Address offset: 0x40  */
    uint32_t      RESERVED0[3];   /*!< Reserved, 0x44 - 0x4C - 0x4C                                    */
    uint32_t IDMACTRL;       /*!< SDMMC DMA control register,               Address offset: 0x50  */
    uint32_t IDMABSIZE;      /*!< SDMMC DMA buffer size register,           Address offset: 0x54  */
    uint32_t IDMABASER;      /*!< SDMMC DMA buffer base address register,   Address offset: 0x58  */
    uint32_t      RESERVED1[2];   /*!< Reserved, 0x60                                             */
    uint32_t IDMALAR;        /*!< SDMMC DMA linked list address register,   Address offset: 0x64  */
    uint32_t IDMABAR;        /*!< SDMMC DMA linked list memory base register,Address offset: 0x68 */
    uint32_t      RESERVED2[5];   /*!< Reserved, 0x6C-0x7C                                             */
    uint32_t FIFO;           /*!< SDMMC data FIFO register,                 Address offset: 0x80  */
  };
};
