#pragma once

#include <string>

#include <tx_api.h>
#include <threadxx/object.hpp>

namespace TXX
{
  class Mutex : public object
  {
    TX_MUTEX mutex;

    void create() { tx_mutex_create(&mutex, (char *)name.c_str(), TX_NO_INHERIT); }

  public:
    Mutex(const std::string &_name) : object(_name) {}

    void get() { tx_mutex_get(&mutex, TX_WAIT_FOREVER); }
    void put() { tx_mutex_put(&mutex); }

    struct guard
    {
      Mutex &mtx;
      
      guard(Mutex &_mtx) : mtx(_mtx) { mtx.get(); }
      ~guard() { mtx.put(); }
    };
  };
}
