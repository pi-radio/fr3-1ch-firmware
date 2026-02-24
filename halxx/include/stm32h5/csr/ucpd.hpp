#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct ucpd
  {
    uint32_t CFG1;         /*!< UCPD configuration register 1,             Address offset: 0x00 */
    uint32_t CFG2;         /*!< UCPD configuration register 2,             Address offset: 0x04 */
    uint32_t CFG3;         /*!< UCPD configuration register 3,             Address offset: 0x08 */
    uint32_t CR;           /*!< UCPD control register,                     Address offset: 0x0C */
    uint32_t IMR;          /*!< UCPD interrupt mask register,              Address offset: 0x10 */
    uint32_t SR;           /*!< UCPD status register,                      Address offset: 0x14 */
    uint32_t ICR;          /*!< UCPD interrupt flag clear register         Address offset: 0x18 */
    uint32_t TX_ORDSET;    /*!< UCPD Tx ordered set type register,         Address offset: 0x1C */
    uint32_t TX_PAYSZ;     /*!< UCPD Tx payload size register,             Address offset: 0x20 */
    uint32_t TXDR;         /*!< UCPD Tx data register,                     Address offset: 0x24 */
    uint32_t RX_ORDSET;    /*!< UCPD Rx ordered set type register,         Address offset: 0x28 */
    uint32_t RX_PAYSZ;     /*!< UCPD Rx payload size register,             Address offset: 0x2C */
    uint32_t RXDR;         /*!< UCPD Rx data register,                     Address offset: 0x30 */
    uint32_t RX_ORDEXT1;   /*!< UCPD Rx ordered set extension 1 register,  Address offset: 0x34 */
    uint32_t RX_ORDEXT2;   /*!< UCPD Rx ordered set extension 2 register,  Address offset: 0x38 */
    uint32_t RESERVED[949];/*!< Reserved,                                  Address offset: 0x3C -- 0x3F0 */
    uint32_t IPVER;        /*!< UCPD IP version register,                  Address offset: 0x3F4 */
    uint32_t IPID;         /*!< UCPD IP Identification register,           Address offset: 0x3F8 */
    uint32_t MID;          /*!< UCPD Magic Identification register,        Address offset: 0x3FC */
  };
};
