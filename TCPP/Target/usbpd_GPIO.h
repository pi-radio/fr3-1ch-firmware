/**
  ******************************************************************************
  * @file    GPIO_CONF.h
  * @author  DFD Application Team
  * @brief   Header file for TCPP_Conf.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GPIO_CONF_H
#define GPIO_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_ll_bus.h"
#include "stm32h5xx_ll_gpio.h"
#include "stm32h5xx_ll_system.h"
#include "stm32h5xx_ll_adc.h"
#include "stm32h5xx_ll_dma.h"
#include "custom_bus.h"
#include "custom_errno.h"
#include "tcpp0203.h"

/* Exported Defines ----------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#define STM32H5XX

/* I2C -----------------------------------------------------------------------*/
#define TCPP0203_I2C_Init                           BSP_I2C4_Init
#define TCPP0203_I2C_DeInit                         BSP_I2C4_DeInit
#define TCPP0203_I2C_IsReady                        BSP_I2C4_IsReady
#define TCPP0203_I2C_WriteReg                       BSP_I2C4_WriteReg
#define TCPP0203_I2C_ReadReg                        BSP_I2C4_ReadReg
#define TCPP0203_I2C_WriteReg16                     BSP_I2C4_WriteReg16
#define TCPP0203_I2C_ReadReg16                      BSP_I2C4_ReadReg16
#define TCPP0203_I2C_Send                           BSP_I2C4_Send
#define TCPP0203_I2C_Recv                           BSP_I2C4_Recv
#define TCPP0203_I2C_SendRecv                       BSP_I2C4_SendRecv
#define TCPP0203_GetTick                            BSP_GetTick
#define BUS_I2C_POLL_TIMEOUT                        0x1000U

/* GPIO ----------------------------------------------------------------------*/
#define TCPP0203_PORT0_ENABLE_GPIO_PORT           GPIOG
#define TCPP0203_PORT0_ENABLE_GPIO_PIN            GPIO_PIN_0
#define TCPP0203_PORT0_ENABLE_GPIO_DEFVALUE()     HAL_GPIO_WritePin(TCPP0203_PORT0_ENABLE_GPIO_PORT,TCPP0203_PORT0_ENABLE_GPIO_PIN,GPIO_PIN_RESET);
#define TCPP0203_PORT0_ENABLE_GPIO_SET()          HAL_GPIO_WritePin(TCPP0203_PORT0_ENABLE_GPIO_PORT,TCPP0203_PORT0_ENABLE_GPIO_PIN,GPIO_PIN_SET);
#define TCPP0203_PORT0_ENABLE_GPIO_RESET()        HAL_GPIO_WritePin(TCPP0203_PORT0_ENABLE_GPIO_PORT,TCPP0203_PORT0_ENABLE_GPIO_PIN,GPIO_PIN_RESET);

#define TCPP0203_PORT0_FLG_GPIO_PORT                GPIOG
#define TCPP0203_PORT0_FLG_GPIO_PIN                 GPIO_PIN_1
#define TCPP0203_PORT0_FLG_EXTI_IRQN                EXTI2_IRQn
#define TCPP0203_PORT0_FLG_EXTI_LINE                2
#define TCPP0203_PORT0_FLG_EXTI_IRQHANDLER          EXTI2_IRQn_IRQHandler

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif
#endif /* GPIO_CONF_H */
