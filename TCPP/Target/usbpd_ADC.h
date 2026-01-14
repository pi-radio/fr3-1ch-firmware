/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ADC
  * @brief   ADC USBPD Config H file
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

#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx.h"
#include "stm32h5xx_hal.h"

#define ADC_DIFF_CAPABLE

/* ADC Vbus is defined */
#define	VISENSE_ADC_INSTANCE                        ADC1
#define ADC_VBUS_CHANNEL                            ADC_CHANNEL_1
extern ADC_HandleTypeDef                            hadc1;

/* ADC Isense not used */
#define ADC_ISENSE_USE                              0

/* ADC Vprov not used */
#define ADC_VPROV_USE                               0

/* ADC Vcons not used */
#define ADC_VCONS_USE                               0

#define VISENSE_ADC_BUFFER_SIZE                     1
#define ADC_VBUS_ONLY

/* ADC_Buffer values */
extern uint16_t usbpd_pwr_adcx_buff[];

void ADC_Start(void);

/**
  * @}
  */

/**
  * @}
  */

#endif /* ADC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
