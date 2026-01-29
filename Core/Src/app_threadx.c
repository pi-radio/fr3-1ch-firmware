/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <console.h>
#include <terminal.h>
#include <lmx.h>
#include <config_data.h>

#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
#define APP_POOL_SIZE  2048

static UCHAR app_pool_data[APP_POOL_SIZE];
static TX_BYTE_POOL app_pool;
static TX_THREAD app_thread;

TX_EVENT_FLAGS_GROUP app_events;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
VOID app_thread_entry(ULONG _a);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  UCHAR *pStack;

  tx_byte_pool_create(&app_pool, "application pool", app_pool_data, APP_POOL_SIZE);

  tx_byte_allocate(&app_pool, (VOID **)&pStack, 1024, TX_NO_WAIT);

  tx_thread_create(&app_thread, "app_thread", app_thread_entry, 1, pStack, 1024, 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

  tx_event_flags_create(&app_events, "app_events");

  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */

  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */

  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */

  /* USER CODE END Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
VOID app_thread_entry(ULONG _a)
{
  printf("FR3 1ch starting...\n");

  printf("Programming LMX...\n");

  //lmx_program();

  while(1) {
    tx_thread_sleep(1000);
  }
}

/* USER CODE END 1 */
