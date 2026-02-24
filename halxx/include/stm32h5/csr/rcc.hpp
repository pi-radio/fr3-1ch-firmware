  __IO uint32_t CR;            /*!< RCC clock control register                                               Address offset: 0x00 */
  uint32_t      RESERVED1[3];  /*!< Reserved,                                                                Address offset: 0x04 */
  __IO uint32_t HSICFGR;       /*!< RCC HSI Clock Calibration Register,                                      Address offset: 0x10 */
  __IO uint32_t CRRCR;         /*!< RCC Clock Recovery RC Register,                                          Address offset: 0x14 */
  __IO uint32_t CSICFGR;       /*!< RCC CSI Clock Calibration Register,                                      Address offset: 0x18 */
  __IO uint32_t CFGR1;         /*!< RCC clock configuration register 1                                       Address offset: 0x1C */
  __IO uint32_t CFGR2;         /*!< RCC clock configuration register 2                                       Address offset: 0x20 */
  uint32_t      RESERVED2;     /*!< Reserved,                                                                Address offset: 0x24 */
  __IO uint32_t PLL1CFGR;      /*!< RCC PLL1 Configuration Register                                          Address offset: 0x28 */
  __IO uint32_t PLL2CFGR;      /*!< RCC PLL2 Configuration Register                                          Address offset: 0x2C */
  __IO uint32_t PLL3CFGR;      /*!< RCC PLL3 Configuration Register                                          Address offset: 0x30 */
  __IO uint32_t PLL1DIVR;      /*!< RCC PLL1 Dividers Configuration Register                                 Address offset: 0x34 */
  __IO uint32_t PLL1FRACR;     /*!< RCC PLL1 Fractional Divider Configuration Register                       Address offset: 0x38 */
  __IO uint32_t PLL2DIVR;      /*!< RCC PLL2 Dividers Configuration Register                                 Address offset: 0x3C */
  __IO uint32_t PLL2FRACR;     /*!< RCC PLL2 Fractional Divider Configuration Register                       Address offset: 0x40 */
  __IO uint32_t PLL3DIVR;      /*!< RCC PLL3 Dividers Configuration Register                                 Address offset: 0x44 */
  __IO uint32_t PLL3FRACR;     /*!< RCC PLL3 Fractional Divider Configuration Register                       Address offset: 0x48 */
  uint32_t      RESERVED5;     /*!< Reserved                                                                 Address offset: 0x4C */
  __IO uint32_t CIER;          /*!< RCC Clock Interrupt Enable Register                                      Address offset: 0x50 */
  __IO uint32_t CIFR;          /*!< RCC Clock Interrupt Flag Register                                        Address offset: 0x54 */
  __IO uint32_t CICR;          /*!< RCC Clock Interrupt Clear Register                                       Address offset: 0x58 */
  uint32_t      RESERVED6;     /*!< Reserved                                                                 Address offset: 0x5C */
  __IO uint32_t AHB1RSTR;      /*!< RCC AHB1 Peripherals Reset Register                                      Address offset: 0x60 */
  __IO uint32_t AHB2RSTR;      /*!< RCC AHB2 Peripherals Reset Register                                      Address offset: 0x64 */
  uint32_t      RESERVED7;     /*!< Reserved                                                                 Address offset: 0x68 */
  __IO uint32_t AHB4RSTR;      /*!< RCC AHB4 Peripherals Reset Register                                      Address offset: 0x6C */
  uint32_t      RESERVED9;     /*!< Reserved                                                                 Address offset: 0x70 */
  __IO uint32_t APB1LRSTR;     /*!< RCC APB1 Peripherals reset Low Word register                             Address offset: 0x74 */
  __IO uint32_t APB1HRSTR;     /*!< RCC APB1 Peripherals reset High Word register                            Address offset: 0x78 */
  __IO uint32_t APB2RSTR;      /*!< RCC APB2 Peripherals Reset Register                                      Address offset: 0x7C */
  __IO uint32_t APB3RSTR;      /*!< RCC APB3 Peripherals Reset Register                                      Address offset: 0x80 */
  uint32_t      RESERVED10;    /*!< Reserved                                                                 Address offset: 0x84 */
  __IO uint32_t AHB1ENR;       /*!< RCC AHB1 Peripherals Clock Enable Register                               Address offset: 0x88 */
  __IO uint32_t AHB2ENR;       /*!< RCC AHB2 Peripherals Clock Enable Register                               Address offset: 0x8C */
  uint32_t      RESERVED11;    /*!< Reserved                                                                 Address offset: 0x90 */
  __IO uint32_t AHB4ENR;       /*!< RCC AHB4 Peripherals Clock Enable Register                               Address offset: 0x94 */
  uint32_t      RESERVED13;    /*!< Reserved                                                                 Address offset: 0x98 */
  __IO uint32_t APB1LENR;      /*!< RCC APB1 Peripherals clock Enable Low Word register                      Address offset: 0x9C */
  __IO uint32_t APB1HENR;      /*!< RCC APB1 Peripherals clock Enable High Word register                     Address offset: 0xA0 */
  __IO uint32_t APB2ENR;       /*!< RCC APB2 Peripherals Clock Enable Register                               Address offset: 0xA4 */
  __IO uint32_t APB3ENR;       /*!< RCC APB3 Peripherals Clock Enable Register                               Address offset: 0xA8 */
  uint32_t      RESERVED14;    /*!< Reserved                                                                 Address offset: 0xAC */
  __IO uint32_t AHB1LPENR;     /*!< RCC AHB1 Peripheral sleep clock Register                                 Address offset: 0xB0 */
  __IO uint32_t AHB2LPENR;     /*!< RCC AHB2 Peripheral sleep clock Register                                 Address offset: 0xB4 */
  uint32_t      RESERVED15;    /*!< Reserved                                                                 Address offset: 0xB8 */
  __IO uint32_t AHB4LPENR;     /*!< RCC AHB4 Peripherals sleep clock Register                                Address offset: 0xBC */
  uint32_t      RESERVED17;    /*!< Reserved                                                                 Address offset: 0xC0 */
  __IO uint32_t APB1LLPENR;    /*!< RCC APB1 Peripherals sleep clock Low Word Register                       Address offset: 0xC4 */
  __IO uint32_t APB1HLPENR;    /*!< RCC APB1 Peripherals sleep clock High Word Register                      Address offset: 0xC8 */
  __IO uint32_t APB2LPENR;     /*!< RCC APB2 Peripherals sleep clock Register                                Address offset: 0xCC */
  __IO uint32_t APB3LPENR;     /*!< RCC APB3 Peripherals Clock Low Power Enable Register                     Address offset: 0xD0 */
  uint32_t      RESERVED18;    /*!< Reserved                                                                 Address offset: 0xD4 */
  __IO uint32_t CCIPR1;        /*!< RCC IPs Clocks Configuration Register 1                                  Address offset: 0xD8 */
  __IO uint32_t CCIPR2;        /*!< RCC IPs Clocks Configuration Register 2                                  Address offset: 0xDC */
  __IO uint32_t CCIPR3;        /*!< RCC IPs Clocks Configuration Register 3                                  Address offset: 0xE0 */
  __IO uint32_t CCIPR4;        /*!< RCC IPs Clocks Configuration Register 4                                  Address offset: 0xE4 */
  __IO uint32_t CCIPR5;        /*!< RCC IPs Clocks Configuration Register 5                                  Address offset: 0xE8 */
  uint32_t      RESERVED19;    /*!< Reserved,                                                                Address offset: 0xEC */
  __IO uint32_t BDCR;          /*!< RCC VSW Backup Domain & V33 Domain Control Register                      Address offset: 0xF0 */
  __IO uint32_t RSR;           /*!< RCC Reset status Register                                                Address offset: 0xF4 */
  uint32_t      RESERVED20[6]; /*!< Reserved                                                                 Address offset: 0xF8 */
  __IO uint32_t SECCFGR;       /*!< RCC Secure mode configuration register                                   Address offset: 0x110 */
  __IO uint32_t PRIVCFGR;      /*!< RCC Privilege configuration register                                     Address offset: 0x114 */
