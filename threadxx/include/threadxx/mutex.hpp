#pragma once

#include <string>

#include <tx_api.h>

namespace TXX
{
  class Mutex
  {
    TX_MUTEX mutex;
    const std::string &name;
  public:
    Mutex(const std::string &_name) : name(_name) {}
    
    void create() { tx_mutex_create(&mutex, (char *)name.c_str(), TX_NO_INHERIT); }

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
