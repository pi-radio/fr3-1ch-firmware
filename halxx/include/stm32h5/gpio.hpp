#pragma once

#include <stm32h5/memory_map.hpp>

namespace gpio
{
  enum class pinmode {
    INPUT = 0,
    OUTPUT = 1,
    ALTERNATE = 2,
    ANALOG = 3
  };

  enum class pinspeed {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
    VERY_HIGH = 3
  };

  enum class pinpull {
    NONE = 0,
    UP = 1,
    DOWN = 2
  };

  
  class GPIOBlock
  {
  public:


    class GPIOPin
    {
      GPIOBlock *block;
      int pin_no;
    public:
      
      GPIOPin(GPIOBlock *, int);

      void enable();
      
      bool read();
      void set(bool);

      void set_open_drain(bool);
      void set_pull(pinpull);

      void set_speed(pinspeed);
      void set_mode(pinmode);
      
      void set_alternate_function(int);
    };

  protected:
    volatile CSR::gpio *csr;
    int enable_bit;
    
  public:
    GPIOBlock(volatile CSR::gpio *_csr, int _enable_bit);

    GPIOPin operator[](int n) { return GPIOPin(this, n); }

    void enable_clock(bool);
  };

  extern GPIOBlock GPIOA;
  extern GPIOBlock GPIOB;
  extern GPIOBlock GPIOC;
  extern GPIOBlock GPIOD;
  extern GPIOBlock GPIOE;
  extern GPIOBlock GPIOF;
  extern GPIOBlock GPIOG;
  extern GPIOBlock GPIOH;
  extern GPIOBlock GPIOI;
};
