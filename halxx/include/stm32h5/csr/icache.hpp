

  __IO uint32_t CR;             /*!< ICACHE control register,                Address offset: 0x00 */
  __IO uint32_t SR;             /*!< ICACHE status register,                 Address offset: 0x04 */
  __IO uint32_t IER;            /*!< ICACHE interrupt enable register,       Address offset: 0x08 */
  __IO uint32_t FCR;            /*!< ICACHE Flag clear register,             Address offset: 0x0C */
  __IO uint32_t HMONR;          /*!< ICACHE hit monitor register,            Address offset: 0x10 */
  __IO uint32_t MMONR;          /*!< ICACHE miss monitor register,           Address offset: 0x14 */
       uint32_t RESERVED1[2];   /*!< Reserved,                               Address offset: 0x018-0x01C */
  __IO uint32_t CRR0;           /*!< ICACHE region 0 configuration register, Address offset: 0x20 */
  __IO uint32_t CRR1;           /*!< ICACHE region 1 configuration register, Address offset: 0x24 */
  __IO uint32_t CRR2;           /*!< ICACHE region 2 configuration register, Address offset: 0x28 */
  __IO uint32_t CRR3;           /*!< ICACHE region 3 configuration register, Address offset: 0x2C */
