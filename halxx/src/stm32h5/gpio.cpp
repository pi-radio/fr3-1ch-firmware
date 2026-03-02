#include <stm32h5/gpio.hpp>

using namespace gpio;

GPIOBlock GPIOA(CSR::periph_nsec.gpioa.ptr(), 0);
GPIOBlock GPIOB(CSR::periph_nsec.gpiob.ptr(), 1);
GPIOBlock GPIOC(CSR::periph_nsec.gpioc.ptr(), 2);
GPIOBlock GPIOD(CSR::periph_nsec.gpiod.ptr(), 3);
GPIOBlock GPIOE(CSR::periph_nsec.gpioe.ptr(), 4);
GPIOBlock GPIOF(CSR::periph_nsec.gpiof.ptr(), 5);
GPIOBlock GPIOG(CSR::periph_nsec.gpiog.ptr(), 6);
GPIOBlock GPIOH(CSR::periph_nsec.gpioh.ptr(), 7);
GPIOBlock GPIOI(CSR::periph_nsec.gpioi.ptr(), 8);


GPIOBlock::GPIOBlock(volatile CSR::gpio *_csr, int _enable_bit) : csr(_csr), enable_bit(_enable_bit)
{
}

void GPIOBlock::enable_clock(bool en)
{
  if(en) {
    CSR::periph_nsec.rcc.ptr()->AHB2ENR |= (1 << enable_bit);
  } else {
    CSR::periph_nsec.rcc.ptr()->AHB2ENR &= ~(1 << enable_bit);
  }
}


GPIOBlock::GPIOPin::GPIOPin(GPIOBlock *blk, int n) : block(blk), pin_no(n)
{
}

void GPIOBlock::GPIOPin::enable()
{
  block->enable_clock(true);
}


bool GPIOBlock::GPIOPin::read()
{
  if (block->csr->IDR & (1 << pin_no)) {
    return true;
  }

  return false;
}

void GPIOBlock::GPIOPin::set(bool b)
{
  if (b)
    block->csr->BSRR = (1 << pin_no);
  else
    block->csr->BRR = (1 << pin_no);
}

void GPIOBlock::GPIOPin::set_mode(pinmode mode)
{
  uint32_t sbit = pin_no * 2;
  uint32_t mask = 0x3 << sbit;

  block->csr->MODER = (block->csr->MODER & ~mask) | ((uint32_t)mode << sbit);
}

void GPIOBlock::GPIOPin::set_pull(pinpull pull)
{
  int sbit = pin_no * 2;
  uint32_t mask = 0x3 << sbit;

  block->csr->PUPDR = (block->csr->PUPDR & ~mask) | ((uint32_t)pull << sbit);
}

void GPIOBlock::GPIOPin::set_speed(pinspeed speed)
{
  int sbit = pin_no * 2;
  uint32_t mask = 0x3 << sbit;

  block->csr->OSPEEDR = (block->csr->OSPEEDR & ~mask) | ((uint32_t)speed << sbit);
}


void GPIOBlock::GPIOPin::set_open_drain(bool od)
{
  if (od) {
    block->csr->MODER |= (1 << pin_no);
  } else {
    block->csr->MODER &= ~(1 << pin_no);
  }
}

void GPIOBlock::GPIOPin::set_alternate_function(int af)
{
  int rn = 0, sbit = pin_no * 4;

  if (pin_no >= 8) {
    rn = 1;
    sbit -= 32;
  }

  uint32_t mask = 0xF << sbit;
  
  block->csr->AFR[rn] = (block->csr->AFR[rn] & ~mask) | ((af << sbit) & mask);

  set_mode(pinmode::ALTERNATE);
}
