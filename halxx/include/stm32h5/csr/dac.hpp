

  __IO uint32_t CR;       /*!< DAC control register,                                    Address offset: 0x00 */
  __IO uint32_t SWTRIGR;  /*!< DAC software trigger register,                           Address offset: 0x04 */
  __IO uint32_t DHR12R1;  /*!< DAC channel1 12-bit right-aligned data holding register, Address offset: 0x08 */
  __IO uint32_t DHR12L1;  /*!< DAC channel1 12-bit left aligned data holding register,  Address offset: 0x0C */
  __IO uint32_t DHR8R1;   /*!< DAC channel1 8-bit right aligned data holding register,  Address offset: 0x10 */
  __IO uint32_t DHR12R2;  /*!< DAC channel2 12-bit right aligned data holding register, Address offset: 0x14 */
  __IO uint32_t DHR12L2;  /*!< DAC channel2 12-bit left aligned data holding register,  Address offset: 0x18 */
  __IO uint32_t DHR8R2;   /*!< DAC channel2 8-bit right-aligned data holding register,  Address offset: 0x1C */
  __IO uint32_t DHR12RD;  /*!< Dual DAC 12-bit right-aligned data holding register,     Address offset: 0x20 */
  __IO uint32_t DHR12LD;  /*!< DUAL DAC 12-bit left aligned data holding register,      Address offset: 0x24 */
  __IO uint32_t DHR8RD;   /*!< DUAL DAC 8-bit right aligned data holding register,      Address offset: 0x28 */
  __IO uint32_t DOR1;     /*!< DAC channel1 data output register,                       Address offset: 0x2C */
  __IO uint32_t DOR2;     /*!< DAC channel2 data output register,                       Address offset: 0x30 */
  __IO uint32_t SR;       /*!< DAC status register,                                     Address offset: 0x34 */
  __IO uint32_t CCR;      /*!< DAC calibration control register,                        Address offset: 0x38 */
  __IO uint32_t MCR;      /*!< DAC mode control register,                               Address offset: 0x3C */
  __IO uint32_t SHSR1;    /*!< DAC Sample and Hold sample time register 1,              Address offset: 0x40 */
  __IO uint32_t SHSR2;    /*!< DAC Sample and Hold sample time register 2,              Address offset: 0x44 */
  __IO uint32_t SHHR;     /*!< DAC Sample and Hold hold time register,                  Address offset: 0x48 */
  __IO uint32_t SHRR;     /*!< DAC Sample and Hold refresh time register,               Address offset: 0x4C */
  __IO uint32_t RESERVED[1];
  __IO uint32_t AUTOCR;      /*!< DAC Autonomous mode register,                         Address offset: 0x54 */
