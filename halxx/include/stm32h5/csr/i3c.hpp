#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct i3c
  {
    uint32_t CR;             /*!< I3C Control register,                      Address offset: 0x00      */
    uint32_t CFGR;           /*!< I3C Controller Configuration register,     Address offset: 0x04      */
    uint32_t      RESERVED1[2];   /*!< Reserved,                                  Address offset: 0x08-0x0C */
    uint32_t RDR;            /*!< I3C Received Data register,                Address offset: 0x10      */
    uint32_t RDWR;           /*!< I3C Received Data Word register,           Address offset: 0x14      */
    uint32_t TDR;            /*!< I3C Transmit Data register,                Address offset: 0x18      */
    uint32_t TDWR;           /*!< I3C Transmit Data Word register,           Address offset: 0x1C      */
    uint32_t IBIDR;          /*!< I3C IBI payload Data register,             Address offset: 0x20      */
    uint32_t TGTTDR;         /*!< I3C Target Transmit register,              Address offset: 0x24      */
    uint32_t      RESERVED2[2];   /*!< Reserved,                                  Address offset: 0x28-0x2C */
    uint32_t SR;             /*!< I3C Status register,                       Address offset: 0x30      */
    uint32_t SER;            /*!< I3C Status Error register,                 Address offset: 0x34      */
    uint32_t      RESERVED3[2];   /*!< Reserved,                                  Address offset: 0x38-0x3C */
    uint32_t RMR;            /*!< I3C Received Message register,             Address offset: 0x40      */
    uint32_t      RESERVED4[3];   /*!< Reserved,                                  Address offset: 0x44-0x4C */
    uint32_t EVR;            /*!< I3C Event register,                        Address offset: 0x50      */
    uint32_t IER;            /*!< I3C Interrupt Enable register,             Address offset: 0x54      */
    uint32_t CEVR;           /*!< I3C Clear Event register,                  Address offset: 0x58      */
    uint32_t RESERVED5;           /*!< Reserved,                                  Address offset: 0x5C      */
    uint32_t DEVR0;          /*!< I3C own Target characteristics register,   Address offset: 0x60      */
    uint32_t DEVRX[4];       /*!< I3C Target x (1<=x<=4) register,           Address offset: 0x64-0x70 */
    uint32_t      RESERVED6[7];   /*!< Reserved,                                  Address offset: 0x74-0x8C */
    uint32_t MAXRLR;         /*!< I3C Maximum Read Length register,          Address offset: 0x90      */
    uint32_t MAXWLR;         /*!< I3C Maximum Write Length register,         Address offset: 0x94      */
    uint32_t      RESERVED7[2];   /*!< Reserved,                                  Address offset: 0x98-0x9C */
    uint32_t TIMINGR0;       /*!< I3C Timing 0 register,                     Address offset: 0xA0      */
    uint32_t TIMINGR1;       /*!< I3C Timing 1 register,                     Address offset: 0xA4      */
    uint32_t TIMINGR2;       /*!< I3C Timing 2 register,                     Address offset: 0xA8      */
    uint32_t      RESERVED9[5];   /*!< Reserved,                                  Address offset: 0xAC-0xBC */
    uint32_t BCR;            /*!< I3C Bus Characteristics register,          Address offset: 0xC0      */
    uint32_t DCR;            /*!< I3C Device Characteristics register,       Address offset: 0xC4      */
    uint32_t GETCAPR;        /*!< I3C GET CAPabilities register,             Address offset: 0xC8      */
    uint32_t CRCAPR;         /*!< I3C Controller CAPabilities register,      Address offset: 0xCC      */
    uint32_t GETMXDSR;       /*!< I3C GET Max Data Speed register,           Address offset: 0xD0      */
    uint32_t EPIDR;          /*!< I3C Extended Provisioned ID register,      Address offset: 0xD4      */
  };
};
