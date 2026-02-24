#pragma once

namespace CSR
{
  template <typename T, uint32_t base>
  struct csr_ptr
  {
    volatile T &operator *() const noexcept { return *(T *)base; }
    volatile T *operator ->() const noexcept { return *(T *)base; }
  };
};
