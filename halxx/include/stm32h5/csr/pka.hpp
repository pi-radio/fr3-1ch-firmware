{
  __IO uint32_t CR;            /*!< PKA control register,             Address offset: 0x00 */
  __IO uint32_t SR;            /*!< PKA status register,              Address offset: 0x04 */
  __IO uint32_t CLRFR;         /*!< PKA clear flag register,          Address offset: 0x08 */
  uint32_t Reserved[253];      /*!< Reserved memory area              Address offset: 0x0C  -> 0x03FC */
  __IO uint32_t RAM[1334];     /*!< PKA RAM                           Address offset: 0x400 -> 0x18D4 */
