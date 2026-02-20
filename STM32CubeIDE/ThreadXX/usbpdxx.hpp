/*
 * usbpdxx.hpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */

#ifndef USBPDXX_HPP_
#define USBPDXX_HPP_

#include <txx.hpp>
#include <usbpd.h>

class USBPD
{
  TXX::StaticBytePool<USBPD_DEVICE_APP_MEM_POOL_SIZE> pool;

public:
  USBPD() : pool("USBPD Pool") {};
  void start() {
    MX_USBPD_Init((void *)(TX_BYTE_POOL *)pool);
  }
};

#endif /* USBPDXX_HPP_ */
