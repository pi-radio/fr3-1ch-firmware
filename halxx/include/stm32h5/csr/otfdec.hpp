/**
  * @brief OTFDEC register
  */
typedef struct
{
  __IO uint32_t REG_CONFIGR;      /*!< OTFDEC Region Configuration register,          Address offset: 0x20 + 0x30 * (x -1) (x = 1 to 4) */
  __IO uint32_t REG_START_ADDR;   /*!< OTFDEC Region Start Address register,          Address offset: 0x24 + 0x30 * (x -1) (x = 1 to 4) */
  __IO uint32_t REG_END_ADDR;     /*!< OTFDEC Region End Address register,            Address offset: 0x28 + 0x30 * (x -1) (x = 1 to 4) */
  __IO uint32_t REG_NONCER0;      /*!< OTFDEC Region Nonce register 0,                Address offset: 0x2C + 0x30 * (x -1) (x = 1 to 4) */
  __IO uint32_t REG_NONCER1;      /*!< OTFDEC Region Nonce register 1,                Address offset: 0x30 + 0x30 * (x -1) (x = 1 to 4) */
  __IO uint32_t REG_KEYR0;        /*!< OTFDEC Region Key register 0,                  Address offset: 0x34 + 0x30 * (x -1) (x = 1 to 4) */
  __IO uint32_t REG_KEYR1;        /*!< OTFDEC Region Key register 1,                  Address offset: 0x38 + 0x30 * (x -1) (x = 1 to 4) */
  __IO uint32_t REG_KEYR2;        /*!< OTFDEC Region Key register 2,                  Address offset: 0x3C + 0x30 * (x -1) (x = 1 to 4) */
  __IO uint32_t REG_KEYR3;        /*!< OTFDEC Region Key register 3,                  Address offset: 0x40 + 0x30 * (x -1) (x = 1 to 4) */
} OTFDEC_Region_TypeDef;

typedef struct
{
  __IO uint32_t CR;               /*!< OTFDEC Control register,                                 Address offset: 0x000 */
  uint32_t RESERVED1[3];          /*!< Reserved,                                                Address offset: 0x004-0x00C */
  __IO uint32_t PRIVCFGR;         /*!< OTFDEC Privileged access control Configuration register, Address offset: 0x010 */
  uint32_t RESERVED2[187];        /*!< Reserved,                                                Address offset: 0x014-0x2FC */
  __IO uint32_t ISR;              /*!< OTFDEC Interrupt Status register,                        Address offset: 0x300 */
  __IO uint32_t ICR;              /*!< OTFDEC Interrupt Clear register,                         Address offset: 0x304 */
  __IO uint32_t IER;              /*!< OTFDEC Interrupt Enable register,                        Address offset: 0x308 */
} OTFDEC_TypeDef;
