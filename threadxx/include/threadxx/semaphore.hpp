#pragma once

#include <string>

#include <tx_api.h>

#include <threadxx/object.hpp>

namespace TXX
{
  class Semaphore : object
  {
    TX_SEMAPHORE sema;
    std::string name;

    void create() override { tx_semaphore_create(&sema, (char *)name.c_str(), 0); }

  public:
    Semaphore(const std::string &_name) : object(_name) {}


    void get() { tx_semaphore_get(&sema, TX_WAIT_FOREVER); }
    void put() { tx_semaphore_put(&sema); }
  };
}
