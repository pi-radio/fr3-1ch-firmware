#include <main.h>

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

#include <iostream>
#include <dbgstream.hpp>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <main.h>
#include <config_data.h>

#include <stdio.h>

#include <txx.hpp>

#include <console.h>
#include <terminal.hpp>
#include <fr3_1ch_hw.h>

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
extern "C" VOID app_thread_entry(ULONG _a);
/* USER CODE END PFP */

#define FAULTPRINT(...) do {\
  printf(__VA_ARGS__); \
  dbgprint(__VA_ARGS__); \
} while(0)

TXPirApp::TXPirApp() : app_thread()
{

}

void TXPirApp::start_app(void)
{
  usbxx.start();
  usbpd.start();

  tx_event_flags_create(&app_events, "app_events");

  app_thread.create();
}

void TXPirApp::error_handler(const char *file, int line)
{
  dbgout << "FATAL ERROR: " << file << ":" << line << std::endl;
  while(1);
}

AppThread::AppThread() : Thread<APP_THREAD_STACK_SIZE>("Application Thread")
{
}

void AppThread::main()
{
  terminal_init();

  dbg_ready = 1;

  console_init();

  fr3_1ch_hw_init();

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

      FAULTPRINT("S(%ld) DCRS(%ld) FType(%ld) Mode (%ld) SPSEL(%ld) ES(%ld)\r\n",
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
  main_app.get_lmx().program();

  while(1) {
    tx_thread_sleep(1000);
  }
}

TXPirApp app;
