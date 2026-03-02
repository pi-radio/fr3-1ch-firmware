#pragma once

#include <tx_api.h>

namespace TXX
{
  class lock_intr
  {
    bool restored;
    UINT save;
  public:
    lock_intr() { save = __disable_interrupts(); restored = false; }
    ~lock_intr() { restore(); }
    void restore() { if (!restored) { restored = true; __restore_interrupt(save); } }
  };
}
