#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct iwdg
  {
    uint32_t KR;            /*!< IWDG Key register,          Address offset: 0x00 */
    uint32_t PR;            /*!< IWDG Prescaler register,    Address offset: 0x04 */
    uint32_t RLR;           /*!< IWDG Reload register,       Address offset: 0x08 */
    uint32_t SR;            /*!< IWDG Status register,       Address offset: 0x0C */
    uint32_t WINR;          /*!< IWDG Window register,       Address offset: 0x10 */
    uint32_t EWCR;          /*!< IWDG Early Wakeup register, Address offset: 0x14 */
  };
};
