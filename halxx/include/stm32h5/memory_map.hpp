#pragma once

#include <stm32h5/csr/can.hpp>
#include <stm32h5/csr/cec.hpp>
#include <stm32h5/csr/crs.hpp>
#include <stm32h5/csr/dma.hpp>
#include <stm32h5/csr/dts.hpp>
#include <stm32h5/csr/i2c.hpp>
#include <stm32h5/csr/i3c.hpp>
#include <stm32h5/csr/iwdg.hpp>
#include <stm32h5/csr/sai.hpp>
#include <stm32h5/csr/spi.hpp>
#include <stm32h5/csr/timer.hpp>
#include <stm32h5/csr/ucpd.hpp>
#include <stm32h5/csr/usart.hpp>
#include <stm32h5/csr/usb.hpp>
#include <stm32h5/csr/wwdg.hpp>

#define SRAM1_SIZE               (0x40000UL)    /*!< SRAM1=256k */
#define SRAM2_SIZE               (0x10000UL)    /*!< SRAM2=64k  */
#define SRAM3_SIZE               (0x50000UL)    /*!< SRAM3=320k */
#define BKPSRAM_SIZE             (0x01000UL)    /*!< BKPSRAM=4k */

/* Flash, Peripheral and internal SRAMs base addresses - Non secure */
#define FLASH_BASE_NS            (0x08000000UL) /*!< FLASH (up to 2 MB) non-secure base address         */
#define SRAM1_BASE_NS            (0x20000000UL) /*!< SRAM1 (256 KB) non-secure base address             */
#define SRAM2_BASE_NS            (0x20040000UL) /*!< SRAM2 (64 KB) non-secure base address              */
#define SRAM3_BASE_NS            (0x20050000UL) /*!< SRAM3 (320 KB) non-secure base address             */
#define PERIPH_BASE_NS           (0x40000000UL) /*!< Peripheral non-secure base address                 */

/* External memories base addresses - Not aliased */
#define FMC_BASE                 (0x60000000UL) /*!< FMC base address                                   */
#define OCTOSPI1_BASE            (0x90000000UL) /*!< OCTOSPI1 memories accessible over AHB base address */

#define FMC_BANK1                FMC_BASE
#define FMC_BANK1_1              FMC_BANK1
#define FMC_BANK1_2              (FMC_BANK1 + 0x04000000UL)  /*!< FMC Memory Bank1 for SRAM, NOR and PSRAM */
#define FMC_BANK1_3              (FMC_BANK1 + 0x08000000UL)
#define FMC_BANK1_4              (FMC_BANK1 + 0x0C000000UL)
#define FMC_BANK3                (FMC_BASE  + 0x20000000UL)  /*!< FMC Memory Bank3 for NAND */
#define FMC_SDRAM_BANK_1         (FMC_BASE +  0x60000000UL)  /*!< FMC Memory SDRAM Bank1 */
#define FMC_SDRAM_BANK_2         (FMC_BASE +  0x70000000UL)  /*!< FMC Memory SDRAM Bank2 */


/* Peripheral memory map - Non secure */
#define APB1PERIPH_BASE_NS       PERIPH_BASE_NS
#define APB2PERIPH_BASE_NS       (PERIPH_BASE_NS + 0x00010000UL)
#define AHB1PERIPH_BASE_NS       (PERIPH_BASE_NS + 0x00020000UL)
#define AHB2PERIPH_BASE_NS       (PERIPH_BASE_NS + 0x02020000UL)
#define APB3PERIPH_BASE_NS       (PERIPH_BASE_NS + 0x04000000UL)
#define AHB3PERIPH_BASE_NS       (PERIPH_BASE_NS + 0x04020000UL)
#define AHB4PERIPH_BASE_NS       (PERIPH_BASE_NS + 0x06000000UL)


