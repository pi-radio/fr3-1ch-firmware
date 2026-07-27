/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"


void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  //__HAL_RCC_GPIOH_CLK_ENABLE();
  //__HAL_RCC_GPIOG_CLK_ENABLE();

#ifndef OCTOLO
  // This is the FR3 1-ch

  // Configure Bank A
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_RESET);


  // Configure Bank B
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  // Configure Bank C
  GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);

  // Configure Bank D
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);

  // Configure Bank E
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_SET);


  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

  // 1. Start the power-up sequence
  	  // a. In-rush Enable
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);
  	  //tx_thread_sleep(100);

  	  // b. Enable BUCK_5V3
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
  	  //tx_thread_sleep(100);

  	  // c. Enable BUCK_3V7
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
  	  //tx_thread_sleep(100);

  // 2. Get the LO Working
  	  // a. LO_CTRL_3V3. 0: Internal LMX. 1: External from SMA
  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);

  	  // b. Enable the TCXO.
  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

  	  // c. Select the Clock source.  0: External. 1: On-board
  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);

  // 3. Configure the TX channel
  	  // a. Set IQ Swap 0: Upper side-band. 1: Lower side-band
  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);

  	  // b. Set the filter. Bypass. LLXXXX. Configure later through Serial.
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
#else
  	  // This is the OCTOLO
  	// Configure Bank A
  	  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
  	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	  GPIO_InitStruct.Pull = GPIO_NOPULL;
  	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_RESET);


  	  // Configure Bank B
  	  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	  GPIO_InitStruct.Pull = GPIO_NOPULL;
  	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  	  // Configure Bank C
  	  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	  GPIO_InitStruct.Pull = GPIO_NOPULL;
  	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);

  	  // Configure Bank D
  	  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	  GPIO_InitStruct.Pull = GPIO_NOPULL;
  	  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  	  // Configure Bank E
  	  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_9 | GPIO_PIN_13;
  	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	  GPIO_InitStruct.Pull = GPIO_NOPULL;
  	  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_9 | GPIO_PIN_13, GPIO_PIN_RESET);


  	  // 1. In-rush Enable
  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
  	  HAL_Delay(500);

  	  // 2. 5V3 Buck Enable
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
  	  HAL_Delay(500);

  	  // 3. 3V7 Buck Enable
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
  	  HAL_Delay(500);

  	  // 4. OCXO LDO Enable
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
  	  HAL_Delay(10000);

  	  // 5. LDO Enable: LMX
  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
  	  HAL_Delay(1500);

  	  // 6. LDO Enable: Amp 1
  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
  	  HAL_Delay(500);

  	  // 7. LDO Enable: Amp 2
  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  	  HAL_Delay(500);

  	  HAL_Delay(500); // Give the OCXO 10s to warm up, before the LMX draws a surge of current.



#endif
}

