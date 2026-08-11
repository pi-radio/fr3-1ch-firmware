#pragma once

#include <string>

#include <tx_api.h>

namespace TXX
{
  class Semaphore
  {
    TX_SEMAPHORE sema;
    std::string name;
  public:
    Semaphore(const std::string &_name) : name(_name) {}

    void create() { tx_semaphore_create(&sema, (char *)name.c_str(), 0); }

    void get() { tx_semaphore_get(&sema, TX_WAIT_FOREVER); }
    void put() { tx_semaphore_put(&sema); }
  };
}
