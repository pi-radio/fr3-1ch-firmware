/*
 * txxbytepool.hpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */

#ifndef TXXBYTEPOOL_HPP_
#define TXXBYTEPOOL_HPP_

#include <string>
#include <stdexcept>
#include <stdint.h>

#include "tx_api.h"

namespace TXX {
  template <int N>
  class StaticBytePool
  {
    const std::string &_name;
    uint8_t _buffer[N] __attribute__((aligned(4)));
    TX_BYTE_POOL _byte_pool;

  public:
    StaticBytePool(const std::string &name) : _name(name) {

    }

    void create() {
      auto result = tx_byte_pool_create(&_byte_pool, (char *)_name.c_str(), _buffer, N);

      if (result != TX_SUCCESS) {
        throw std::runtime_error("tx_byte_pool_create failed");
      }
    }

    operator TX_BYTE_POOL*() { return &_byte_pool; }

    void *allocate(uint32_t size) {
      void *retval;
      uint32_t result = tx_byte_allocate(*this, &retval, size, TX_WAIT_FOREVER);

      if (result != TX_SUCCESS) {
        throw std::runtime_error("Byte pool allocate failed");
      }

      return retval;
    }
  };
};

#endif /* TXXBYTEPOOL_HPP_ */
