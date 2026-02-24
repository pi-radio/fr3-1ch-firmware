  __IO uint32_t CR;             /*!< DCACHE control register,               Address offset: 0x00 */
  __IO uint32_t SR;             /*!< DCACHE status register,                Address offset: 0x04 */
  __IO uint32_t IER;            /*!< DCACHE interrupt enable register,      Address offset: 0x08 */
  __IO uint32_t FCR;            /*!< DCACHE Flag clear register,            Address offset: 0x0C */
  __IO uint32_t RHMONR;         /*!< DCACHE Read hit monitor register,      Address offset: 0x10 */
  __IO uint32_t RMMONR;         /*!< DCACHE Read miss monitor register,     Address offset: 0x14 */
       uint32_t RESERVED1[2];   /*!< Reserved,                              Address offset: 0x18-0x1C */
  __IO uint32_t WHMONR;         /*!< DCACHE Write hit monitor register,     Address offset: 0x20 */
  __IO uint32_t WMMONR;         /*!< DCACHE Write miss monitor register,    Address offset: 0x24 */
  __IO uint32_t CMDRSADDRR;     /*!< DCACHE Command Start Address register, Address offset: 0x28 */
  __IO uint32_t CMDREADDRR;     /*!< DCACHE Command End Address register,   Address offset: 0x2C */
