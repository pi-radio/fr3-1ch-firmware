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
#include <usart.h>
#include <app_threadx.h>
#include <tx_trace.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

static UCHAR app_pool_stack[APP_POOL_SIZE];
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
  tx_thread_create(&app_thread, "app_thread", app_thread_entry, 1, app_pool_stack, sizeof(app_pool_stack), 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

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
#if DUMP_TRACEX
static TX_TRACE_BUFFER_ENTRY *last_ptr;
#endif

#define FAULTPRINT(...) do {\
  printf(__VA_ARGS__); \
  dbgprint(__VA_ARGS__); \
} while(0)

VOID app_thread_entry(ULONG _a)
{
#if DUMP_TRACEX
  int n;
  char buf[128];
  last_ptr  = _tx_trace_buffer_start_ptr;
#endif

  printf("FR3 1ch starting...\n");

  exception_info_t *einfo = get_exception_info();

  if (einfo->exception_type == EXCEPTION_ERROR_HANDLER) {
    einfo->exception_type = 0;
    dbgprint("Error_Handler called: %s: %d\r\n", (const char *)einfo->excSP, einfo->excLR);
  } else if (einfo->exception_type) {
    int display = 1;

    switch(einfo->exception_type) {
    case EXCEPTION_HARD_FAULT:
      FAULTPRINT("Hard Fault:");

      if (einfo->HFSR & (1 << 31)) FAULTPRINT(" DEBUGEVT");
      if (einfo->HFSR & (1 << 30)) FAULTPRINT(" FORCED");
      if (einfo->HFSR & (1 << 1)) FAULTPRINT(" VECTTBL");

      FAULTPRINT("\r\n");

      break;
    case EXCEPTION_DEBUG:
      FAULTPRINT("Debug Fault:\r\n");
      break;
    case EXCEPTION_NMI:
      FAULTPRINT("NMI:\r\n");
      break;
    case EXCEPTION_MEM_MANAGE:
      FAULTPRINT("Memory Management Fault:\r\n");
      break;
    case EXCEPTION_BUS_FAULT:
      FAULTPRINT("Bus Fault:\r\n");
      break;
    case EXCEPTION_USAGE_FAULT:
      FAULTPRINT("Usage Fault:\r\n");
      break;
    default:
      display = 0;
      break;
    }

    if (display) {
      FAULTPRINT("CONTROL: %08lx", einfo->CONTROL);

      if (einfo->CONTROL & (1 << 3)) FAULTPRINT(" SFPA");
      if (einfo->CONTROL & (1 << 2)) FAULTPRINT(" FPCA");

      if (einfo->CONTROL & (1 << 1))
        FAULTPRINT(" PSP");
      else
        FAULTPRINT(" MSP");

      if (einfo->CONTROL & (1 << 0))
        FAULTPRINT(" UNPRIV");
      else
        FAULTPRINT(" PRIV");

      FAULTPRINT("\r\n");

      FAULTPRINT("CFSR: %08lx  |  UFSR: %04lx BFSR: %02lx MMFSR: %02lx\r\n",
          einfo->CFSR, (einfo->CFSR >> 16) & 0xFFFF, (einfo->CFSR >> 8) & 0xFF, einfo->CFSR & 0xFF);

      FAULTPRINT("Exception SP: %08lx LR: %08lx\r\n", einfo->excSP, einfo->excLR);

      FAULTPRINT("S(%d) DCRS(%d) FType(%d) Mode (%d) SPSEL(%d) ES(%d)\r\n",
          (einfo->excLR >> 6) & 1,
          (einfo->excLR >> 5) & 1,
          (einfo->excLR >> 4) & 1,
          (einfo->excLR >> 3) & 1,
          (einfo->excLR >> 2) & 1,
          (einfo->excLR >> 0) & 1);

      FAULTPRINT("SHCSR: %08lx VTOR: %08lx\r\n", einfo->SHCSR, einfo->VTOR);
      FAULTPRINT("PC: %08lx LR: %08lx SP: %08lx xPSR: %08lx\r\n", einfo->base_ctx.PC, einfo->base_ctx.LR, einfo->SP, einfo->base_ctx.xPSR);
      FAULTPRINT("R0: %08lx R1: %08lx R2: %08lx R3: %08lx R12: %08lx\r\n", einfo->base_ctx.R0, einfo->base_ctx.R1, einfo->base_ctx.R2, einfo->base_ctx.R3, einfo->base_ctx.R12);
    }

    clear_exception();
  }

  tx_thread_sleep(1000);

  printf("Programming LMX...\n");
  lmx_program();

  while(1) {
    tx_thread_sleep(1000);

#if DUMP_TRACEX
    if (_tx_trace_buffer_current_ptr < last_ptr) {
      last_ptr = _tx_trace_buffer_start_ptr;
    }

    if (_tx_trace_buffer_current_ptr != last_ptr) {
      while (last_ptr < _tx_trace_buffer_current_ptr) {
        n = snprintf(buf, sizeof(buf), "%08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\r\n",
            last_ptr->tx_trace_buffer_entry_thread_pointer,
            last_ptr->tx_trace_buffer_entry_thread_priority,
            last_ptr->tx_trace_buffer_entry_event_id,
            last_ptr->tx_trace_buffer_entry_time_stamp,
            last_ptr->tx_trace_buffer_entry_information_field_1,
            last_ptr->tx_trace_buffer_entry_information_field_2,
            last_ptr->tx_trace_buffer_entry_information_field_3,
            last_ptr->tx_trace_buffer_entry_information_field_4);

        HAL_UART_Transmit(&huart1, (uint8_t *)buf, n, 0xFFFF);
      }
    }
#endif
  }
}

/* USER CODE END 1 */
