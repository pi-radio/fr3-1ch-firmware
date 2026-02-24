#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct usart
  {
    uint32_t CR1;         /*!< USART Control register 1,                 Address offset: 0x00  */
    uint32_t CR2;         /*!< USART Control register 2,                 Address offset: 0x04  */
    uint32_t CR3;         /*!< USART Control register 3,                 Address offset: 0x08  */
    uint32_t BRR;         /*!< USART Baud rate register,                 Address offset: 0x0C  */
    uint32_t GTPR;        /*!< USART Guard time and prescaler register,  Address offset: 0x10  */
    uint32_t RTOR;        /*!< USART Receiver Time Out register,         Address offset: 0x14  */
    uint32_t RQR;         /*!< USART Request register,                   Address offset: 0x18  */
    uint32_t ISR;         /*!< USART Interrupt and status register,      Address offset: 0x1C  */
    uint32_t ICR;         /*!< USART Interrupt flag Clear register,      Address offset: 0x20  */
    uint32_t RDR;         /*!< USART Receive Data register,              Address offset: 0x24  */
    uint32_t TDR;         /*!< USART Transmit Data register,             Address offset: 0x28  */
    uint32_t PRESC;       /*!< USART Prescaler register,                 Address offset: 0x2C  */
  };
};
