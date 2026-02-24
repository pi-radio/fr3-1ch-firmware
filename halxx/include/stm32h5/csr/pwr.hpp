  __IO uint32_t PMCR;         /*!< Power mode control register ,          Address offset: 0x00      */
  __IO uint32_t PMSR;         /*!< Power mode status register ,           Address offset: 0x04      */
       uint32_t RESERVED1[2]; /*!< Reserved,                              Address offset: 0x08-0x0C */
  __IO uint32_t VOSCR;        /*!< Voltage scaling control register ,     Address offset: 0x10      */
  __IO uint32_t VOSSR;        /*!< Voltage sacling status register ,      Address offset: 0x14      */
       uint32_t RESERVED2[2]; /*!< Reserved,                              Address offset: 0x18-0x1C */
  __IO uint32_t BDCR;         /*!< BacKup domain control register ,       Address offset: 0x20      */
  __IO uint32_t DBPCR;        /*!< DBP control register,                  Address offset: 0x24      */
  __IO uint32_t BDSR;         /*!< BacKup domain status register,         Address offset: 0x28      */
  __IO uint32_t UCPDR;        /*!< Usb typeC and Power Delivery Register, Address offset: 0x2C      */
  __IO uint32_t SCCR;         /*!< Supply configuration control register, Address offset: 0x30      */
  __IO uint32_t VMCR;         /*!< Voltage Monitor Control Register,      Address offset: 0x34      */
  __IO uint32_t USBSCR;       /*!< USB Supply Control Register            Address offset: 0x38      */
  __IO uint32_t VMSR;         /*!< Status Register Voltage Monitoring,    Address offset: 0x3C      */
  __IO uint32_t WUSCR;        /*!< WakeUP status clear register,          Address offset: 0x40      */
  __IO uint32_t WUSR;         /*!< WakeUP status Register,                Address offset: 0x44      */
  __IO uint32_t WUCR;         /*!< WakeUP configuration register,         Address offset: 0x48      */
       uint32_t RESERVED3;    /*!< Reserved,                              Address offset: 0x4C      */
  __IO uint32_t IORETR;       /*!< IO RETention Register,                 Address offset: 0x50      */
       uint32_t RESERVED4[43];/*!< Reserved,                              Address offset: 0x54-0xFC */
  __IO uint32_t SECCFGR;      /*!< Security configuration register,       Address offset: 0x100     */
  __IO uint32_t PRIVCFGR;     /*!< Privilege configuration register,     Address offset: 0x104      */
