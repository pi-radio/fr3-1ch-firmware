#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct fmc_bank1
  {
    uint32_t BTCR[8];     /*!< NOR/PSRAM chip-select control register(BCR) and chip-select timing register(BTR), Address offset: 0x00-1C */
    uint32_t PCSCNTR;     /*!< PSRAM chip-select counter register,                                               Address offset:    0x20 */
  };

  struct fmc_bank1e
  {
    uint32_t BWTR[7];     /*!< NOR/PSRAM write timing registers, Address offset: 0x104-0x11C */
  };

  struct fmc_bank3
  {
    uint32_t PCR;        /*!< NAND Flash control register,                       Address offset: 0x80 */
    uint32_t SR;         /*!< NAND Flash FIFO status and interrupt register,     Address offset: 0x84 */
    uint32_t PMEM;       /*!< NAND Flash Common memory space timing register,    Address offset: 0x88 */
    uint32_t PATT;       /*!< NAND Flash Attribute memory space timing register, Address offset: 0x8C */
    uint32_t      RESERVED0;  /*!< Reserved, 0x90                                                          */
    uint32_t ECCR;       /*!< NAND Flash ECC result registers,                   Address offset: 0x94 */
  };

  struct fmc_bank56
  {
    uint32_t SDCR[2];     /*!< SDRAM Control registers ,       Address offset: 0x140-0x144  */
    uint32_t SDTR[2];     /*!< SDRAM Timing registers ,        Address offset: 0x148-0x14C  */
    uint32_t SDCMR;       /*!< SDRAM Command Mode register,    Address offset: 0x150        */
    uint32_t SDRTR;       /*!< SDRAM Refresh Timer register,   Address offset: 0x154        */
    uint32_t SDSR;        /*!< SDRAM Status register,          Address offset: 0x158        */
  };
};
