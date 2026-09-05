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
#include <usbxx/endpoint.hpp>
#include <usbxx/interface.hpp>
#include <usbxx/class.hpp>

#include <memory>
#include <vector>

namespace USBXX
{
  class DeviceBase;
}

struct UX_SYSTEM_SLAVE
{
    USBXX::DeviceBase *device;
    UCHAR           *ux_system_slave_dfu_framework;
    ULONG           ux_system_slave_dfu_framework_length;
    USBXX::USBClass  *ux_system_slave_class_array;
    USBXX::USBClass  *ux_system_slave_interface_class_array[UX_MAX_SLAVE_INTERFACES];
    ULONG           ux_system_slave_speed;
    ULONG           ux_system_slave_power_state;
    ULONG           ux_system_slave_remote_wakeup_capability;
    ULONG           ux_system_slave_remote_wakeup_enabled;
    ULONG           ux_system_slave_device_dfu_capabilities;
    ULONG           ux_system_slave_device_dfu_detach_timeout;
    ULONG           ux_system_slave_device_dfu_transfer_size;
    ULONG           ux_system_slave_device_dfu_state_machine;
    ULONG           ux_system_slave_device_dfu_mode;
    ULONG           ux_system_slave_device_vendor_request;

} ;

extern UX_SYSTEM_SLAVE *_ux_system_slave;

namespace USBXX
{
  class DeviceBase
  {
    static UINT _usbxx_change_notification(ULONG);

    uint32_t on_change(uint32_t);

    USBClass classes[UX_MAX_SLAVE_CLASS_DRIVER];

    Descriptor  fs_desc;
    Descriptor  hs_desc;
    Strings     strings;
    LanguageIDs lang_ids;

    DCD *dcd;

  public:

    ULONG            state;
    DeviceDescriptor descriptor;
    ULONG            configuration_selected;
    ConfigurationDescriptor
                    configuration_descriptor;

    std::vector<std::shared_ptr<Interface> > interfaces;
    ULONG           power_state;

  protected:
    void thread_entry();

  public:
    DeviceBase();

    void set_dcd(DCD *_dcd)
    {
      dcd = _dcd;
      dcd->set_device(this);
    }

    DCD *get_dcd()
    {
      return dcd;
    }

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
    virtual void start_app() {}

    virtual uint32_t on_attached() { return 0; }
    virtual uint32_t on_removed() { return 0; }

    virtual uint32_t on_connected() { return 0; }
    virtual uint32_t on_disconnected() { return 0; }

    virtual uint32_t on_suspended() { return 0; }
    virtual uint32_t on_resumed() { return 0; }

    virtual uint32_t on_sof() { return 0; }

    void start();
    uint32_t transfer_request(Transfer *transfer_request,
                                                ULONG slave_length,
                                                ULONG host_length);

    uint16_t get_interface_number(uint8_t class_type, uint8_t interface_type) {
      return fs_desc.get_interface_number(class_type, interface_type);
    }

    uint16_t get_configuration_number(uint8_t class_type, uint8_t interface_type) {
      return 1;
    }

    Transfer *get_control_transfer() { return dcd->get_control_transfer(); };
    Endpoint *get_control_endpoint() { return dcd->get_control_endpoint(); }

    void set_state(uint32_t state) { state = state; }
    uint32_t get_state() { return state; }

    const Descriptor &get_current_descriptor() { return fs_desc; }

    UINT send_device_descriptor(ULONG descriptor_type, ULONG request_index, ULONG host_length);
    UINT send_compound_descriptor(ULONG descriptor_type, ULONG descriptor_index, ULONG request_index, ULONG host_length);
    UINT send_descriptor(const ControlRequest &);

    void disconnect();


    uint32_t on_get_alternate_setting(ULONG interface_value);
    uint32_t on_set_alternate_setting(ULONG interface_value, ULONG alternate_setting_value);
    uint32_t on_get_configuration();
    uint32_t on_set_configuration(uint32_t configuration_value);
    uint32_t clear_feature(uint32_t request_type, uint32_t request_value, uint32_t request_index);

    uint32_t process_control_event(Transfer *transfer_request);
    uint32_t set_feature(uint32_t request_type, uint32_t request_value, uint32_t request_index);
    uint32_t set_interface(const uint8_t * device_framework, uint32_t device_framework_length,
        uint32_t alternate_setting_value);
    uint32_t get_entity_status(uint32_t request_type, uint32_t request_index, uint32_t request_length);

    UINT on_vendor_request(ULONG, ULONG, ULONG, ULONG, UCHAR *, ULONG *) { return 0; };

    uint32_t get_interface(uint8_t interface_value);

    uint32_t register_class(const std::string &class_name,
                            uint32_t configuration_number,
                            uint32_t interface_number,
                            void *parameter);

    void uninitialize(void);

    std::shared_ptr<Interface> find_interface(uint8_t ifno) {
      for (auto iface : interfaces) {
        if (iface->descriptor.bInterfaceNumber == ifno)
          return iface;
      }

      return nullptr;
    }


    // MOVE THESE HORRIBLE FUNCTIONS
    virtual uint32_t class_initialize() = 0;
    virtual uint32_t class_uninitialize() = 0;
    virtual uint32_t class_activate(Interface::ptr) = 0;
    virtual uint32_t class_deactivate() = 0;
    virtual bool class_query(Interface::ptr) = 0;
    virtual uint32_t class_command_request() = 0;
    virtual uint32_t class_on_change() { return 0; }
  };



  //UINT    ux_device_class_storage_entry(USBClass_COMMAND *command);

  /*
  UINT    ux_device_stack_class_unregister(UCHAR *class_name,
                                      UINT (*class_entry_function)(USBClass_COMMAND *));
  UINT    _ux_device_stack_class_unregister(UCHAR *class_name, UINT (*class_entry_function)(USBClass_COMMAND *));
  */


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
