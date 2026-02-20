/*
 * usbxx.hpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */

#ifndef USBXX_HPP_
#define USBXX_HPP_

#include <txx.hpp>
#include <app_usbx_device.h>

class USBXX
{
  TXX::StaticBytePool<UX_DEVICE_APP_MEM_POOL_SIZE> pool;

public:
  USBXX() : pool("USBX Pool") {};
  void start() {
    MX_USBX_Device_Init((void *)(TX_BYTE_POOL *)pool);
  }
};



#endif /* USBXX_HPP_ */
