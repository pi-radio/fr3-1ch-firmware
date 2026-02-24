#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <stm32h5/memory_map.hpp>

namespace clocking
{
  class clock {
  public:
    virtual int f() = 0;
  };

  typedef std::vector<clock *> clock_list_t;


  class ext_clk : public clock {
    int _f;
  public:
    ext_clk(int f) : _f(f) { }
    virtual int f() { return _f; }
  };

  template <int _f>
  class static_clock : public clock {
  public:
    int f() override { return _f; };
  };

  class rcc_clock : public clock {
  public:
    virtual void commit() = 0;
    virtual bool on() = 0;
    virtual bool ready() = 0;
  };


  class hsi_clock : public rcc_clock {
  public:
    void commit() override;
    bool on() override;
    bool ready() override;

    uint32_t divider(void);
    void set_divider(uint32_t n);
  };

  class csi_clock : public rcc_clock {
  public:
    void commit() override;
    bool on() override;
    bool ready() override;
  };

  class pll : public rcc_clock {
    void commit() override;
    bool on() override;
    bool ready() override;
  };

  class clock_mux : public clock {
    int i;
    clock_list_t clocks;

  public:
    clock_mux(auto &&... args) : i(0)
    {
      for (auto clk : std::initializer_list<clock *>{ args... }) {
        clocks.push_back(clk);
      }
    }

    int f() override { return clocks[i]->f(); };
  };



  class _sysclk : public clock_mux {
  public:
    _sysclk(auto &&... args) : clock_mux(args...) {}
  };

  class system
  {
  public:
    _sysclk sysclk;

    ext_clk hse;
    static_clock<64000000> hsi;
    static_clock<4000000>  csi;
    static_clock<48000000> hsi48;

    system(int fhse = 0);
  };

};
