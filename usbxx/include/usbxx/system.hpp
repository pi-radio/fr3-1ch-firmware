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
#if 0
  class SystemDCD
  {
    uint32_t   status;
    uint32_t   controller_type;
    uint32_t   otg_capabilities;
    uint32_t   irq;
    uint32_t  io;
    uint32_t  device_address;
    void   *controller_hardware;

    void   function(uint32_t, VOID*);
  };
#endif

  class SystemBase
  {
#if 0
    UX_SLAVE_DCD    dcd;
    UX_SLAVE_DEVICE device;
    uint8_t           *device_framework;
    uint32_t           device_framework_length;
    uint8_t           *device_framework_full_speed;
    uint32_t           device_framework_length_full_speed;
    uint8_t           *device_framework_high_speed;
    uint32_t           device_framework_length_high_speed;
    uint8_t           *string_framework;
    uint32_t           string_framework_length;
    uint8_t           *language_id_framework;
    uint32_t           language_id_framework_length;
    uint8_t           *dfu_framework;
    uint32_t           dfu_framework_length;
  #if UX_MAX_SLAVE_CLASS_DRIVER > 1
    uint32_t            max_class;
  #endif
    USBClass  *class_array;
    USBClass  *interface_class_array[UX_MAX_SLAVE_INTERFACES];
    uint32_t           speed;
    uint32_t           power_state;
    uint32_t           remote_wakeup_capability;
    uint32_t           remote_wakeup_enabled;
    uint32_t           device_dfu_capabilities;
    uint32_t           device_dfu_detach_timeout;
    uint32_t           device_dfu_transfer_size;
    uint32_t           device_dfu_state_machine;
    uint32_t           device_dfu_mode;
    uint32_t            (*change_function) (uint32_t);
    uint32_t           device_vendor_request;
    uint32_t            (*device_vendor_request_function) (uint32_t, uint32_t, uint32_t, uint32_t, uint8_t *, uint32_t *);
#endif


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
