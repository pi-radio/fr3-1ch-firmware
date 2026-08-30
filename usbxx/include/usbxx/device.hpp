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

#include <usbxx/ux_api.h>

#include <usbxx/dcd.hpp>

namespace USBXX
{
  class DeviceBase
  {
    static UINT _usbxx_change_notification(ULONG);

    uint32_t on_change(uint32_t);

    Descriptor  fs_desc;
    Descriptor  hs_desc;
    Strings     strings;
    LanguageIDs lang_ids;

    DCD *dcd;

  protected:
    void thread_entry();

  public:
    DeviceBase();

    void set_dcd(DCD *_dcd) { dcd = _dcd; }

    void add_class(uint8_t cls) {
      fs_desc.add_class(cls);
      hs_desc.add_class(cls);
    }

    void set_manufacturer(const std::string &s) {
      fs_desc.set_manufacturer(s);
      hs_desc.set_manufacturer(s);
    }

    void set_product(const std::string &s) {
      fs_desc.set_product(s);
      hs_desc.set_product(s);
    }

    void set_serial(const std::string &s) {
      fs_desc.set_serial(s);
      hs_desc.set_serial(s);
    }

    std::string get_manufacturer() {
      return fs_desc.get_manufacturer();
    }

    std::string get_product() {
      return fs_desc.get_product();
    }

    std::string get_serial() {
      return fs_desc.get_serial();
    }


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

    uint16_t get_interface_number(uint8_t class_type, uint8_t interface_type) {
      return fs_desc.get_interface_number(class_type, interface_type);
    }

    uint16_t get_configuration_number(uint8_t class_type, uint8_t interface_type) {
      return 1;
    }



    UINT send_device_descriptor(ULONG descriptor_type, ULONG request_index, ULONG host_length);
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
