typedef struct
{
  __IO uint32_t CR;          /*!< OCTOSPI Control register,                            Address offset: 0x000 */
  uint32_t RESERVED;         /*!< Reserved,                                            Address offset: 0x004 */
  __IO uint32_t DCR1;        /*!< OCTOSPI Device Configuration register 1,             Address offset: 0x008 */
  __IO uint32_t DCR2;        /*!< OCTOSPI Device Configuration register 2,             Address offset: 0x00C */
  __IO uint32_t DCR3;        /*!< OCTOSPI Device Configuration register 3,             Address offset: 0x010 */
  __IO uint32_t DCR4;        /*!< OCTOSPI Device Configuration register 4,             Address offset: 0x014 */
  uint32_t RESERVED1[2];     /*!< Reserved,                                            Address offset: 0x018-0x01C */
  __IO uint32_t SR;          /*!< OCTOSPI Status register,                             Address offset: 0x020 */
  __IO uint32_t FCR;         /*!< OCTOSPI Flag Clear register,                         Address offset: 0x024 */
  uint32_t RESERVED2[6];     /*!< Reserved,                                            Address offset: 0x028-0x03C */
  __IO uint32_t DLR;         /*!< OCTOSPI Data Length register,                        Address offset: 0x040 */
  uint32_t RESERVED3;        /*!< Reserved,                                            Address offset: 0x044 */
  __IO uint32_t AR;          /*!< OCTOSPI Address register,                            Address offset: 0x048 */
  uint32_t RESERVED4;        /*!< Reserved,                                            Address offset: 0x04C */
  __IO uint32_t DR;          /*!< OCTOSPI Data register,                               Address offset: 0x050 */
  uint32_t RESERVED5[11];    /*!< Reserved,                                            Address offset: 0x054-0x07C */
  __IO uint32_t PSMKR;       /*!< OCTOSPI Polling Status Mask register,                Address offset: 0x080 */
  uint32_t RESERVED6;        /*!< Reserved,                                            Address offset: 0x084 */
  __IO uint32_t PSMAR;       /*!< OCTOSPI Polling Status Match register,               Address offset: 0x088 */
  uint32_t RESERVED7;        /*!< Reserved,                                            Address offset: 0x08C */
  __IO uint32_t PIR;         /*!< OCTOSPI Polling Interval register,                   Address offset: 0x090 */
  uint32_t RESERVED8[27];    /*!< Reserved,                                            Address offset: 0x094-0x0FC */
  __IO uint32_t CCR;         /*!< OCTOSPI Communication Configuration register,        Address offset: 0x100 */
  uint32_t RESERVED9;        /*!< Reserved,                                            Address offset: 0x104 */
  __IO uint32_t TCR;         /*!< OCTOSPI Timing Configuration register,               Address offset: 0x108 */
  uint32_t RESERVED10;       /*!< Reserved,                                            Address offset: 0x10C */
  __IO uint32_t IR;          /*!< OCTOSPI Instruction register,                        Address offset: 0x110 */
  uint32_t RESERVED11[3];    /*!< Reserved,                                            Address offset: 0x114-0x11C */
  __IO uint32_t ABR;         /*!< OCTOSPI Alternate Bytes register,                    Address offset: 0x120 */
  uint32_t RESERVED12[3];    /*!< Reserved,                                            Address offset: 0x124-0x12C */
  __IO uint32_t LPTR;        /*!< OCTOSPI Low Power Timeout register,                  Address offset: 0x130 */
  uint32_t RESERVED13[3];    /*!< Reserved,                                            Address offset: 0x134-0x13C */
  __IO uint32_t WPCCR;       /*!< OCTOSPI Wrap Communication Configuration register,   Address offset: 0x140 */
  uint32_t RESERVED14;       /*!< Reserved,                                            Address offset: 0x144 */
  __IO uint32_t WPTCR;       /*!< OCTOSPI Wrap Timing Configuration register,          Address offset: 0x148 */
  uint32_t RESERVED15;       /*!< Reserved,                                            Address offset: 0x14C */
  __IO uint32_t WPIR;        /*!< OCTOSPI Wrap Instruction register,                   Address offset: 0x150 */
  uint32_t RESERVED16[3];    /*!< Reserved,                                            Address offset: 0x154-0x15C */
  __IO uint32_t WPABR;       /*!< OCTOSPI Wrap Alternate Bytes register,               Address offset: 0x160 */
  uint32_t RESERVED17[7];    /*!< Reserved,                                            Address offset: 0x164-0x17C */
  __IO uint32_t WCCR;        /*!< OCTOSPI Write Communication Configuration register,  Address offset: 0x180 */
  uint32_t RESERVED18;       /*!< Reserved,                                            Address offset: 0x184 */
  __IO uint32_t WTCR;        /*!< OCTOSPI Write Timing Configuration register,         Address offset: 0x188 */
  uint32_t RESERVED19;       /*!< Reserved,                                            Address offset: 0x18C */
  __IO uint32_t WIR;         /*!< OCTOSPI Write Instruction register,                  Address offset: 0x190 */
  uint32_t RESERVED20[3];    /*!< Reserved,                                            Address offset: 0x194-0x19C */
  __IO uint32_t WABR;        /*!< OCTOSPI Write Alternate Bytes register,              Address offset: 0x1A0 */
  uint32_t RESERVED21[23];   /*!< Reserved,                                            Address offset: 0x1A4-0x1FC */
  __IO uint32_t HLCR;        /*!< OCTOSPI Hyperbus Latency Configuration register,     Address offset: 0x200 */
} XSPI_TypeDef;

typedef  XSPI_TypeDef OCTOSPI_TypeDef;
