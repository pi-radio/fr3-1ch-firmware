#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct usb_drd {
    uint32_t CHEP0R;          /*!< USB Channel/Endpoint 0 register,      Address offset: 0x00 */
    uint32_t CHEP1R;          /*!< USB Channel/Endpoint 1 register,      Address offset: 0x04 */
    uint32_t CHEP2R;          /*!< USB Channel/Endpoint 2 register,      Address offset: 0x08 */
    uint32_t CHEP3R;          /*!< USB Channel/Endpoint 3 register,      Address offset: 0x0C */
    uint32_t CHEP4R;          /*!< USB Channel/Endpoint 4 register,      Address offset: 0x10 */
    uint32_t CHEP5R;          /*!< USB Channel/Endpoint 5 register,      Address offset: 0x14 */
    uint32_t CHEP6R;          /*!< USB Channel/Endpoint 6 register,      Address offset: 0x18 */
    uint32_t CHEP7R;          /*!< USB Channel/Endpoint 7 register,      Address offset: 0x1C */
    uint32_t RESERVED0[8];    /*!< Reserved,                                                  */
    uint32_t CNTR;            /*!< Control register,                     Address offset: 0x40 */
    uint32_t ISTR;            /*!< Interrupt status register,            Address offset: 0x44 */
    uint32_t FNR;             /*!< Frame number register,                Address offset: 0x48 */
    uint32_t DADDR;           /*!< Device address register,              Address offset: 0x4C */
    uint32_t RESERVED1;       /*!< Reserved */
    uint32_t LPMCSR;          /*!< LPM Control and Status register,      Address offset: 0x54 */
    uint32_t BCDR;            /*!< Battery Charging detector register,   Address offset: 0x58 */
  };

  struct usb_drd_pma_buf
  {
    uint32_t TXBD;             /*!<Transmission buffer address*/
    uint32_t RXBD;             /*!<Reception buffer address */
  };
};
