#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct spi
  {
    uint32_t CR1;           /*!< SPI/I2S Control register 1,                      Address offset: 0x00 */
    uint32_t CR2;           /*!< SPI Control register 2,                          Address offset: 0x04 */
    uint32_t CFG1;          /*!< SPI Configuration register 1,                    Address offset: 0x08 */
    uint32_t CFG2;          /*!< SPI Configuration register 2,                    Address offset: 0x0C */
    uint32_t IER;           /*!< SPI/I2S Interrupt Enable register,               Address offset: 0x10 */
    uint32_t SR;            /*!< SPI/I2S Status register,                         Address offset: 0x14 */
    uint32_t IFCR;          /*!< SPI/I2S Interrupt/Status flags clear register,   Address offset: 0x18 */
    uint32_t RESERVED0;     /*!< Reserved, 0x1C                                                        */
    uint32_t TXDR;          /*!< SPI/I2S Transmit data register,                  Address offset: 0x20 */
    uint32_t RESERVED1[3];  /*!< Reserved, 0x24-0x2C                                                   */
    uint32_t RXDR;          /*!< SPI/I2S Receive data register,                   Address offset: 0x30 */
    uint32_t RESERVED2[3];  /*!< Reserved, 0x34-0x3C                                                   */
    uint32_t CRCPOLY;       /*!< SPI CRC Polynomial register,                     Address offset: 0x40 */
    uint32_t TXCRC;         /*!< SPI Transmitter CRC register,                    Address offset: 0x44 */
    uint32_t RXCRC;         /*!< SPI Receiver CRC register,                       Address offset: 0x48 */
    uint32_t UDRDR;         /*!< SPI Underrun data register,                      Address offset: 0x4C */
    uint32_t I2SCFGR;       /*!< I2S Configuration register,                      Address offset: 0x50 */
  };
};
