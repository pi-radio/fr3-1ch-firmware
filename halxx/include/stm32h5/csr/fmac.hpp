#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct fmac
  {
    uint32_t X1BUFCFG;        /*!< FMAC X1 Buffer Configuration register, Address offset: 0x00          */
    uint32_t X2BUFCFG;        /*!< FMAC X2 Buffer Configuration register, Address offset: 0x04          */
    uint32_t YBUFCFG;         /*!< FMAC Y Buffer Configuration register,  Address offset: 0x08          */
    uint32_t PARAM;           /*!< FMAC Parameter register,               Address offset: 0x0C          */
    uint32_t CR;              /*!< FMAC Control register,                 Address offset: 0x10          */
    uint32_t SR;              /*!< FMAC Status register,                  Address offset: 0x14          */
    uint32_t WDATA;           /*!< FMAC Write Data register,              Address offset: 0x18          */
    uint32_t RDATA;           /*!< FMAC Read Data register,               Address offset: 0x1C          */
  };
};
