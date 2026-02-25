#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct pssi
  {
    uint32_t CR;             /*!< PSSI control register,                 Address offset: 0x000 */
    uint32_t SR;             /*!< PSSI status register,                  Address offset: 0x004 */
    uint32_t RIS;            /*!< PSSI raw interrupt status register,    Address offset: 0x008 */
    uint32_t IER;            /*!< PSSI interrupt enable register,        Address offset: 0x00C */
    uint32_t MIS;            /*!< PSSI masked interrupt status register, Address offset: 0x010 */
    uint32_t ICR;            /*!< PSSI interrupt clear register,         Address offset: 0x014 */
    uint32_t RESERVED1[4];   /*!< Reserved,                                      0x018 - 0x024 */
    uint32_t DR;             /*!< PSSI data register,                    Address offset: 0x028 */
  };
};
