#pragma once

#include <cstddef>
#include <tuple>

#include <stm32h5/csr/adc.hpp>
#include <stm32h5/csr/aes.hpp>
#include <stm32h5/csr/can.hpp>
#include <stm32h5/csr/cec.hpp>
#include <stm32h5/csr/cordic.hpp>
#include <stm32h5/csr/crc.hpp>
#include <stm32h5/csr/crs.hpp>
#include <stm32h5/csr/dac.hpp>
#include <stm32h5/csr/dbgmcu.hpp>
#include <stm32h5/csr/dcache.hpp>
#include <stm32h5/csr/dcmi.hpp>
#include <stm32h5/csr/dlyb.hpp>
#include <stm32h5/csr/dma.hpp>
#include <stm32h5/csr/dts.hpp>
#include <stm32h5/csr/eth.hpp>
#include <stm32h5/csr/exti.hpp>
#include <stm32h5/csr/flash.hpp>
#include <stm32h5/csr/fmac.hpp>
#include <stm32h5/csr/fmc.hpp>
#include <stm32h5/csr/gpio.hpp>
#include <stm32h5/csr/gtzc.hpp>
#include <stm32h5/csr/hash.hpp>
#include <stm32h5/csr/i2c.hpp>
#include <stm32h5/csr/i3c.hpp>
#include <stm32h5/csr/icache.hpp>
#include <stm32h5/csr/iwdg.hpp>
#include <stm32h5/csr/octospi.hpp>
#include <stm32h5/csr/otfdec.hpp>
#include <stm32h5/csr/ramcfg.hpp>
#include <stm32h5/csr/rcc.hpp>
#include <stm32h5/csr/pka.hpp>
#include <stm32h5/csr/pssi.hpp>
#include <stm32h5/csr/pwr.hpp>
#include <stm32h5/csr/rcc.hpp>
#include <stm32h5/csr/rtc.hpp>
#include <stm32h5/csr/rng.hpp>
#include <stm32h5/csr/sai.hpp>
#include <stm32h5/csr/sbs.hpp>
#include <stm32h5/csr/sdmmc.hpp>
#include <stm32h5/csr/spi.hpp>
#include <stm32h5/csr/tamp.hpp>
#include <stm32h5/csr/timer.hpp>
#include <stm32h5/csr/ucpd.hpp>
#include <stm32h5/csr/usart.hpp>
#include <stm32h5/csr/usb.hpp>
#include <stm32h5/csr/vref.hpp>
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
#define FMC_BANK                 (FMC_BASE  + 0x20000000UL)  /*!< FMC Memory Bank3 for NAND */
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
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x0000> tim2 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x0400> tim3 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x0800> tim4 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x0C00> tim5 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x1000> tim6 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x1400> tim7 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x1800> tim12 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x1C00> tim13 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x2000> tim14 {};
      static constexpr CSR::csr_ptr<CSR::wwdg, base + 0x2C00> wwdg {};
      static constexpr CSR::csr_ptr<CSR::iwdg, base + 0x3000> iwdg {};
      static constexpr CSR::csr_ptr<CSR::spi, base + 0x3800> spi2 {};
      static constexpr CSR::csr_ptr<CSR::spi, base + 0x3C00> spi3 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x4400> usart2 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x4800> usart3 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x4C00> uart4 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x5000> uart5 {};
      static constexpr CSR::csr_ptr<CSR::i2c, base + 0x5400> i2c1 {};
      static constexpr CSR::csr_ptr<CSR::i2c, base + 0x5800> i2c2 {};
      static constexpr CSR::csr_ptr<CSR::i3c, base + 0x5C00> i3c1 {};
      static constexpr CSR::csr_ptr<CSR::crs, base + 0x6000> crs {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x6400> usart6 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x6800> usart10 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x6C00> usart11 {};
      static constexpr CSR::csr_ptr<CSR::cec, base + 0x7000> cec {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x7800> uart7 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x7C00> uart8 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x8000> uart9 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x8400> uart12 {};
      static constexpr CSR::csr_ptr<CSR::dts, base + 0x8C00> dts {};
      static constexpr CSR::csr_ptr<CSR::lptimer, base + 0x9400> lptim2 {};
      static constexpr CSR::csr_ptr<CSR::fdcan, base + 0xA400> fdcan1 {};
      static constexpr CSR::csr_ptr<CSR::fdcan_config, base + 0xA500> fdcan_config {};
      static constexpr CSR::csr_ptr<CSR::fdcan_sram, base + 0xAC00> fdcan_sram {};
      static constexpr CSR::csr_ptr<CSR::fdcan, base + 0xA800> fdcan2 {};
      static constexpr CSR::csr_ptr<CSR::ucpd, base + 0xDC00> ucpd {};
    };

    template <uint32_t base>
    struct apb2
    {
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x2C00> tim1 {};
      static constexpr CSR::csr_ptr<CSR::spi, base + 0x3000> spi1 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x3400> tim8 {};
      static constexpr CSR::csr_ptr<CSR::usart, base + 0x3800> usart1 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x4000> tim15 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x4400> tim16 {};
      static constexpr CSR::csr_ptr<CSR::timer, base + 0x4800> tim17 {};
      static constexpr CSR::csr_ptr<CSR::spi, base + 0x4C00> spi4 {};
      static constexpr CSR::csr_ptr<CSR::spi, base + 0x5000> spi6 {};
      static constexpr CSR::csr_ptr<CSR::sai, base + 0x5400> sai1 {};
      static constexpr CSR::csr_ptr<CSR::sai_block, base + 0x5404> sai1_block_A {};
      static constexpr CSR::csr_ptr<CSR::sai_block, base + 0x5424> sai1_block_B {};
      static constexpr CSR::csr_ptr<CSR::sai, base + 0x5800> sai2 {};
      static constexpr CSR::csr_ptr<CSR::sai_block, base + 0x5804> sai2_block_A {};
      static constexpr CSR::csr_ptr<CSR::sai_block, base + 0x5824> sai2_block_B {};
      static constexpr CSR::csr_ptr<CSR::usb_drd, base + 0x6000> usb {};
      static constexpr CSR::csr_ptr<CSR::usb_drd_pma_buf, base + 0x6400> usb_pma {};
    };

    template <uint32_t base>
    struct ahb1
    {
      dmac<base> gpdma1;
      dmac<base + 0x01000> gpdma2;
      CSR::csr_ptr<CSR::flash, base + 0x02000> flash;
      CSR::csr_ptr<CSR::crc, base + 0x03000> crc;
      CSR::csr_ptr<CSR::cordic, base + 0x03800> cordic;
      CSR::csr_ptr<CSR::fmac, base + 0x03C00> fmac;
      CSR::csr_ptr<CSR::ramcfg, base + 0x06000> ramcfg;
      CSR::csr_ptr<CSR::eth, base + 0x08000> eth;
      CSR::csr_ptr<CSR::icache, base + 0x10400> icache;
      CSR::csr_ptr<CSR::dcache, base + 0x11400> dcache;
      CSR::csr_ptr<CSR::gtzc_sc, base + 0x12400> gtzc_sc;
      CSR::csr_ptr<CSR::gtzc_ic, base + 0x12800> gtzc_ic;
      CSR::csr_ptr<CSR::gtzc_mpcbb, base + 0x12C00> mpcbb1;
      CSR::csr_ptr<CSR::gtzc_mpcbb, base + 0x13000> mpcbb2;
      CSR::csr_ptr<CSR::gtzc_mpcbb, base + 0x13400> mpcbb3;


#if 0
      // Well, what is this???
#define BKPSRAM_BASE_NS          (AHB1PERIPH_BASE_NS + 0x16400UL)
#endif
    };

    template <uint32_t base>
    struct ahb2
    {
      static constexpr CSR::csr_ptr<CSR::gpio, base + 0x00000> gpioa {};
      static constexpr CSR::csr_ptr<CSR::gpio, base + 0x00400> gpiob {};
      static constexpr CSR::csr_ptr<CSR::gpio, base + 0x00800> gpioc {};
      static constexpr CSR::csr_ptr<CSR::gpio, base + 0x00C00> gpiod {};
      static constexpr CSR::csr_ptr<CSR::gpio, base + 0x01000> gpioe {};
      static constexpr CSR::csr_ptr<CSR::gpio, base + 0x01400> gpiof {};
      static constexpr CSR::csr_ptr<CSR::gpio, base + 0x01800> gpiog {};
      static constexpr CSR::csr_ptr<CSR::gpio, base + 0x01C00> gpioh {};
      static constexpr CSR::csr_ptr<CSR::gpio, base + 0x02000> gpioi {};
      CSR::csr_ptr<CSR::adc, base + 0x08000> adc1;
      CSR::csr_ptr<CSR::adc, base + 0x08100> adc2;
      CSR::csr_ptr<CSR::adc_common, base + 0x08000> adc_common;
      CSR::csr_ptr<CSR::dac, base + 0x08400> dac1;
      CSR::csr_ptr<CSR::dcmi, base + 0x0C000> dcmi;
      CSR::csr_ptr<CSR::pssi, base + 0x0C400> pssi;
      CSR::csr_ptr<CSR::aes, base + 0xA0000> aes;
      CSR::csr_ptr<CSR::hash, base + 0xA0400> hash;
      CSR::csr_ptr<CSR::hash_digest, base + 0xA0710> hash_digest;
      CSR::csr_ptr<CSR::aes, base + 0xA0C00> saes;
      CSR::csr_ptr<CSR::rng, base + 0xA0800> rng;
      CSR::csr_ptr<CSR::pka, base + 0xA2000> pka;
    };

    template <uint32_t base>
    struct apb3
    {
      CSR::csr_ptr<CSR::sbs, base + 0x04000> sbs;
      CSR::csr_ptr<CSR::spi, base + 0x20000> spi5;
      CSR::csr_ptr<CSR::usart, base + 0x24000> lpuart1;
      CSR::csr_ptr<CSR::i2c, base + 0x28000> i2c3;
      CSR::csr_ptr<CSR::i2c, base + 0x2C000> i2c4;
      CSR::csr_ptr<CSR::lptimer, base + 0x44000> lptimer1;
      CSR::csr_ptr<CSR::lptimer, base + 0x48000> lptimer3;
      CSR::csr_ptr<CSR::lptimer, base + 0x4C000> lptimer4;
      CSR::csr_ptr<CSR::lptimer, base + 0x50000> lptimer5;
      CSR::csr_ptr<CSR::lptimer, base + 0x54000> lptimer6;
      CSR::csr_ptr<CSR::vref, base + 0x74000> vref;
      CSR::csr_ptr<CSR::rtc, base + 0x78000> rtc;
      CSR::csr_ptr<CSR::tamp, base + 0x7C000> tamp;
    };

    template <uint32_t base>
    struct ahb3
    {
      static constexpr CSR::csr_ptr<CSR::pwr, base + 0x0800> pwr {};
      static constexpr CSR::csr_ptr<CSR::rcc, base + 0x0C00> rcc {};
      static constexpr CSR::csr_ptr<CSR::exti, base + 0x2000> exti {};
      static constexpr CSR::csr_ptr<CSR::dbgmcu, base + 0x4000> dbgmcu {};
    };

    template <uint32_t base>
    struct ahb4
    {
      CSR::csr_ptr<CSR::otfdec, base + 0x5000> otfdec1;
      CSR::csr_ptr<CSR::otfdec_region, base + 0x5020> otfdec1r1;
      CSR::csr_ptr<CSR::otfdec_region, base + 0x5050> otfdec1r2;
      CSR::csr_ptr<CSR::otfdec_region, base + 0x5080> otfdec1r3;
      CSR::csr_ptr<CSR::otfdec_region, base + 0x50B0> otfdec1r4;
      CSR::csr_ptr<CSR::sdmmc, base + 0x8000> sdmmc1;
      CSR::csr_ptr<CSR::dlyblk, base + 0x8400> sdmmc1_dly;
      CSR::csr_ptr<CSR::sdmmc, base + 0x8C00> sdmmc2;
      CSR::csr_ptr<CSR::dlyblk, base + 0x8800> sdmmc2_dly;
      CSR::csr_ptr<CSR::dlyblk, base + 0xF000> octospi_dly;

      CSR::csr_ptr<CSR::fmc_bank1, base + 0x1000400> fmc_bank1;
      CSR::csr_ptr<CSR::fmc_bank3, base + 0x1000400 + 0x80> fmc_bank3;
      CSR::csr_ptr<CSR::fmc_bank1e, base + 0x1000400 + 0x104> fmc_bank1e;
      CSR::csr_ptr<CSR::fmc_bank56, base + 0x1000400 + 0x140> fmc_bank56;

      CSR::csr_ptr<CSR::octospi, base + 0x1001400> octospi;
    };
  };

  template <uint32_t base>
  struct peripherals
  {
    constexpr static struct peripherals *p = (struct peripherals *)base;

    static constexpr bus::apb1<base + 0> apb1 {};
    static constexpr bus::apb2<base + 0x00010000> apb2 {};
    static constexpr bus::ahb1<base + 0x00020000> ahb1 {};
    static constexpr bus::ahb2<base + 0x02020000> ahb2 {};
    static constexpr bus::apb3<base + 0x04000000> apb3 {};
    static constexpr bus::ahb3<base + 0x04020000> ahb3 {};
    static constexpr bus::ahb4<base + 0x06000000> ahb4 {};



    static constexpr auto &gpioa = ahb2.gpioa;
    static constexpr auto &gpiob = ahb2.gpiob;
    static constexpr auto &gpioc = ahb2.gpioc;
    static constexpr auto &gpiod = ahb2.gpiod;
    static constexpr auto &gpioe = ahb2.gpioe;
    static constexpr auto &gpiof = ahb2.gpiof;
    static constexpr auto &gpiog = ahb2.gpiog;
    static constexpr auto &gpioh = ahb2.gpioh;
    static constexpr auto &gpioi = ahb2.gpioi;

    static constexpr auto &sai1 = apb2.sai1;
    static constexpr auto &sai1_block_A = apb2.sai1_block_A;
    static constexpr auto &sai1_block_B = apb2.sai1_block_B;
    static constexpr auto &sai2 = apb2.sai2;
    static constexpr auto &sai2_block_A = apb2.sai2_block_A;
    static constexpr auto &sai2_block_B = apb2.sai2_block_B;


    static constexpr auto &spi1 = apb2.spi1;
    static constexpr auto &spi4 = apb2.spi4;
    static constexpr auto &spi6 = apb2.spi6;

    static constexpr auto &tim1 = apb2.tim1;
    static constexpr auto &tim2 = apb1.tim2;
    static constexpr auto &tim3 = apb1.tim3;
    static constexpr auto &tim4 = apb1.tim4;
    static constexpr auto &tim5 = apb1.tim5;
    static constexpr auto &tim6 = apb1.tim6;
    static constexpr auto &tim7 = apb1.tim7;
    static constexpr auto &tim8 = apb2.tim8;
    static constexpr auto &tim15 = apb2.tim15;
    static constexpr auto &tim16 = apb2.tim16;
    static constexpr auto &tim17 = apb2.tim17;

    static constexpr auto &usart1 = apb2.usart1;

    
    static constexpr auto &usb = apb2.usb;
    static constexpr auto &usb_pma = apb2.usb_pma;

    static constexpr auto &pwr = ahb3.pwr;
    static constexpr auto &rcc = ahb3.rcc;
    static constexpr auto &exti = ahb3.exti;
    static constexpr auto &dbgmcu = ahb3.dbgmcu;

    static constexpr auto usarts = std::make_tuple(0,
        apb2.usart1,
        apb1.usart2,
        apb1.usart3,
        apb1.uart4,
        apb1.uart5);
  };

  extern peripherals<0x40000000> periph_nsec;
  //extern peripherals<> periph_sec;

  template <int n>
  inline volatile CSR::usart *get_usart()
  {
    return NULL;
  }

  template <>
  inline volatile CSR::usart *get_usart<1>()
  {
    return periph_nsec.apb2.usart1.ptr();
  }

}
