/*
 * txx.cpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */
#include "app_threadx.h"
#include "stm32h5xx_hal.h"

#include "app_usbx_device.h"
#include "usbpd.h"

#include <stdint.h>

#include <threadxx/txx.hpp>

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

void tx_application_define(void *first_unused_memory)
{
  TXX::ThreadXApp::app->_start(first_unused_memory);
}

