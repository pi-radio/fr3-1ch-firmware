#pragma once

#include <stdexcept>

#include <halxx/pinout.hpp>
#include <halxx/memory_map.hpp>
#include <halxx/clocking.hpp>

namespace usart
{
  enum class wordlength {
    EIGHT = 0,
    SEVEN = (1 << 28) // Bit in CR1
  };

  enum class stopbits {
    ONE,
    ONE_POINT_FIVE,
    TWO
  };

  enum class parity {
    NONE,
    EVEN,
    ODD
  };

  enum class pinmode {
    TX,
    RX,
    TX_RX
  };
  
  enum class flow_control {
    NONE,
    HW
  };

  class USARTBase
  {
  protected:
    volatile CSR::usart *csr;
    void (*set_clk_sel)(uint32_t v);
    void (*set_clk_en)(bool b);
    uint32_t br;
    wordlength wl;
    stopbits sb;
    pinmode mode;
    flow_control fc;

    
  public:
    USARTBase(volatile CSR::usart *_csr,
              void (*_set_clk_sel)(uint32_t v),
              void (*_set_clk_en)(bool b)) :
      csr(_csr),
      set_clk_sel(_set_clk_sel),
      set_clk_en(_set_clk_en)
    {
    }

    void init(uint32_t _baud_rate,
              wordlength _wl = wordlength::EIGHT,
              stopbits _sb = stopbits::ONE,
              pinmode _mode = pinmode::TX_RX,
              flow_control _fc = flow_control::NONE,
              uint32_t oversampling = 16,
              bool one_bit_sampling = false,
              uint32_t divider = 1,
              bool advanced = false);

    void _enable();
    void _disable();
  
    parity get_parity() const;
    void set_parity(parity);
    
    wordlength get_wordlength() const;
    void set_wordlength(wordlength);
    
    stopbits get_stopbits() const;
    void set_parity(stopbits);
    
    flow_control get_hw_flow_control();
    void set_hw_flow_control(flow_control);
  };
  
  template <int n>
  class USART : public USARTBase
  {
  protected:
    volatile CSR::usart *csr;
    
  public:
    USART() : USARTBase(std::get<n>(CSR::periph_nsec.usarts).ptr(),
                        clocking::usart_clk_sel<n>::set,
                        std::get<n>(clocking::enables::usart).set) {}
  };

  extern USART<1> usart1;
};