namespace CSR
{
  namespace bus {
    template <uint32_t base>
    struct apb1
    {
      CSR::csr_ptr<CSR::timer, base + 0x0000> tim2;
      CSR::csr_ptr<CSR::timer, base + 0x0400> tim3;
      CSR::csr_ptr<CSR::timer, base + 0x0800> tim4;
      CSR::csr_ptr<CSR::timer, base + 0x0C00> tim5;
      CSR::csr_ptr<CSR::timer, base + 0x1000> tim6;
      CSR::csr_ptr<CSR::timer, base + 0x1400> tim7;
      CSR::csr_ptr<CSR::timer, base + 0x1800> tim12;
      CSR::csr_ptr<CSR::timer, base + 0x1C00> tim13;
      CSR::csr_ptr<CSR::timer, base + 0x2000> tim14;
      CSR::csr_ptr<CSR::wwdg, base + 0x2C00> wwdg;
      CSR::csr_ptr<CSR::iwdg, base + 0x3000> iwdg;
      CSR::csr_ptr<CSR::spi, base + 0x3800> spi2;
      CSR::csr_ptr<CSR::spi, base + 0x3C00> spi3;
      CSR::csr_ptr<CSR::usart, base + 0x4400> usart2;
      CSR::csr_ptr<CSR::usart, base + 0x4800> usart3;
      CSR::csr_ptr<CSR::usart, base + 0x4C00> uart4;
      CSR::csr_ptr<CSR::usart, base + 0x5000> uart5;
      CSR::csr_ptr<CSR::i2c, base + 0x5400> i2c1;
      CSR::csr_ptr<CSR::i2c, base + 0x5800> i2c2;
      CSR::csr_ptr<CSR::i3c, base + 0x5C00> i3c1;
      CSR::csr_ptr<CSR::crs, base + 0x6000> crs;
      CSR::csr_ptr<CSR::usart, base + 0x6400> usart6;
      CSR::csr_ptr<CSR::usart, base + 0x6800> usart10;
      CSR::csr_ptr<CSR::usart, base + 0x6C00> usart11;
      CSR::csr_ptr<CSR::cec, base + 0x7000> cec;
      CSR::csr_ptr<CSR::usart, base + 0x7800> uart7;
      CSR::csr_ptr<CSR::usart, base + 0x7C00> uart8;
      CSR::csr_ptr<CSR::usart, base + 0x8000> uart9;
      CSR::csr_ptr<CSR::usart, base + 0x8400> uart12;
      CSR::csr_ptr<CSR::dts, base + 0x8C00> dts;
      CSR::csr_ptr<CSR::lptimer, base + 0x9400> lptim2;
      CSR::csr_ptr<CSR::fdcan, base + 0xA400> fdcan1;
      CSR::csr_ptr<CSR::fdcan_config, base + 0xA500> fdcan_config;
      CSR::csr_ptr<CSR::fdcan_sram, base + 0xAC00> fdcan_sram;
      CSR::csr_ptr<CSR::fdcan, base + 0xA800> fdcan2;
      CSR::csr_ptr<CSR::ucpd, base + 0xDC00> ucpd;
    };

    template <uint32_t base>
    struct apb2
    {
      CSR::csr_ptr<CSR::timer, base + 0x2C00> tim1;
      CSR::csr_ptr<CSR::spi, base + 0x3000> spi1;
      CSR::csr_ptr<CSR::timer, base + 0x3400> tim8;
      CSR::csr_ptr<CSR::usart, base + 0x3800> usart1;
      CSR::csr_ptr<CSR::timer, base + 0x4000> tim15;
      CSR::csr_ptr<CSR::timer, base + 0x4400> tim16;
      CSR::csr_ptr<CSR::timer, base + 0x4800> tim17;
      CSR::csr_ptr<CSR::spi, base + 0x4C00> spi4;
      CSR::csr_ptr<CSR::spi, base + 0x5000> spi6;
      CSR::csr_ptr<CSR::sai, base + 0x5400> sai1;
      CSR::csr_ptr<CSR::sai_block, base + 0x5404> sai1_block_A;
      CSR::csr_ptr<CSR::sai_block, base + 0x5424> sai1_block_B;
      CSR::csr_ptr<CSR::sai, base + 0x5800> sai2;
      CSR::csr_ptr<CSR::sai_block, base + 0x5804> sai2_block_A;
      CSR::csr_ptr<CSR::sai_block, base + 0x5824> sai2_block_B;
      CSR::csr_ptr<CSR::usb_drd, base + 0x6000> usb;
      CSR::csr_ptr<CSR::usb_drd_pma_buf, base + 0x6400> usb_pma;
    };

    template <uint32_t base>
    struct ahb1
    {
      dmac<base> gpdma1;
      dmac<base + 0x1000> gpdma2;
    };

    template <uint32_t base>
    struct ahb2
    {
    };
  };

  template <uint32_t base>
  struct peripherals
  {
    constexpr static struct peripherals *p = (struct peripherals *)base;

    bus::apb1<base + 0> apb1;
    bus::apb2<base + 0x00010000> apb2;
    bus::ahb1<base + 0x00020000> ahb1;
    bus::ahb2<base + 0x02020000> ahb2;
  };

