#pragma once

#include <cstdint>
#include <string>

#include <tx_api.h>

#include <threadxx/object.hpp>

namespace TXX
{
  class Semaphore : public TXX::object
  {
    TX_SEMAPHORE sema;

    void create() { tx_semaphore_create(&sema, (char *)name.c_str(), 0); }

    friend class creator<Semaphore>;
    static constexpr creator<Semaphore> c = {};

  public:
    Semaphore(const std::string &_name) : object(_name, c) {}

    uint32_t get_suspended_count() { return sema.tx_semaphore_suspended_count; }

    uint32_t get() { return tx_semaphore_get(&sema, TX_WAIT_FOREVER); }
    uint32_t get(uint32_t timeout) { return tx_semaphore_get(&sema, timeout); }
    void put() { tx_semaphore_put(&sema); }
  };
}
