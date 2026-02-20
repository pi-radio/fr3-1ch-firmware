/*
 * txx.cpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */
#include "app_threadx.h"
#include "stm32h5xx_hal.h"
#include "app_azure_rtos_config.h"

#include "app_usbx_device.h"
#include "usbpd.h"
#if 0
#endif

#include <stdint.h>

#include <txx.hpp>

TXX::ThreadXApp *TXX::ThreadXApp::app = NULL;

TXX::ThreadXApp::ThreadXApp() : app_pool("Application Pool") {
  app = this;
}

void TXX::ThreadXApp::_start(void *bss)
{
  pbss = bss;

  try {
    app_pool.create();
  } catch(...) {
    error_handler(__FILE__, __LINE__);
  }

  start_app();
}

void TXX::ThreadXApp::start()
{
  tx_kernel_enter();
}


static uint8_t tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE] __attribute__((aligned(4)));
static TX_BYTE_POOL tx_app_byte_pool;

static uint8_t ux_device_byte_pool_buffer[UX_DEVICE_APP_MEM_POOL_SIZE] __attribute__((aligned(4)));
static TX_BYTE_POOL ux_device_app_byte_pool;

static uint8_t usbpd_byte_pool_buffer[TX_APP_MEM_POOL_SIZE] __attribute__((aligned(4)));
static TX_BYTE_POOL usbpd_app_byte_pool;

void tx_application_define(void *first_unused_memory)
{
  TXX::ThreadXApp::app->_start(first_unused_memory);
}

