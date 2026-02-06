/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ADC
  * @brief   ADC USBPD Config C file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *  ******************************************************************************
  */

#include "usbpd_ADC.h"

void ADC_Start(void)
{
#if defined ADC_DIFF_CAPABLE

  HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);

#else

  HAL_ADCEx_Calibration_Start(&hadc1);

#endif /* ADC_DIFF_CAPABLE */

  HAL_ADC_Start(&hadc1);
}
/**
  *
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
