#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct i2c
  {
    uint32_t CR1;         /*!< I2C Control register 1,            Address offset: 0x00 */
    uint32_t CR2;         /*!< I2C Control register 2,            Address offset: 0x04 */
    uint32_t OAR1;        /*!< I2C Own address 1 register,        Address offset: 0x08 */
    uint32_t OAR2;        /*!< I2C Own address 2 register,        Address offset: 0x0C */
    uint32_t TIMINGR;     /*!< I2C Timing register,               Address offset: 0x10 */
    uint32_t TIMEOUTR;    /*!< I2C Timeout register,              Address offset: 0x14 */
    uint32_t ISR;         /*!< I2C Interrupt and status register, Address offset: 0x18 */
    uint32_t ICR;         /*!< I2C Interrupt clear register,      Address offset: 0x1C */
    uint32_t PECR;        /*!< I2C PEC register,                  Address offset: 0x20 */
    uint32_t RXDR;        /*!< I2C Receive data register,         Address offset: 0x24 */
    uint32_t TXDR;        /*!< I2C Transmit data register,        Address offset: 0x28 */
  };
};
