#pragma once

#include <stdint.h>

namespace CSR
{
  template <typename T, uint32_t base>
  struct csr_ptr
  {
    static inline volatile T *ptr() { return (volatile T *)base; };
    volatile T &operator *() noexcept { return *(T *)base; }
    volatile T *operator ->() noexcept { return (T *)base; }

    operator volatile T *() noexcept { return (T *)base; }
  };

  template <uint32_t sbit, uint32_t len>
  struct field
  {
    static constexpr uint32_t mask = (((1 << (len + 1)) - 1) << sbit);
    volatile uint32_t &r;
    
    field(volatile uint32_t &_r) : r(_r)
    {
    }

    uint32_t get() {
      return (r & mask) >> sbit;
    }

    void set(uint32_t v) {
      r = (r & ~mask) | ((v << sbit) & mask);
    }
  };

};
