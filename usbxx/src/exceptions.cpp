#include <usbxx/exceptions.hpp>

using namespace USBXX;

extern "C" {
#include "stm32h573xx.h"
}

runtime_error::runtime_error(const std::string &_what) : std::runtime_error(_what)
{
  auto dbgmcu = ((DBGMCU_TypeDef *) (0x44024000UL));

  if (dbgmcu->CR & 0x7)
    __asm volatile ("BKPT     %0" : : "i"(0));
}
