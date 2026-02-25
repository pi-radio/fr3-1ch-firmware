#pragma once

#include <stm32h5/csr/csr.hpp>

namespace CSR
{
  struct flash
  {
    uint32_t ACR;             /*!< FLASH access control register,                                     Address offset: 0x00 */
    uint32_t NSKEYR;          /*!< FLASH non-secure key register,                                     Address offset: 0x04 */
    uint32_t SECKEYR;         /*!< FLASH secure key register,                                         Address offset: 0x08 */
    uint32_t OPTKEYR;         /*!< FLASH option key register,                                         Address offset: 0x0C */
    uint32_t NSOBKKEYR;       /*!< FLASH non-secure option bytes keys key register,                   Address offset: 0x10 */
    uint32_t SECOBKKEYR;      /*!< FLASH secure option bytes keys key register,                       Address offset: 0x14 */
    uint32_t OPSR;            /*!< FLASH OPSR register,                                               Address offset: 0x18 */
    uint32_t OPTCR;           /*!< Flash Option Control Register,                                     Address offset: 0x1C */
    uint32_t NSSR;            /*!< FLASH non-secure status register,                                  Address offset: 0x20 */
    uint32_t SECSR;           /*!< FLASH secure status register,                                      Address offset: 0x24 */
    uint32_t NSCR;            /*!< FLASH non-secure control register,                                 Address offset: 0x28 */
    uint32_t SECCR;           /*!< FLASH secure control register,                                     Address offset: 0x2C */
    uint32_t NSCCR;           /*!< FLASH non-secure clear control register,                           Address offset: 0x30 */
    uint32_t SECCCR;          /*!< FLASH secure clear control register,                               Address offset: 0x34 */
    uint32_t RESERVED1;       /*!< Reserved1,                                                         Address offset: 0x38 */
    uint32_t PRIVCFGR;        /*!< FLASH privilege configuration register,                            Address offset: 0x3C */
    uint32_t NSOBKCFGR;       /*!< FLASH non-secure option byte key configuration register,           Address offset: 0x40 */
    uint32_t SECOBKCFGR;      /*!< FLASH secure option byte key configuration register,               Address offset: 0x44 */
    uint32_t HDPEXTR;         /*!< FLASH HDP extension register,                                      Address offset: 0x48 */
    uint32_t RESERVED2;       /*!< Reserved2,                                                         Address offset: 0x4C */
    uint32_t OPTSR_CUR;       /*!< FLASH option status current register,                              Address offset: 0x50 */
    uint32_t OPTSR_PRG;       /*!< FLASH option status to program register,                           Address offset: 0x54 */
    uint32_t RESERVED3[2];    /*!< Reserved3,                                                         Address offset: 0x58-0x5C */
    uint32_t NSEPOCHR_CUR;    /*!< FLASH non-secure epoch current register,                           Address offset: 0x60 */
    uint32_t NSEPOCHR_PRG;    /*!< FLASH non-secure epoch to program register,                        Address offset: 0x64 */
    uint32_t SECEPOCHR_CUR;   /*!< FLASH secure epoch current register,                               Address offset: 0x68 */
    uint32_t SECEPOCHR_PRG;   /*!< FLASH secure epoch to program register,                            Address offset: 0x6C */
    uint32_t OPTSR2_CUR;      /*!< FLASH option status current register 2,                            Address offset: 0x70 */
    uint32_t OPTSR2_PRG;      /*!< FLASH option status to program register 2,                         Address offset: 0x74 */
    uint32_t RESERVED4[2];    /*!< Reserved4,                                                         Address offset: 0x78-0x7C */
    uint32_t NSBOOTR_CUR;     /*!< FLASH non-secure unique boot entry current register,               Address offset: 0x80 */
    uint32_t NSBOOTR_PRG;     /*!< FLASH non-secure unique boot entry to program register,            Address offset: 0x84 */
    uint32_t SECBOOTR_CUR;    /*!< FLASH secure unique boot entry current register,                   Address offset: 0x88 */
    uint32_t SECBOOTR_PRG;    /*!< FLASH secure unique boot entry to program register,                Address offset: 0x8C */
    uint32_t OTPBLR_CUR;      /*!< FLASH OTP block lock current register,                             Address offset: 0x90 */
    uint32_t OTPBLR_PRG;      /*!< FLASH OTP block Lock to program register,                          Address offset: 0x94 */
    uint32_t RESERVED5[2];    /*!< Reserved5,                                                         Address offset: 0x98-0x9C */
    uint32_t SECBB1R1;        /*!< FLASH secure block-based bank 1 register 1,                        Address offset: 0xA0 */
    uint32_t SECBB1R2;        /*!< FLASH secure block-based bank 1 register 2,                        Address offset: 0xA4 */
    uint32_t SECBB1R3;        /*!< FLASH secure block-based bank 1 register 3,                        Address offset: 0xA8 */
    uint32_t SECBB1R4;        /*!< FLASH secure block-based bank 1 register 4,                        Address offset: 0xAC */
    uint32_t RESERVED6[4];    /*!< Reserved6,                                                         Address offset: 0xB0-0xBC */
    uint32_t PRIVBB1R1;       /*!< FLASH privilege block-based bank 1 register 1,                     Address offset: 0xC0 */
    uint32_t PRIVBB1R2;       /*!< FLASH privilege block-based bank 1 register 2,                     Address offset: 0xC4 */
    uint32_t PRIVBB1R3;       /*!< FLASH privilege block-based bank 1 register 3,                     Address offset: 0xC8 */
    uint32_t PRIVBB1R4;       /*!< FLASH privilege block-based bank 1 register 4,                     Address offset: 0xCC */
    uint32_t RESERVED7[4];    /*!< Reserved7,                                                         Address offset: 0xD0-0xDC */
    uint32_t SECWM1R_CUR;     /*!< FLASH secure watermark 1 current register,                         Address offset: 0xE0 */
    uint32_t SECWM1R_PRG;     /*!< FLASH secure watermark 1 to program register,                      Address offset: 0xE4 */
    uint32_t WRP1R_CUR;       /*!< FLASH write sector group protection current register for bank1,    Address offset: 0xE8 */
    uint32_t WRP1R_PRG;       /*!< FLASH write sector group protection to program register for bank1, Address offset: 0xEC */
    uint32_t EDATA1R_CUR;     /*!< FLASH data sectors configuration current register for bank1,       Address offset: 0xF0 */
    uint32_t EDATA1R_PRG;     /*!< FLASH data sectors configuration to program register for bank1,    Address offset: 0xF4 */
    uint32_t HDP1R_CUR;       /*!< FLASH HDP configuration current register for bank1,                Address offset: 0xF8 */
    uint32_t HDP1R_PRG;       /*!< FLASH HDP configuration to program register for bank1,             Address offset: 0xFC */
    uint32_t ECCCORR;         /*!< FLASH ECC correction register,                                     Address offset: 0x100 */
    uint32_t ECCDETR;         /*!< FLASH ECC detection register,                                      Address offset: 0x104 */
    uint32_t ECCDR;           /*!< FLASH ECC data register,                                           Address offset: 0x108 */
    uint32_t RESERVED8[37];   /*!< Reserved8,                                                         Address offset: 0x10C-0x19C */
    uint32_t SECBB2R1;        /*!< FLASH secure block-based bank 2 register 1,                        Address offset: 0x1A0 */
    uint32_t SECBB2R2;        /*!< FLASH secure block-based bank 2 register 2,                        Address offset: 0x1A4 */
    uint32_t SECBB2R3;        /*!< FLASH secure block-based bank 2 register 3,                        Address offset: 0x1A8 */
    uint32_t SECBB2R4;        /*!< FLASH secure block-based bank 2 register 4,                        Address offset: 0x1AC */
    uint32_t RESERVED9[4];    /*!< Reserved9,                                                         Address offset: 0x1B0-0x1BC */
    uint32_t PRIVBB2R1;       /*!< FLASH privilege block-based bank 2 register 1,                     Address offset: 0x1C0 */
    uint32_t PRIVBB2R2;       /*!< FLASH privilege block-based bank 2 register 2,                     Address offset: 0x1C4 */
    uint32_t PRIVBB2R3;       /*!< FLASH privilege block-based bank 2 register 3,                     Address offset: 0x1C8 */
    uint32_t PRIVBB2R4;       /*!< FLASH privilege block-based bank 2 register 4,                     Address offset: 0x1CC */
    uint32_t RESERVED10[4];   /*!< Reserved10,                                                        Address offset: 0x1D0-0x1DC */
    uint32_t SECWM2R_CUR;     /*!< FLASH secure watermark 2 current register,                         Address offset: 0x1E0 */
    uint32_t SECWM2R_PRG;     /*!< FLASH secure watermark 2 to program register,                      Address offset: 0x1E4 */
    uint32_t WRP2R_CUR;       /*!< FLASH write sector group protection current register for bank2,    Address offset: 0x1E8 */
    uint32_t WRP2R_PRG;       /*!< FLASH write sector group protection to program register for bank2, Address offset: 0x1EC */
    uint32_t EDATA2R_CUR;     /*!< FLASH data sectors configuration current register for bank2,       Address offset: 0x1F0 */
    uint32_t EDATA2R_PRG;     /*!< FLASH data sectors configuration to program register for bank2,    Address offset: 0x1F4 */
    uint32_t HDP2R_CUR;       /*!< FLASH HDP configuration current register for bank2,                Address offset: 0x1F8 */
    uint32_t HDP2R_PRG;       /*!< FLASH HDP configuration to program register for bank2,             Address offset: 0x1FC */
  };
};
