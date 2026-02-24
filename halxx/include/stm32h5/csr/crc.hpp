#pragma once

#include <csr.hpp>

namespace CSR
{
  struct crc
  {
    uint32_t DR;             /*!< CRC Data register,                           Address offset: 0x00 */
    uint32_t IDR;            /*!< CRC Independent data register,               Address offset: 0x04 */
    uint32_t CR;             /*!< CRC Control register,                        Address offset: 0x08 */
    uint32_t RESERVED2;      /*!< Reserved,                                                    0x0C */
    uint32_t INIT;           /*!< Initial CRC value register,                  Address offset: 0x10 */
    uint32_t POL;            /*!< CRC polynomial register,                     Address offset: 0x14 */
  }    
};
