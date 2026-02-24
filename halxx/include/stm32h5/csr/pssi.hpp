  __IO uint32_t CR;             /*!< PSSI control register,                 Address offset: 0x000 */
  __IO uint32_t SR;             /*!< PSSI status register,                  Address offset: 0x004 */
  __IO uint32_t RIS;            /*!< PSSI raw interrupt status register,    Address offset: 0x008 */
  __IO uint32_t IER;            /*!< PSSI interrupt enable register,        Address offset: 0x00C */
  __IO uint32_t MIS;            /*!< PSSI masked interrupt status register, Address offset: 0x010 */
  __IO uint32_t ICR;            /*!< PSSI interrupt clear register,         Address offset: 0x014 */
  __IO uint32_t RESERVED1[4];   /*!< Reserved,                                      0x018 - 0x024 */
  __IO uint32_t DR;             /*!< PSSI data register,                    Address offset: 0x028 */
