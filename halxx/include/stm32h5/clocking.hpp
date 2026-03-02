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

  static volatile CSR::rcc *rcc_csr = CSR::periph_nsec.rcc.ptr();

  template <uint32_t CSR::rcc::* reg, int bit>
  struct clk_en
  {
    static void set(bool b)
    {
      if (b) {
        rcc_csr->*reg |= (1 << bit);
      } else {
        rcc_csr->*reg &= ~(1 << bit);
      }
    }
  };

  namespace enables {
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 0> gpdma1;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 1> gpdma2;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 8> flash;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 12> crc;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 14> cordic;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 15> fmac;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 17> ramcfg;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 19> eth;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 20> eth_tx;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 21> eth_rx;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 24> tzsc;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 28> bkpram;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 30> dcache;
    static constexpr clk_en<&CSR::rcc::AHB1ENR, 21> sram1;


    static constexpr clk_en<&CSR::rcc::APB1LENR, 0> tim2;
    
    static constexpr clk_en<&CSR::rcc::APB2ENR, 14> usart1;

    static auto usart = std::make_tuple(0, usart1);
  };
  
  template <uint32_t n>
  struct usart_clk_sel
  {
    static inline volatile uint32_t &reg() { return (n <= 10) ? CSR::periph_nsec.rcc.ptr()->CCIPR1 : CSR::periph_nsec.rcc.ptr()->CCIPR2; };
    static constexpr uint32_t sbit = (n <= 10) ? ((n-1)*3) : ((n-11)*3);
    
    static uint32_t get()
    {
      return CSR::field<sbit, 3>(reg()).get();
    }

    static void set(uint32_t v)
    {
      return CSR::field<sbit, 3>(reg()).set(v);
    }
  };

  enum class systick_src {
    HCLK_DIV8 = 0x0,
    LSI = 0x1,
    LSE = 0x2,
    HCLK = 0x4
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
