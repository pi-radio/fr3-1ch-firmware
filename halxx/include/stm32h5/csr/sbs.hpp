       uint32_t RESERVED1[4];   /*!< RESERVED1,                                       Address offset: 0x00 - 0x0C */
  __IO uint32_t HDPLCR;         /*!< SBS HDPL Control Register,                       Address offset: 0x10 */
  __IO uint32_t HDPLSR;         /*!< SBS HDPL Status Register,                        Address offset: 0x14 */
  __IO uint32_t NEXTHDPLCR;     /*!< NEXT HDPL Control Register,                      Address offset: 0x18 */
  __IO uint32_t RESERVED2;      /*!< RESERVED2,                                       Address offset: 0x1C */
  __IO uint32_t DBGCR;          /*!< SBS Debug Control Register,                      Address offset: 0x20 */
  __IO uint32_t DBGLOCKR;       /*!< SBS Debug Lock Register,                         Address offset: 0x24 */
       uint32_t RESERVED3[3];   /*!< RESERVED3,                                       Address offset: 0x28 - 0x30 */
  __IO uint32_t RSSCMDR;        /*!< SBS RSS Command Register,                        Address offset: 0x34 */
       uint32_t RESERVED4[26];  /*!< RESERVED4,                                       Address offset: 0x38 - 0x9C */
  __IO uint32_t EPOCHSELCR;     /*!< EPOCH Selection Register,                        Address offset: 0xA0 */
       uint32_t RESERVED5[7];   /*!< RESERVED5,                                       Address offset: 0xA4 - 0xBC */
  __IO uint32_t SECCFGR;        /*!< SBS Security Mode Configuration,                 Address offset: 0xC0 */
       uint32_t RESERVED6[15];  /*!< RESERVED6,                                       Address offset: 0xC4 - 0xFC */
  __IO uint32_t PMCR;           /*!< SBS Product Mode & Config Register,              Address offset: 0x100 */
  __IO uint32_t FPUIMR;         /*!< SBS FPU Interrupt Mask Register,                 Address offset: 0x104 */
  __IO uint32_t MESR;           /*!< SBS Memory Erase Status Register,                Address offset: 0x108 */
       uint32_t RESERVED7;      /*!< RESERVED7,                                       Address offset: 0x10C */
  __IO uint32_t CCCSR;          /*!< SBS Compensation Cell Control & Status Register, Address offset: 0x110 */
  __IO uint32_t CCVALR;         /*!< SBS Compensation Cell Value Register,            Address offset: 0x114 */
  __IO uint32_t CCSWCR;         /*!< SBS Compensation Cell for I/Os sw code Register, Address offset: 0x118 */
  __IO uint32_t RESERVED8;      /*!< RESERVED8,                                       Address offset: 0x11C */
  __IO uint32_t CFGR2;          /*!< SBS Class B Register,                            Address offset: 0x120 */
       uint32_t RESERVED9[8];   /*!< RESERVED9,                                       Address offset: 0x124 - 0x140 */
  __IO uint32_t CNSLCKR;        /*!< SBS CPU Non-secure Lock Register,                Address offset: 0x144 */
  __IO uint32_t CSLCKR;         /*!< SBS CPU Secure Lock Register,                    Address offset: 0x148 */
  __IO uint32_t ECCNMIR;        /*!< SBS FLITF ECC NMI MASK Register,                 Address offset: 0x14C */
