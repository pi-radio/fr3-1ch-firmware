#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct dma_csr
  {
    uint32_t SECCFGR;     /*!< DMA secure configuration register,               Address offset: 0x00  */
    uint32_t PRIVCFGR;    /*!< DMA privileged configuration register,           Address offset: 0x04  */
    uint32_t RCFGLOCKR;   /*!< DMA lock configuration register,                 Address offset: 0x08  */
    uint32_t MISR;        /*!< DMA non secure masked interrupt status register, Address offset: 0x0C  */
    uint32_t SMISR;       /*!< DMA secure masked interrupt status register,     Address offset: 0x10  */
  };

  struct dma_channel
  {
    uint32_t CLBAR;        /*!< DMA channel x linked-list base address register, Address offset: 0x50 + (x * 0x80) */
    uint32_t RESERVED1[2]; /*!< Reserved 1,                                      Address offset: 0x54 -- 0x58      */
    uint32_t CFCR;         /*!< DMA channel x flag clear register,               Address offset: 0x5C + (x * 0x80) */
    uint32_t CSR;          /*!< DMA channel x flag status register,              Address offset: 0x60 + (x * 0x80) */
    uint32_t CCR;          /*!< DMA channel x control register,                  Address offset: 0x64 + (x * 0x80) */
    uint32_t RESERVED2[10];/*!< Reserved 2,                                      Address offset: 0x68 -- 0x8C      */
    uint32_t CTR1;         /*!< DMA channel x transfer register 1,               Address offset: 0x90 + (x * 0x80) */
    uint32_t CTR2;         /*!< DMA channel x transfer register 2,               Address offset: 0x94 + (x * 0x80) */
    uint32_t CBR1;         /*!< DMA channel x block register 1,                  Address offset: 0x98 + (x * 0x80) */
    uint32_t CSAR;         /*!< DMA channel x source address register,           Address offset: 0x9C + (x * 0x80) */
    uint32_t CDAR;         /*!< DMA channel x destination address register,      Address offset: 0xA0 + (x * 0x80) */
    uint32_t CTR3;         /*!< DMA channel x transfer register 3,               Address offset: 0xA4 + (x * 0x80) */
    uint32_t CBR2;         /*!< DMA channel x block register 2,                  Address offset: 0xA8 + (x * 0x80) */
    uint32_t RESERVED3[8]; /*!< Reserved 3,                                      Address offset: 0xAC -- 0xC8      */
    uint32_t CLLR;         /*!< DMA channel x linked-list address register,      Address offset: 0xCC + (x * 0x80) */
  };

  template <uint32_t base>
  struct dmac
  {
    csr_ptr<dma_csr, base + 0> ctrl;
    csr_ptr<dma_channel, base + 0x50> ch0;
    csr_ptr<dma_channel, base + 0xD0> ch1;
    csr_ptr<dma_channel, base + 0x150> ch2;
    csr_ptr<dma_channel, base + 0x1D0> ch3;
    csr_ptr<dma_channel, base + 0x250> ch4;
    csr_ptr<dma_channel, base + 0x2D0> ch5;
    csr_ptr<dma_channel, base + 0x350> ch6;
    csr_ptr<dma_channel, base + 0x3D0> ch7;
  };
}
