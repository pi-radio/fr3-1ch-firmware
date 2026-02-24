

  __IO uint32_t RTSR1;          /*!< EXTI Rising Trigger Selection Register 1,        Address offset:   0x00 */
  __IO uint32_t FTSR1;          /*!< EXTI Falling Trigger Selection Register 1,       Address offset:   0x04 */
  __IO uint32_t SWIER1;         /*!< EXTI Software Interrupt event Register 1,        Address offset:   0x08 */
  __IO uint32_t RPR1;           /*!< EXTI Rising Pending Register 1,                  Address offset:   0x0C */
  __IO uint32_t FPR1;           /*!< EXTI Falling Pending Register 1,                 Address offset:   0x10 */
  __IO uint32_t SECCFGR1;       /*!< EXTI Security Configuration Register 1,          Address offset:   0x14 */
  __IO uint32_t PRIVCFGR1;      /*!< EXTI Privilege Configuration Register 1,         Address offset:   0x18 */
       uint32_t RESERVED1;      /*!< Reserved 1,                                      Address offset:   0x1C */
  __IO uint32_t RTSR2;          /*!< EXTI Rising Trigger Selection Register 2,        Address offset:   0x20 */
  __IO uint32_t FTSR2;          /*!< EXTI Falling Trigger Selection Register 2,       Address offset:   0x24 */
  __IO uint32_t SWIER2;         /*!< EXTI Software Interrupt event Register 2,        Address offset:   0x28 */
  __IO uint32_t RPR2;           /*!< EXTI Rising Pending Register 2,                  Address offset:   0x2C */
  __IO uint32_t FPR2;           /*!< EXTI Falling Pending Register 2,                 Address offset:   0x30 */
  __IO uint32_t SECCFGR2;       /*!< EXTI Security Configuration Register 2,          Address offset:   0x34 */
  __IO uint32_t PRIVCFGR2;      /*!< EXTI Privilege Configuration Register 2,         Address offset:   0x38 */
       uint32_t RESERVED2[9];   /*!< Reserved 2,                                                 0x3C-- 0x5C */
  __IO uint32_t EXTICR[4];      /*!< EXIT External Interrupt Configuration Register,            0x60 -- 0x6C */
  __IO uint32_t LOCKR;          /*!< EXTI Lock Register,                              Address offset:   0x70 */
       uint32_t RESERVED3[3];   /*!< Reserved 3,                                                0x74 -- 0x7C */
  __IO uint32_t IMR1;           /*!< EXTI Interrupt Mask Register 1,                  Address offset:   0x80 */
  __IO uint32_t EMR1;           /*!< EXTI Event Mask Register 1,                      Address offset:   0x84 */
       uint32_t RESERVED4[2];   /*!< Reserved 4,                                                0x88 -- 0x8C */
  __IO uint32_t IMR2;           /*!< EXTI Interrupt Mask Register 2,                  Address offset:   0x90 */
  __IO uint32_t EMR2;           /*!< EXTI Event Mask Register 2,                      Address offset:   0x94 */
