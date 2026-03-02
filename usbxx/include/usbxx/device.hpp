/*
 * usbxxdevice.hpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */

#ifndef USBXX_USBXXDEVICE_HPP_
#define USBXX_USBXXDEVICE_HPP_

#include <usbxx/system.hpp>
#include <usbxx/descriptor.hpp>

#include <ux_api.h>

namespace USBXX
{
  extern UINT _usbxx_change_notification(ULONG);

  class DeviceBase
  {
    friend UINT _usbxx_change_notification(ULONG);

    uint32_t on_change(uint32_t);
  protected:
    void thread_entry();

  public:
    DeviceBase();

    virtual void start_system() {}
    virtual void setup_device();
    virtual void class_init() {}
    virtual void register_class() {}
    virtual void start_app() {}

    virtual uint32_t on_attached() { return UX_SUCCESS; }
    virtual uint32_t on_removed() { return UX_SUCCESS; }

    virtual uint32_t on_connected() { return UX_SUCCESS; }
    virtual uint32_t on_disconnected() { return UX_SUCCESS; }

    virtual uint32_t on_suspended() { return UX_SUCCESS; }
    virtual uint32_t on_resumed() { return UX_SUCCESS; }

    virtual uint32_t on_sof() { return UX_SUCCESS; }

    void start();
  };


  template <size_t system_stack_size, size_t app_stack_size>
  class Device : public DeviceBase
  {
    class AppThread : public TXX::Thread<app_stack_size> {
      Device *_dev;
    public:
      AppThread(Device *dev) : TXX::Thread<app_stack_size>("USB Device Thread"), _dev(dev) {}
      void main() override { _dev->thread_entry(); }
    };

    System<system_stack_size> system;
    AppThread app_thread;


  public:
    Device() : app_thread(this) {}

    void start_system() override {
      system.start();
    }

    void start_app() override {
      app_thread.create();
    }
  };
};


#endif /* USBXX_USBXXDEVICE_HPP_ */