  peripherals<0x40000000> periph_nsec;
  peripherals<0x50000000> periph_sec;



#if 0


/*!< AHB1 Non secure peripherals */
#define GPDMA1_BASE_NS           AHB1PERIPH_BASE_NS
#define GPDMA2_BASE_NS           (AHB1PERIPH_BASE_NS + 0x01000UL)
#define FLASH_R_BASE_NS          (AHB1PERIPH_BASE_NS + 0x02000UL)
#define CRC_BASE_NS              (AHB1PERIPH_BASE_NS + 0x03000UL)
#define CORDIC_BASE_NS           (AHB1PERIPH_BASE_NS + 0x03800UL)
#define FMAC_BASE_NS             (AHB1PERIPH_BASE_NS + 0x03C00UL)
#define RAMCFG_BASE_NS           (AHB1PERIPH_BASE_NS + 0x06000UL)
#define ETH_BASE_NS              (AHB1PERIPH_BASE_NS + 0x8000UL)
#define ETH_MAC_BASE_NS          (ETH_BASE)
#define ICACHE_BASE_NS           (AHB1PERIPH_BASE_NS + 0x10400UL)
#define DCACHE1_BASE_NS          (AHB1PERIPH_BASE_NS + 0x11400UL)
#define GTZC_TZSC1_BASE_NS       (AHB1PERIPH_BASE_NS + 0x12400UL)
#define GTZC_TZIC1_BASE_NS       (AHB1PERIPH_BASE_NS + 0x12800UL)
#define GTZC_MPCBB1_BASE_NS      (AHB1PERIPH_BASE_NS + 0x12C00UL)
#define GTZC_MPCBB2_BASE_NS      (AHB1PERIPH_BASE_NS + 0x13000UL)
#define GTZC_MPCBB3_BASE_NS      (AHB1PERIPH_BASE_NS + 0x13400UL)
#define BKPSRAM_BASE_NS          (AHB1PERIPH_BASE_NS + 0x16400UL)

#define GPDMA1_Channel0_BASE_NS   (GPDMA1_BASE_NS + 0x0050UL)
#define GPDMA1_Channel1_BASE_NS   (GPDMA1_BASE_NS + 0x00D0UL)
#define GPDMA1_Channel2_BASE_NS   (GPDMA1_BASE_NS + 0x0150UL)
#define GPDMA1_Channel3_BASE_NS   (GPDMA1_BASE_NS + 0x01D0UL)
#define GPDMA1_Channel4_BASE_NS   (GPDMA1_BASE_NS + 0x0250UL)
#define GPDMA1_Channel5_BASE_NS   (GPDMA1_BASE_NS + 0x02D0UL)
#define GPDMA1_Channel6_BASE_NS   (GPDMA1_BASE_NS + 0x0350UL)
#define GPDMA1_Channel7_BASE_NS   (GPDMA1_BASE_NS + 0x03D0UL)
#define GPDMA2_Channel0_BASE_NS   (GPDMA2_BASE_NS + 0x0050UL)
#define GPDMA2_Channel1_BASE_NS   (GPDMA2_BASE_NS + 0x00D0UL)
#define GPDMA2_Channel2_BASE_NS   (GPDMA2_BASE_NS + 0x0150UL)
#define GPDMA2_Channel3_BASE_NS   (GPDMA2_BASE_NS + 0x01D0UL)
#define GPDMA2_Channel4_BASE_NS   (GPDMA2_BASE_NS + 0x0250UL)
#define GPDMA2_Channel5_BASE_NS   (GPDMA2_BASE_NS + 0x02D0UL)
#define GPDMA2_Channel6_BASE_NS   (GPDMA2_BASE_NS + 0x0350UL)
#define GPDMA2_Channel7_BASE_NS   (GPDMA2_BASE_NS + 0x03D0UL)

#define RAMCFG_SRAM1_BASE_NS     (RAMCFG_BASE_NS)
#define RAMCFG_SRAM2_BASE_NS     (RAMCFG_BASE_NS + 0x0040UL)
#define RAMCFG_SRAM3_BASE_NS     (RAMCFG_BASE_NS + 0x0080UL)
#define RAMCFG_BKPRAM_BASE_NS    (RAMCFG_BASE_NS + 0x0100UL)

/*!< AHB2 Non secure peripherals */
#define GPIOA_BASE_NS            (AHB2PERIPH_BASE_NS + 0x00000UL)
#define GPIOB_BASE_NS            (AHB2PERIPH_BASE_NS + 0x00400UL)
#define GPIOC_BASE_NS            (AHB2PERIPH_BASE_NS + 0x00800UL)
#define GPIOD_BASE_NS            (AHB2PERIPH_BASE_NS + 0x00C00UL)
#define GPIOE_BASE_NS            (AHB2PERIPH_BASE_NS + 0x01000UL)
#define GPIOF_BASE_NS            (AHB2PERIPH_BASE_NS + 0x01400UL)
#define GPIOG_BASE_NS            (AHB2PERIPH_BASE_NS + 0x01800UL)
#define GPIOH_BASE_NS            (AHB2PERIPH_BASE_NS + 0x01C00UL)
#define GPIOI_BASE_NS            (AHB2PERIPH_BASE_NS + 0x02000UL)
#define ADC1_BASE_NS             (AHB2PERIPH_BASE_NS + 0x08000UL)
#define ADC2_BASE_NS             (AHB2PERIPH_BASE_NS + 0x08100UL)
#define ADC12_COMMON_BASE_NS     (AHB2PERIPH_BASE_NS + 0x08300UL)
#define DAC1_BASE_NS             (AHB2PERIPH_BASE_NS + 0x08400UL)
#define DCMI_BASE_NS             (AHB2PERIPH_BASE_NS + 0x0C000UL)
#define PSSI_BASE_NS             (AHB2PERIPH_BASE_NS + 0x0C400UL)
#define AES_BASE_NS              (AHB2PERIPH_BASE_NS + 0xA0000UL)
#define HASH_BASE_NS             (AHB2PERIPH_BASE_NS + 0xA0400UL)
#define HASH_DIGEST_BASE_NS      (AHB2PERIPH_BASE_NS + 0xA0710UL)
#define RNG_BASE_NS              (AHB2PERIPH_BASE_NS + 0xA0800UL)
#define SAES_BASE_NS             (AHB2PERIPH_BASE_NS + 0xA0C00UL)
#define PKA_BASE_NS              (AHB2PERIPH_BASE_NS + 0xA2000UL)
#define PKA_RAM_BASE_NS          (AHB2PERIPH_BASE_NS + 0xA2400UL)

/*!< APB3 Non secure peripherals */
#define SBS_BASE_NS              (APB3PERIPH_BASE_NS + 0x0400UL)
#define SPI5_BASE_NS             (APB3PERIPH_BASE_NS + 0x2000UL)
#define LPUART1_BASE_NS          (APB3PERIPH_BASE_NS + 0x2400UL)
#define I2C3_BASE_NS             (APB3PERIPH_BASE_NS + 0x2800UL)
#define I2C4_BASE_NS             (APB3PERIPH_BASE_NS + 0x2C00UL)
#define LPTIM1_BASE_NS           (APB3PERIPH_BASE_NS + 0x4400UL)
#define LPTIM3_BASE_NS           (APB3PERIPH_BASE_NS + 0x4800UL)
#define LPTIM4_BASE_NS           (APB3PERIPH_BASE_NS + 0x4C00UL)
#define LPTIM5_BASE_NS           (APB3PERIPH_BASE_NS + 0x5000UL)
#define LPTIM6_BASE_NS           (APB3PERIPH_BASE_NS + 0x5400UL)
#define VREFBUF_BASE_NS          (APB3PERIPH_BASE_NS + 0x7400UL)
#define RTC_BASE_NS              (APB3PERIPH_BASE_NS + 0x7800UL)
#define TAMP_BASE_NS             (APB3PERIPH_BASE_NS + 0x7C00UL)

/*!< AHB3 Non secure peripherals */
#define PWR_BASE_NS              (AHB3PERIPH_BASE_NS + 0x0800UL)
#define RCC_BASE_NS              (AHB3PERIPH_BASE_NS + 0x0C00UL)
#define EXTI_BASE_NS             (AHB3PERIPH_BASE_NS + 0x2000UL)
#define DEBUG_BASE_NS            (AHB3PERIPH_BASE_NS + 0x4000UL)

/*!< AHB4 Non secure peripherals */
#define OTFDEC1_BASE_NS          (AHB4PERIPH_BASE_NS + 0x5000UL)
#define OTFDEC1_REGION1_BASE_NS  (OTFDEC1_BASE_NS + 0x20UL)
#define OTFDEC1_REGION2_BASE_NS  (OTFDEC1_BASE_NS + 0x50UL)
#define OTFDEC1_REGION3_BASE_NS  (OTFDEC1_BASE_NS + 0x80UL)
#define OTFDEC1_REGION4_BASE_NS  (OTFDEC1_BASE_NS + 0xB0UL)
#define SDMMC1_BASE_NS           (AHB4PERIPH_BASE_NS + 0x8000UL)
#define DLYB_SDMMC1_BASE_NS      (AHB4PERIPH_BASE_NS + 0x8400UL)
#define SDMMC2_BASE_NS           (AHB4PERIPH_BASE_NS + 0x8C00UL)
#define DLYB_SDMMC2_BASE_NS      (AHB4PERIPH_BASE_NS + 0x8800UL)
#define FMC_R_BASE_NS            (AHB4PERIPH_BASE_NS + 0x1000400UL) /*!< FMC control registers base address              */
#define OCTOSPI1_R_BASE_NS       (AHB4PERIPH_BASE_NS + 0x1001400UL) /*!< OCTOSPI1 control registers base address         */
#define DLYB_OCTOSPI1_BASE_NS    (AHB4PERIPH_BASE_NS + 0x0F000UL)

/*!< FMC Banks Non secure registers base address */
#define FMC_Bank1_R_BASE_NS      (FMC_R_BASE_NS + 0x0000UL)
#define FMC_Bank1E_R_BASE_NS     (FMC_R_BASE_NS + 0x0104UL)
#define FMC_Bank3_R_BASE_NS      (FMC_R_BASE_NS + 0x0080UL)
#define FMC_Bank5_6_R_BASE_NS    (FMC_R_BASE_NS + 0x0140UL)

/* Flash, Peripheral and internal SRAMs base addresses - Secure */
#define FLASH_BASE_S            (0x0C000000UL) /*!< FLASH (up to 2 MB) secure base address */
#define SRAM1_BASE_S            (0x30000000UL) /*!< SRAM1 (256 KB) secure base address     */
#define SRAM2_BASE_S            (0x30040000UL) /*!< SRAM2 (64 KB) secure base address      */
#define SRAM3_BASE_S            (0x30050000UL) /*!< SRAM3 (320 KB) secure base address     */
#define PERIPH_BASE_S           (0x50000000UL) /*!< Peripheral secure base address         */

/* Peripheral memory map - Secure */
#define APB1PERIPH_BASE_S       PERIPH_BASE_S
#define APB2PERIPH_BASE_S       (PERIPH_BASE_S + 0x00010000UL)
#define AHB1PERIPH_BASE_S       (PERIPH_BASE_S + 0x00020000UL)
#define AHB2PERIPH_BASE_S       (PERIPH_BASE_S + 0x02020000UL)
#define APB3PERIPH_BASE_S       (PERIPH_BASE_S + 0x04000000UL)
#define AHB3PERIPH_BASE_S       (PERIPH_BASE_S + 0x04020000UL)
#define AHB4PERIPH_BASE_S       (PERIPH_BASE_S + 0x06000000UL)

/*!< APB1 secure peripherals */
#define TIM2_BASE_S             (APB1PERIPH_BASE_S + 0x0000UL)
#define TIM3_BASE_S             (APB1PERIPH_BASE_S + 0x0400UL)
#define TIM4_BASE_S             (APB1PERIPH_BASE_S + 0x0800UL)
#define TIM5_BASE_S             (APB1PERIPH_BASE_S + 0x0C00UL)
#define TIM6_BASE_S             (APB1PERIPH_BASE_S + 0x1000UL)
#define TIM7_BASE_S             (APB1PERIPH_BASE_S + 0x1400UL)
#define TIM12_BASE_S            (APB1PERIPH_BASE_S + 0x1800UL)
#define TIM13_BASE_S            (APB1PERIPH_BASE_S + 0x1C00UL)
#define TIM14_BASE_S            (APB1PERIPH_BASE_S + 0x2000UL)
#define WWDG_BASE_S             (APB1PERIPH_BASE_S + 0x2C00UL)
#define IWDG_BASE_S             (APB1PERIPH_BASE_S + 0x3000UL)
#define SPI2_BASE_S             (APB1PERIPH_BASE_S + 0x3800UL)
#define SPI3_BASE_S             (APB1PERIPH_BASE_S + 0x3C00UL)
#define USART2_BASE_S           (APB1PERIPH_BASE_S + 0x4400UL)
#define USART3_BASE_S           (APB1PERIPH_BASE_S + 0x4800UL)
#define UART4_BASE_S            (APB1PERIPH_BASE_S + 0x4C00UL)
#define UART5_BASE_S            (APB1PERIPH_BASE_S + 0x5000UL)
#define I2C1_BASE_S             (APB1PERIPH_BASE_S + 0x5400UL)
#define I2C2_BASE_S             (APB1PERIPH_BASE_S + 0x5800UL)
#define I3C1_BASE_S             (APB1PERIPH_BASE_S + 0x5C00UL)
#define CRS_BASE_S              (APB1PERIPH_BASE_S + 0x6000UL)
#define USART6_BASE_S           (APB1PERIPH_BASE_S + 0x6400UL)
#define USART10_BASE_S          (APB1PERIPH_BASE_S + 0x6800UL)
#define USART11_BASE_S          (APB1PERIPH_BASE_S + 0x6C00UL)
#define CEC_BASE_S              (APB1PERIPH_BASE_S + 0x7000UL)
#define UART7_BASE_S            (APB1PERIPH_BASE_S + 0x7800UL)
#define UART8_BASE_S            (APB1PERIPH_BASE_S + 0x7C00UL)
#define UART9_BASE_S            (APB1PERIPH_BASE_S + 0x8000UL)
#define UART12_BASE_S           (APB1PERIPH_BASE_S + 0x8400UL)
#define DTS_BASE_S              (APB1PERIPH_BASE_S + 0x8C00UL)
#define LPTIM2_BASE_S           (APB1PERIPH_BASE_S + 0x9400UL)
#define FDCAN1_BASE_S           (APB1PERIPH_BASE_S + 0xA400UL)
#define FDCAN_CONFIG_BASE_S     (APB1PERIPH_BASE_S + 0xA500UL)
#define SRAMCAN_BASE_S          (APB1PERIPH_BASE_S + 0xAC00UL)
#define FDCAN2_BASE_S           (APB1PERIPH_BASE_S + 0xA800UL)
#define UCPD1_BASE_S            (APB1PERIPH_BASE_S + 0xDC00UL)

/*!< APB2 Secure peripherals */
#define TIM1_BASE_S             (APB2PERIPH_BASE_S + 0x2C00UL)
#define SPI1_BASE_S             (APB2PERIPH_BASE_S + 0x3000UL)
#define TIM8_BASE_S             (APB2PERIPH_BASE_S + 0x3400UL)
#define USART1_BASE_S           (APB2PERIPH_BASE_S + 0x3800UL)
#define TIM15_BASE_S            (APB2PERIPH_BASE_S + 0x4000UL)
#define TIM16_BASE_S            (APB2PERIPH_BASE_S + 0x4400UL)
#define TIM17_BASE_S            (APB2PERIPH_BASE_S + 0x4800UL)
#define SPI4_BASE_S             (APB2PERIPH_BASE_S + 0x4C00UL)
#define SPI6_BASE_S             (APB2PERIPH_BASE_S + 0x5000UL)
#define SAI1_BASE_S             (APB2PERIPH_BASE_S + 0x5400UL)
#define SAI1_Block_A_BASE_S     (SAI1_BASE_S + 0x004UL)
#define SAI1_Block_B_BASE_S     (SAI1_BASE_S + 0x024UL)
#define SAI2_BASE_S             (APB2PERIPH_BASE_S + 0x5800UL)
#define SAI2_Block_A_BASE_S     (SAI2_BASE_S + 0x004UL)
#define SAI2_Block_B_BASE_S     (SAI2_BASE_S + 0x024UL)
#define USB_DRD_BASE_S          (APB2PERIPH_BASE_S + 0x6000UL)
#define USB_DRD_PMAADDR_S       (APB2PERIPH_BASE_S + 0x6400UL)

/*!< AHB1 secure peripherals */
#define GPDMA1_BASE_S           AHB1PERIPH_BASE_S
#define GPDMA2_BASE_S           (AHB1PERIPH_BASE_S + 0x01000UL)
#define FLASH_R_BASE_S          (AHB1PERIPH_BASE_S + 0x02000UL)
#define CRC_BASE_S              (AHB1PERIPH_BASE_S + 0x03000UL)
#define CORDIC_BASE_S           (AHB1PERIPH_BASE_S + 0x03800UL)
#define FMAC_BASE_S             (AHB1PERIPH_BASE_S + 0x03C00UL)
#define RAMCFG_BASE_S           (AHB1PERIPH_BASE_S + 0x06000UL)
#define ETH_BASE_S              (AHB1PERIPH_BASE_S + 0x8000UL)
#define ETH_MAC_BASE_S          (ETH_BASE_S)
#define ICACHE_BASE_S           (AHB1PERIPH_BASE_S + 0x10400UL)
#define DCACHE1_BASE_S          (AHB1PERIPH_BASE_S + 0x11400UL)
#define GTZC_TZSC1_BASE_S       (AHB1PERIPH_BASE_S + 0x12400UL)
#define GTZC_TZIC1_BASE_S       (AHB1PERIPH_BASE_S + 0x12800UL)
#define GTZC_MPCBB1_BASE_S      (AHB1PERIPH_BASE_S + 0x12C00UL)
#define GTZC_MPCBB2_BASE_S      (AHB1PERIPH_BASE_S + 0x13000UL)
#define GTZC_MPCBB3_BASE_S      (AHB1PERIPH_BASE_S + 0x13400UL)
#define BKPSRAM_BASE_S          (AHB1PERIPH_BASE_S + 0x16400UL)
#define GPDMA1_Channel0_BASE_S   (GPDMA1_BASE_S + 0x0050UL)
#define GPDMA1_Channel1_BASE_S   (GPDMA1_BASE_S + 0x00D0UL)
#define GPDMA1_Channel2_BASE_S   (GPDMA1_BASE_S + 0x0150UL)
#define GPDMA1_Channel3_BASE_S   (GPDMA1_BASE_S + 0x01D0UL)
#define GPDMA1_Channel4_BASE_S   (GPDMA1_BASE_S + 0x0250UL)
#define GPDMA1_Channel5_BASE_S   (GPDMA1_BASE_S + 0x02D0UL)
#define GPDMA1_Channel6_BASE_S   (GPDMA1_BASE_S + 0x0350UL)
#define GPDMA1_Channel7_BASE_S   (GPDMA1_BASE_S + 0x03D0UL)
#define GPDMA2_Channel0_BASE_S   (GPDMA2_BASE_S + 0x0050UL)
#define GPDMA2_Channel1_BASE_S   (GPDMA2_BASE_S + 0x00D0UL)
#define GPDMA2_Channel2_BASE_S   (GPDMA2_BASE_S + 0x0150UL)
#define GPDMA2_Channel3_BASE_S   (GPDMA2_BASE_S + 0x01D0UL)
#define GPDMA2_Channel4_BASE_S   (GPDMA2_BASE_S + 0x0250UL)
#define GPDMA2_Channel5_BASE_S   (GPDMA2_BASE_S + 0x02D0UL)
#define GPDMA2_Channel6_BASE_S   (GPDMA2_BASE_S + 0x0350UL)
#define GPDMA2_Channel7_BASE_S   (GPDMA2_BASE_S + 0x03D0UL)
#define RAMCFG_SRAM1_BASE_S     (RAMCFG_BASE_S)
#define RAMCFG_SRAM2_BASE_S     (RAMCFG_BASE_S + 0x0040UL)
#define RAMCFG_SRAM3_BASE_S     (RAMCFG_BASE_S + 0x0080UL)
#define RAMCFG_BKPRAM_BASE_S    (RAMCFG_BASE_S + 0x0100UL)

/*!< AHB2 secure peripherals */
#define GPIOA_BASE_S            (AHB2PERIPH_BASE_S + 0x00000UL)
#define GPIOB_BASE_S            (AHB2PERIPH_BASE_S + 0x00400UL)
#define GPIOC_BASE_S            (AHB2PERIPH_BASE_S + 0x00800UL)
#define GPIOD_BASE_S            (AHB2PERIPH_BASE_S + 0x00C00UL)
#define GPIOE_BASE_S            (AHB2PERIPH_BASE_S + 0x01000UL)
#define GPIOF_BASE_S            (AHB2PERIPH_BASE_S + 0x01400UL)
#define GPIOG_BASE_S            (AHB2PERIPH_BASE_S + 0x01800UL)
#define GPIOH_BASE_S            (AHB2PERIPH_BASE_S + 0x01C00UL)
#define GPIOI_BASE_S            (AHB2PERIPH_BASE_S + 0x02000UL)
#define ADC1_BASE_S             (AHB2PERIPH_BASE_S + 0x08000UL)
#define ADC2_BASE_S             (AHB2PERIPH_BASE_S + 0x08100UL)
#define ADC12_COMMON_BASE_S     (AHB2PERIPH_BASE_S + 0x08300UL)
#define DAC1_BASE_S             (AHB2PERIPH_BASE_S + 0x08400UL)
#define DCMI_BASE_S             (AHB2PERIPH_BASE_S + 0x0C000UL)
#define PSSI_BASE_S             (AHB2PERIPH_BASE_S + 0x0C400UL)
#define AES_BASE_S              (AHB2PERIPH_BASE_S + 0xA0000UL)
#define HASH_BASE_S             (AHB2PERIPH_BASE_S + 0xA0400UL)
#define HASH_DIGEST_BASE_S      (AHB2PERIPH_BASE_S + 0xA0710UL)
#define RNG_BASE_S              (AHB2PERIPH_BASE_S + 0xA0800UL)
#define SAES_BASE_S             (AHB2PERIPH_BASE_S + 0xA0C00UL)
#define PKA_BASE_S              (AHB2PERIPH_BASE_S + 0xA2000UL)
#define PKA_RAM_BASE_S          (AHB2PERIPH_BASE_S + 0xA2400UL)

/*!< APB3 secure peripherals */
#define SBS_BASE_S              (APB3PERIPH_BASE_S + 0x0400UL)
#define SPI5_BASE_S             (APB3PERIPH_BASE_S + 0x2000UL)
#define LPUART1_BASE_S          (APB3PERIPH_BASE_S + 0x2400UL)
#define I2C3_BASE_S             (APB3PERIPH_BASE_S + 0x2800UL)
#define I2C4_BASE_S             (APB3PERIPH_BASE_S + 0x2C00UL)
#define LPTIM1_BASE_S           (APB3PERIPH_BASE_S + 0x4400UL)
#define LPTIM3_BASE_S           (APB3PERIPH_BASE_S + 0x4800UL)
#define LPTIM4_BASE_S           (APB3PERIPH_BASE_S + 0x4C00UL)
#define LPTIM5_BASE_S           (APB3PERIPH_BASE_S + 0x5000UL)
#define LPTIM6_BASE_S           (APB3PERIPH_BASE_S + 0x5400UL)
#define VREFBUF_BASE_S          (APB3PERIPH_BASE_S + 0x7400UL)
#define RTC_BASE_S              (APB3PERIPH_BASE_S + 0x7800UL)
#define TAMP_BASE_S             (APB3PERIPH_BASE_S + 0x7C00UL)

/*!< AHB3 secure peripherals */
#define PWR_BASE_S              (AHB3PERIPH_BASE_S + 0x0800UL)
#define RCC_BASE_S              (AHB3PERIPH_BASE_S + 0x0C00UL)
#define EXTI_BASE_S             (AHB3PERIPH_BASE_S + 0x2000UL)
#define DEBUG_BASE_S            (AHB3PERIPH_BASE_S + 0x4000UL)

/*!< AHB4 secure peripherals */
#define OTFDEC1_BASE_S          (AHB4PERIPH_BASE_S + 0x5000UL)
#define OTFDEC1_REGION1_BASE_S  (OTFDEC1_BASE_S + 0x20UL)
#define OTFDEC1_REGION2_BASE_S  (OTFDEC1_BASE_S + 0x50UL)
#define OTFDEC1_REGION3_BASE_S  (OTFDEC1_BASE_S + 0x80UL)
#define OTFDEC1_REGION4_BASE_S  (OTFDEC1_BASE_S + 0xB0UL)
#define SDMMC1_BASE_S           (AHB4PERIPH_BASE_S + 0x8000UL)
#define DLYB_SDMMC1_BASE_S      (AHB4PERIPH_BASE_S + 0x8400UL)
#define SDMMC2_BASE_S           (AHB4PERIPH_BASE_S + 0x8C00UL)
#define DLYB_SDMMC2_BASE_S      (AHB4PERIPH_BASE_S + 0x8800UL)
#define FMC_R_BASE_S            (AHB4PERIPH_BASE_S + 0x1000400UL) /*!< FMC control registers base address              */
#define OCTOSPI1_R_BASE_S       (AHB4PERIPH_BASE_S + 0x1001400UL) /*!< OCTOSPI1 control registers base address         */
#define DLYB_OCTOSPI1_BASE_S    (AHB4PERIPH_BASE_S + 0x0F000UL)

/*!< FMC Banks Non secure registers base address */
#define FMC_Bank1_R_BASE_S      (FMC_R_BASE_S + 0x0000UL)
#define FMC_Bank1E_R_BASE_S     (FMC_R_BASE_S + 0x0104UL)
#define FMC_Bank3_R_BASE_S      (FMC_R_BASE_S + 0x0080UL)
#define FMC_Bank5_6_R_BASE_S    (FMC_R_BASE_S + 0x0140UL)

/* Debug MCU registers base address */
#define DBGMCU_BASE             (0x44024000UL)
#define PACKAGE_BASE            (0x08FFF80EUL) /*!< Package data register base address     */
#define UID_BASE                (0x08FFF800UL) /*!< Unique device ID register base address */
#define FLASHSIZE_BASE          (0x08FFF80CUL) /*!< Flash size data register base address  */

/* Internal Flash OTP Area */
#define FLASH_OTP_BASE          (0x08FFF000UL) /*!< FLASH OTP (one-time programmable) base address */
#define FLASH_OTP_SIZE          (0x800U)       /*!< 2048 bytes OTP (one-time programmable)         */

/* Flash system Area */
#define FLASH_SYSTEM_BASE_NS    (0x0BF80000UL) /*!< FLASH System non-secure base address  */
#define FLASH_SYSTEM_BASE_S     (0x0FF80000UL) /*!< FLASH System secure base address      */
#define FLASH_SYSTEM_SIZE       (0x10000U)     /*!< 64 Kbytes system Flash */

/* Internal Flash EDATA Area */
#define FLASH_EDATA_BASE_NS     (0x09000000UL) /*!< FLASH high-cycle data non-secure base address */
#define FLASH_EDATA_BASE_S      (0x0D000000UL) /*!< FLASH high-cycle data secure base address     */
#define FLASH_EDATA_SIZE        (0x18000U)     /*!< 96 KB of Flash high-cycle data                */

/* Internal Flash OBK Area */
#define FLASH_OBK_BASE_NS       (0x0BFD0000UL)  /*!< FLASH OBK (option byte keys) non-secure base address */
#define FLASH_OBK_BASE_S        (0x0FFD0000UL)  /*!< FLASH OBK (option byte keys) secure base address     */
#define FLASH_OBK_SIZE          (0x2000U)       /*!< 8 KB of option byte keys                             */
#define FLASH_OBK_HDPL0_SIZE    (0x100U)        /*!< 256 Bytes of HDPL1 option byte keys                  */

#define FLASH_OBK_HDPL1_BASE_NS (FLASH_OBK_BASE_NS + FLASH_OBK_HDPL0_SIZE)         /*!< FLASH OBK HDPL1 non-secure base address         */
#define FLASH_OBK_HDPL1_BASE_S  (FLASH_OBK_BASE_S + FLASH_OBK_HDPL0_SIZE)          /*!< FLASH OBK HDPL1 secure base address             */
#define FLASH_OBK_HDPL1_SIZE    (0x800U)                                           /*!< 2 KB of HDPL1 option byte keys                  */

#define FLASH_OBK_HDPL2_BASE_NS (FLASH_OBK_HDPL1_BASE_NS + FLASH_OBK_HDPL1_SIZE)   /*!< FLASH OBK HDPL2 non-secure base address         */
#define FLASH_OBK_HDPL2_BASE_S  (FLASH_OBK_HDPL1_BASE_S + FLASH_OBK_HDPL1_SIZE)    /*!< FLASH OBK HDPL2 secure base address             */
#define FLASH_OBK_HDPL2_SIZE    (0x300U)                                           /*!< 768 Bytes of HDPL2 option byte keys             */

#define FLASH_OBK_HDPL3_BASE_NS (FLASH_OBK_HDPL2_BASE_NS + FLASH_OBK_HDPL2_SIZE)   /*!< FLASH OBK HDPL3 non-secure base address         */
#define FLASH_OBK_HDPL3_BASE_S  (FLASH_OBK_HDPL2_BASE_S + FLASH_OBK_HDPL2_SIZE)    /*!< FLASH OBK HDPL3 secure base address             */
#define FLASH_OBK_HDPL3_SIZE    (0x13F0U)                                          /*!< 5104 Bytes HDPL3 option byte keys */
#endif


}
