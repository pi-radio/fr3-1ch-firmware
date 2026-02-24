  __IO uint32_t CR;             /*!< TZSC control register,                                                Address offset: 0x00      */
       uint32_t RESERVED1[3];   /*!< Reserved1,                                                            Address offset: 0x04-0x0C */
  __IO uint32_t SECCFGR1;       /*!< TZSC secure configuration register 1,                                 Address offset: 0x10      */
  __IO uint32_t SECCFGR2;       /*!< TZSC secure configuration register 2,                                 Address offset: 0x14      */
  __IO uint32_t SECCFGR3;       /*!< TZSC secure configuration register 3,                                 Address offset: 0x18      */
       uint32_t RESERVED2;      /*!< Reserved2,                                                            Address offset: 0x1C      */
  __IO uint32_t PRIVCFGR1;      /*!< TZSC privilege configuration register 1,                              Address offset: 0x20      */
  __IO uint32_t PRIVCFGR2;      /*!< TZSC privilege configuration register 2,                              Address offset: 0x24      */
  __IO uint32_t PRIVCFGR3;      /*!< TZSC privilege configuration register 3,                              Address offset: 0x28      */
       uint32_t RESERVED3[5];   /*!< Reserved3,                                                            Address offset: 0x2C-0x3C */
  __IO uint32_t MPCWM1ACFGR;    /*!< TZSC memory 1 sub-region A watermark configuration register,          Address offset: 0x40      */
  __IO uint32_t MPCWM1AR;       /*!< TZSC memory 1 sub-region A watermark register,                        Address offset: 0x44      */
  __IO uint32_t MPCWM1BCFGR;    /*!< TZSC memory 1 sub-region B watermark configuration register,          Address offset: 0x48      */
  __IO uint32_t MPCWM1BR;       /*!< TZSC memory 1 sub-region B watermark register,                        Address offset: 0x4C      */
  __IO uint32_t MPCWM2ACFGR;    /*!< TZSC memory 2 sub-region A watermark configuration register,          Address offset: 0x50      */
  __IO uint32_t MPCWM2AR;       /*!< TZSC memory 2 sub-region A watermark register,                        Address offset: 0x54      */
  __IO uint32_t MPCWM2BCFGR;    /*!< TZSC memory 2 sub-region B watermark configuration register,          Address offset: 0x58      */
  __IO uint32_t MPCWM2BR;       /*!< TZSC memory 2 sub-region B watermark register,                        Address offset: 0x5C      */
  __IO uint32_t MPCWM3ACFGR;    /*!< TZSC memory 3 sub-region A watermark configuration register,          Address offset: 0x60      */
  __IO uint32_t MPCWM3AR;       /*!< TZSC memory 3 sub-region A watermark register,                        Address offset: 0x64      */
  __IO uint32_t MPCWM3BCFGR;    /*!< TZSC memory 3 sub-region B watermark configuration register,          Address offset: 0x68      */
  __IO uint32_t MPCWM3BR;       /*!< TZSC memory 3 sub-region B watermark register,                        Address offset: 0x6C      */
  __IO uint32_t MPCWM4ACFGR;    /*!< TZSC memory 4 sub-region A watermark configuration register,          Address offset: 0x70      */
  __IO uint32_t MPCWM4AR;       /*!< TZSC memory 4 sub-region A watermark register,                        Address offset: 0x74      */
  __IO uint32_t MPCWM4BCFGR;    /*!< TZSC memory 4 sub-region B watermark configuration register,          Address offset: 0x78      */
  __IO uint32_t MPCWM4BR;       /*!< TZSC memory 4 sub-region B watermark register,                        Address offset: 0x7c      */

typedef struct
{
  __IO uint32_t CR;             /*!< MPCBBx control register,                  Address offset: 0x00        */
  uint32_t RESERVED1[3];        /*!< Reserved1,                                Address offset: 0x04-0x0C   */
  __IO uint32_t CFGLOCKR1;      /*!< MPCBBx lock register,                     Address offset: 0x10        */
  uint32_t RESERVED2[59];       /*!< Reserved2,                                Address offset: 0x14-0xFC   */
  __IO uint32_t SECCFGR[32];    /*!< MPCBBx security configuration registers,  Address offset: 0x100-0x17C */
  uint32_t RESERVED3[32];       /*!< Reserved3,                                Address offset: 0x180-0x1FC */
  __IO uint32_t PRIVCFGR[32];   /*!< MPCBBx privilege configuration registers, Address offset: 0x200-0x280 */
} GTZC_MPCBB_TypeDef;

{
  __IO uint32_t IER1;           /*!< TZIC interrupt enable register 1, Address offset: 0x00 */
  __IO uint32_t IER2;           /*!< TZIC interrupt enable register 2, Address offset: 0x04 */
  __IO uint32_t IER3;           /*!< TZIC interrupt enable register 3, Address offset: 0x08 */
  __IO uint32_t IER4;           /*!< TZIC interrupt enable register 4, Address offset: 0x0C */
  __IO uint32_t SR1;            /*!< TZIC status register 1,           Address offset: 0x10 */
  __IO uint32_t SR2;            /*!< TZIC status register 2,           Address offset: 0x14 */
  __IO uint32_t SR3;            /*!< TZIC status register 3,           Address offset: 0x18 */
  __IO uint32_t SR4;            /*!< TZIC status register 4,           Address offset: 0x1C */
  __IO uint32_t FCR1;           /*!< TZIC flag clear register 1,       Address offset: 0x20 */
  __IO uint32_t FCR2;           /*!< TZIC flag clear register 2,       Address offset: 0x24 */
  __IO uint32_t FCR3;           /*!< TZIC flag clear register 3,       Address offset: 0x28 */
  __IO uint32_t FCR4;           /*!< TZIC flag clear register 3,       Address offset: 0x2C */
} GTZC_TZIC_TypeDef;
