/*
 * usbxxsystem.hpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */

#ifndef USBXX_USBXXSYSTEM_HPP_
#define USBXX_USBXXSYSTEM_HPP_

#include <threadxx/txx.hpp>

namespace USBXX {
  class SystemBase
  {
  public:
    SystemBase();

    void start_system(uint8_t *stack, size_t n);
  };

  template <size_t system_stack_size>
  class System : public SystemBase
  {
    uint8_t _system_stack[system_stack_size];

  public:
    System() { }

    void start() { start_system(_system_stack, system_stack_size); }
  };
}


#endif /* USBXX_USBXXSYSTEM_HPP_ */
