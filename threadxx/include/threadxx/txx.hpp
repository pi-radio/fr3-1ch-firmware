/*
 * txx.hpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */

#ifndef TXX_HPP_
#define TXX_HPP_

#include <threadxx/bytepool.hpp>
#include <threadxx/thread.hpp>
#include <threadxx/eventflags.hpp>

#include "txx_config.h"

namespace TXX {
  class ThreadXApp
  {
    friend void ::tx_application_define(void *);

    StaticBytePool<TX_APP_MEM_POOL_SIZE> app_pool;

    void *pbss;

    void _start(void *);

  protected:
    virtual void start_app(void) = 0;
    virtual void error_handler(const char *file, int line) = 0;

  public:
    static ThreadXApp *app;

    ThreadXApp();

    void start();
  };
};


#endif /* TXX_HPP_ */
