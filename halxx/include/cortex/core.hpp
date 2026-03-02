#pragma once

typedef enum
{
/* =======================================  ARM Cortex-M33 Specific Interrupt Numbers  ======================================= */
  Reset_IRQn                = -15,    /*!< -15 Reset Vector, invoked on Power up and warm reset              */
  NonMaskableInt_IRQn       = -14,    /*!< -14 Non maskable Interrupt, cannot be stopped or preempted        */
  HardFault_IRQn            = -13,    /*!< -13 Hard Fault, all classes of Fault                              */
  MemoryManagement_IRQn     = -12,    /*!< -12 Memory Management, MPU mismatch, including Access Violation
                                               and No Match                                                  */
  BusFault_IRQn             = -11,    /*!< -11 Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                               related Fault                                                 */
  UsageFault_IRQn           = -10,    /*!< -10 Usage Fault, i.e. Undef Instruction, Illegal State Transition */
  SecureFault_IRQn          =  -9,    /*!< -9  Secure Fault                                                  */
  SVCall_IRQn               =  -5,    /*!< -5  System Service Call via SVC instruction                       */
  DebugMonitor_IRQn         =  -4,    /*!< -4  Debug Monitor                                                 */
  PendSV_IRQn               =  -2,    /*!< -2  Pendable request for system service                           */
  SysTick_IRQn              =  -1,    /*!< -1  System Tick Timer                                             */

/* ===========================================  STM32H573xx Specific Interrupt Numbers  ====================================== */
  WWDG_IRQn                 = 0,      /*!< Window WatchDog interrupt                                         */
  PVD_AVD_IRQn              = 1,      /*!< PVD/AVD through EXTI Line detection Interrupt                     */
  RTC_IRQn                  = 2,      /*!< RTC non-secure interrupt                                          */
  RTC_S_IRQn                = 3,      /*!< RTC secure interrupt                                              */
  TAMP_IRQn                 = 4,      /*!< Tamper global interrupt                                           */
  RAMCFG_IRQn               = 5,      /*!< RAMCFG global interrupt                                           */
  FLASH_IRQn                = 6,      /*!< FLASH non-secure global interrupt                                 */
  FLASH_S_IRQn              = 7,      /*!< FLASH secure global interrupt                                     */
  GTZC_IRQn                 = 8,      /*!< Global TrustZone Controller interrupt                             */
  RCC_IRQn                  = 9,      /*!< RCC non secure global interrupt                                   */
  RCC_S_IRQn                = 10,     /*!< RCC secure global interrupt                                       */
  EXTI0_IRQn                = 11,     /*!< EXTI Line0 interrupt                                              */
  EXTI1_IRQn                = 12,     /*!< EXTI Line1 interrupt                                              */
  EXTI2_IRQn                = 13,     /*!< EXTI Line2 interrupt                                              */
  EXTI3_IRQn                = 14,     /*!< EXTI Line3 interrupt                                              */
  EXTI4_IRQn                = 15,     /*!< EXTI Line4 interrupt                                              */
  EXTI5_IRQn                = 16,     /*!< EXTI Line5 interrupt                                              */
  EXTI6_IRQn                = 17,     /*!< EXTI Line6 interrupt                                              */
  EXTI7_IRQn                = 18,     /*!< EXTI Line7 interrupt                                              */
  EXTI8_IRQn                = 19,     /*!< EXTI Line8 interrupt                                              */
  EXTI9_IRQn                = 20,     /*!< EXTI Line9 interrupt                                              */
  EXTI10_IRQn               = 21,     /*!< EXTI Line10 interrupt                                             */
  EXTI11_IRQn               = 22,     /*!< EXTI Line11 interrupt                                             */
  EXTI12_IRQn               = 23,     /*!< EXTI Line12 interrupt                                             */
  EXTI13_IRQn               = 24,     /*!< EXTI Line13 interrupt                                             */
  EXTI14_IRQn               = 25,     /*!< EXTI Line14 interrupt                                             */
  EXTI15_IRQn               = 26,     /*!< EXTI Line15 interrupt                                             */
  GPDMA1_Channel0_IRQn      = 27,     /*!< GPDMA1 Channel 0 global interrupt                                 */
  GPDMA1_Channel1_IRQn      = 28,     /*!< GPDMA1 Channel 1 global interrupt                                 */
  GPDMA1_Channel2_IRQn      = 29,     /*!< GPDMA1 Channel 2 global interrupt                                 */
  GPDMA1_Channel3_IRQn      = 30,     /*!< GPDMA1 Channel 3 global interrupt                                 */
  GPDMA1_Channel4_IRQn      = 31,     /*!< GPDMA1 Channel 4 global interrupt                                 */
  GPDMA1_Channel5_IRQn      = 32,     /*!< GPDMA1 Channel 5 global interrupt                                 */
  GPDMA1_Channel6_IRQn      = 33,     /*!< GPDMA1 Channel 6 global interrupt                                 */
  GPDMA1_Channel7_IRQn      = 34,     /*!< GPDMA1 Channel 7 global interrupt                                 */
  IWDG_IRQn                 = 35,     /*!< IWDG global interrupt                                             */
  SAES_IRQn                 = 36,     /*!< Secure AES global interrupt                                       */
  ADC1_IRQn                 = 37,     /*!< ADC1 global interrupt                                             */
  DAC1_IRQn                 = 38,     /*!< DAC1 global interrupt                                             */
  FDCAN1_IT0_IRQn           = 39,     /*!< FDCAN1 interrupt 0                                                */
  FDCAN1_IT1_IRQn           = 40,     /*!< FDCAN1 interrupt 1                                                */
  TIM1_BRK_IRQn             = 41,     /*!< TIM1 Break interrupt                                              */
  TIM1_UP_IRQn              = 42,     /*!< TIM1 Update interrupt                                             */
  TIM1_TRG_COM_IRQn         = 43,     /*!< TIM1 Trigger and Commutation interrupt                            */
  TIM1_CC_IRQn              = 44,     /*!< TIM1 Capture Compare interrupt                                    */
  TIM2_IRQn                 = 45,     /*!< TIM2 global interrupt                                             */
  TIM3_IRQn                 = 46,     /*!< TIM3 global interrupt                                             */
  TIM4_IRQn                 = 47,     /*!< TIM4 global interrupt                                             */
  TIM5_IRQn                 = 48,     /*!< TIM5 global interrupt                                             */
  TIM6_IRQn                 = 49,     /*!< TIM6 global interrupt                                             */
  TIM7_IRQn                 = 50,     /*!< TIM7 global interrupt                                             */
  I2C1_EV_IRQn              = 51,     /*!< I2C1 Event interrupt                                              */
  I2C1_ER_IRQn              = 52,     /*!< I2C1 Error interrupt                                              */
  I2C2_EV_IRQn              = 53,     /*!< I2C2 Event interrupt                                              */
  I2C2_ER_IRQn              = 54,     /*!< I2C2 Error interrupt                                              */
  SPI1_IRQn                 = 55,     /*!< SPI1 global interrupt                                             */
  SPI2_IRQn                 = 56,     /*!< SPI2 global interrupt                                             */
  SPI3_IRQn                 = 57,     /*!< SPI3 global interrupt                                             */
  USART1_IRQn               = 58,     /*!< USART1 global interrupt                                           */
  USART2_IRQn               = 59,     /*!< USART2 global interrupt                                           */
  USART3_IRQn               = 60,     /*!< USART3 global interrupt                                           */
  UART4_IRQn                = 61,     /*!< UART4 global interrupt                                            */
  UART5_IRQn                = 62,     /*!< UART5 global interrupt                                            */
  LPUART1_IRQn              = 63,     /*!< LPUART1 global interrupt                                          */
  LPTIM1_IRQn               = 64,     /*!< LPTIM1 global interrupt                                           */
  TIM8_BRK_IRQn             = 65,     /*!< TIM8 Break interrupt                                              */
  TIM8_UP_IRQn              = 66,     /*!< TIM8 Update interrupt                                             */
  TIM8_TRG_COM_IRQn         = 67,     /*!< TIM8 Trigger and Commutation interrupt                            */
  TIM8_CC_IRQn              = 68,     /*!< TIM8 Capture Compare interrupt                                    */
  ADC2_IRQn                 = 69,     /*!< ADC2 global interrupt                                             */
  LPTIM2_IRQn               = 70,     /*!< LPTIM2 global interrupt                                           */
  TIM15_IRQn                = 71,     /*!< TIM15 global interrupt                                            */
  TIM16_IRQn                = 72,     /*!< TIM16 global interrupt                                            */
  TIM17_IRQn                = 73,     /*!< TIM17 global interrupt                                            */
  USB_DRD_FS_IRQn           = 74,     /*!< USB FS global interrupt                                           */
  CRS_IRQn                  = 75,     /*!< CRS global interrupt                                              */
  UCPD1_IRQn                = 76,     /*!< UCPD1 global interrupt                                            */
  FMC_IRQn                  = 77,     /*!< FMC global interrupt                                              */
  OCTOSPI1_IRQn             = 78,     /*!< OctoSPI1 global interrupt                                         */
  SDMMC1_IRQn               = 79,     /*!< SDMMC1 global interrupt                                           */
  I2C3_EV_IRQn              = 80,     /*!< I2C3 event interrupt                                              */
  I2C3_ER_IRQn              = 81,     /*!< I2C3 error interrupt                                              */
  SPI4_IRQn                 = 82,     /*!< SPI4 global interrupt                                             */
  SPI5_IRQn                 = 83,     /*!< SPI5 global interrupt                                             */
  SPI6_IRQn                 = 84,     /*!< SPI6 global interrupt                                             */
  USART6_IRQn               = 85,     /*!< USART6 global interrupt                                           */
  USART10_IRQn              = 86,     /*!< USART10 global interrupt                                          */
  USART11_IRQn              = 87,     /*!< USART11 global interrupt                                          */
  SAI1_IRQn                 = 88,     /*!< Serial Audio Interface 1 global interrupt                         */
  SAI2_IRQn                 = 89,     /*!< Serial Audio Interface 2 global interrupt                         */
  GPDMA2_Channel0_IRQn      = 90,     /*!< GPDMA2 Channel 0 global interrupt                                 */
  GPDMA2_Channel1_IRQn      = 91,     /*!< GPDMA2 Channel 1 global interrupt                                 */
  GPDMA2_Channel2_IRQn      = 92,     /*!< GPDMA2 Channel 2 global interrupt                                 */
  GPDMA2_Channel3_IRQn      = 93,     /*!< GPDMA2 Channel 3 global interrupt                                 */
  GPDMA2_Channel4_IRQn      = 94,     /*!< GPDMA2 Channel 4 global interrupt                                 */
  GPDMA2_Channel5_IRQn      = 95,     /*!< GPDMA2 Channel 5 global interrupt                                 */
  GPDMA2_Channel6_IRQn      = 96,     /*!< GPDMA2 Channel 6 global interrupt                                 */
  GPDMA2_Channel7_IRQn      = 97,     /*!< GPDMA2 Channel 7 global interrupt                                 */
  UART7_IRQn                = 98,     /*!< UART7 global interrupt                                            */
  UART8_IRQn                = 99,     /*!< UART8 global interrupt                                            */
  UART9_IRQn                = 100,    /*!< UART9 global interrupt                                            */
  UART12_IRQn               = 101,    /*!< UART12 global interrupt                                           */
  SDMMC2_IRQn               = 102,    /*!< SDMMC2 global interrupt                                           */
  FPU_IRQn                  = 103,    /*!< FPU global interrupt                                              */
  ICACHE_IRQn               = 104,    /*!< Instruction cache global interrupt                                */
  DCACHE1_IRQn              = 105,    /*!< Data cache global interrupt                                       */
  ETH_IRQn                  = 106,    /*!< Ethernet global interrupt                                         */
  ETH_WKUP_IRQn             = 107,    /*!< Ethernet Wakeup global interrupt                                  */
  DCMI_PSSI_IRQn            = 108,    /*!< DCMI/PSSI global interrupt                                        */
  FDCAN2_IT0_IRQn           = 109,    /*!< FDCAN2 interrupt 0                                                */
  FDCAN2_IT1_IRQn           = 110,    /*!< FDCAN2 interrupt 1                                                */
  CORDIC_IRQn               = 111,    /*!< CORDIC global interrupt                                           */
  FMAC_IRQn                 = 112,    /*!< FMAC global interrupt                                             */
  DTS_IRQn                  = 113,    /*!< DTS global interrupt                                              */
  RNG_IRQn                  = 114,    /*!< RNG global interrupt                                              */
  OTFDEC1_IRQn              = 115,    /*!< OTFDEC1 global interrupt                                          */
  AES_IRQn                  = 116,    /*!< AES global interrupt                                              */
  HASH_IRQn                 = 117,    /*!< HASH global interrupt                                             */
  PKA_IRQn                  = 118,    /*!< PKA global interrupt                                              */
  CEC_IRQn                  = 119,    /*!< CEC-HDMI global interrupt                                         */
  TIM12_IRQn                = 120,    /*!< TIM12 global interrupt                                            */
  TIM13_IRQn                = 121,    /*!< TIM13 global interrupt                                            */
  TIM14_IRQn                = 122,    /*!< TIM14 global interrupt                                            */
  I3C1_EV_IRQn              = 123,    /*!< I3C1 event interrupt                                              */
  I3C1_ER_IRQn              = 124,    /*!< I3C1 error interrupt                                              */
  I2C4_EV_IRQn              = 125,    /*!< I2C4 event interrupt                                              */
  I2C4_ER_IRQn              = 126,    /*!< I2C4 error interrupt                                              */
  LPTIM3_IRQn               = 127,    /*!< LPTIM3 global interrupt                                           */
  LPTIM4_IRQn               = 128,    /*!< LPTIM4 global interrupt                                           */
  LPTIM5_IRQn               = 129,    /*!< LPTIM5 global interrupt                                           */
  LPTIM6_IRQn               = 130,    /*!< LPTIM6 global interrupt                                           */
} IRQn_Type;


// TODO -- Remove hardwire
#define SMPS       /*!< Switched mode power supply feature */

/* --------  Configuration of the Cortex-M33 Processor and Core Peripherals  ------ */
#define __CM33_REV                0x0000U   /* Core revision r0p1 */
#define __SAUREGION_PRESENT       1U        /* SAU regions present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          4U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1U        /* FPU present */
#define __DSP_PRESENT             1U        /* DSP extension present */

#define __STATIC_INLINE static inline 

extern "C" {
  //#define MPU_Type void
#include "core_cm33.h"
#undef MPU
};
