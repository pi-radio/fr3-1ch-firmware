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
    UINT   status;
    UINT   controller_type;
    UINT   otg_capabilities;
    UINT   irq;
    ULONG  io;
    ULONG  device_address;
    void   *controller_hardware;

    void   function(UINT, VOID*);
  };
#endif

  class SystemBase
  {
#if 0
    UX_SLAVE_DCD    dcd;
    UX_SLAVE_DEVICE device;
    UCHAR           *device_framework;
    ULONG           device_framework_length;
    UCHAR           *device_framework_full_speed;
    ULONG           device_framework_length_full_speed;
    UCHAR           *device_framework_high_speed;
    ULONG           device_framework_length_high_speed;
    UCHAR           *string_framework;
    ULONG           string_framework_length;
    UCHAR           *language_id_framework;
    ULONG           language_id_framework_length;
    UCHAR           *dfu_framework;
    ULONG           dfu_framework_length;
  #if UX_MAX_SLAVE_CLASS_DRIVER > 1
    UINT            max_class;
  #endif
    UX_SLAVE_CLASS  *class_array;
    UX_SLAVE_CLASS  *interface_class_array[UX_MAX_SLAVE_INTERFACES];
    ULONG           speed;
    ULONG           power_state;
    ULONG           remote_wakeup_capability;
    ULONG           remote_wakeup_enabled;
    ULONG           device_dfu_capabilities;
    ULONG           device_dfu_detach_timeout;
    ULONG           device_dfu_transfer_size;
    ULONG           device_dfu_state_machine;
    ULONG           device_dfu_mode;
    UINT            (*change_function) (ULONG);
    ULONG           device_vendor_request;
    UINT            (*device_vendor_request_function) (ULONG, ULONG, ULONG, ULONG, UCHAR *, ULONG *);
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
