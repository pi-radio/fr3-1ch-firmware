  __IO uint32_t CR;       /*!< Control Register,                  Address offset: 0x00 */
  __IO uint32_t IER;      /*!< Interrupt Enable Register,         Address offset: 0x04 */
  __IO uint32_t ISR;      /*!< Interrupt Status Register,         Address offset: 0x08 */
  __IO uint32_t SEAR;     /*!< ECC Single Error Address Register, Address offset: 0x0C */
  __IO uint32_t DEAR;     /*!< ECC Double Error Address Register, Address offset: 0x10 */
  __IO uint32_t ICR;      /*!< Interrupt Clear Register,          Address offset: 0x14 */
  __IO uint32_t WPR1;     /*!< SRAM Write Protection Register 1,  Address offset: 0x18 */
  __IO uint32_t WPR2;     /*!< SRAM Write Protection Register 2,  Address offset: 0x1C */
  uint32_t      RESERVED; /*!< Reserved,                          Address offset: 0x20 */
  __IO uint32_t ECCKEY;   /*!< SRAM ECC Key Register,             Address offset: 0x24 */
  __IO uint32_t ERKEYR;   /*!< SRAM Erase Key Register,           Address offset: 0x28 */
