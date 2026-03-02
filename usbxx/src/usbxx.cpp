/*
 * usbxx.cpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */
#include <stdexcept>

#include <usbxx/usbxx.hpp>

#include <ux_api.h>

#include <usb.h>

USBXX::SystemBase::SystemBase()
{
}

void USBXX::SystemBase::start_system(uint8_t *stack, size_t n)
{
    if (ux_system_initialize(stack, n, UX_NULL, 0) != UX_SUCCESS)
    {
      throw std::runtime_error("Unable to start USBX system");
    }
}



#if 0
  class System
  {
    uint8_t _system_stack[system_stack_size];

  public:

  template <size_t system_stack_size, size_t app_stack_size>
  class Device
  {
    class AppThread : TXX::Thread<app_stack_size> {
      AppThread() : TXX::Thread<app_stack_size>("USB Device Thread") {}
      void main() override;
    };

    System<system_stack_size> system;
    AppThread app_thread;


    friend void _usbxx_change_notification();
  public:
    Device();

    void start();
  };

void TXX::USBXXBase::start_system(uint8_t *stack, int stack_size)
{
}
#endif
