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

#include <usbxx/dcd.hpp>
#include <usbxx/endpoint.hpp>
#include <usbxx/interface.hpp>
#include <usbxx/class.hpp>

#include <memory>
#include <vector>
#include <deque>

namespace USBXX
{
  class DeviceBase;
}

struct UX_SYSTEM_SLAVE
{
    uint8_t           *ux_system_slave_dfu_framework;
    uint32_t           ux_system_slave_dfu_framework_length;
    uint32_t           ux_system_slave_power_state;
    bool           ux_system_slave_remote_wakeup_capability;
    bool           ux_system_slave_remote_wakeup_enabled;
    uint32_t           ux_system_slave_device_dfu_capabilities;
    uint32_t           ux_system_slave_device_dfu_detach_timeout;
    uint32_t           ux_system_slave_device_dfu_transfer_size;
    uint32_t           ux_system_slave_device_dfu_state_machine;
    uint32_t           ux_system_slave_device_dfu_mode;
    uint32_t           ux_system_slave_device_vendor_request;

} ;

extern UX_SYSTEM_SLAVE *_ux_system_slave;

namespace USBXX
{
  enum class LPMEvent
  {
    L0,
    L1
  };

  enum class DeviceState
  {
    RESET,
    ATTACHED,
    ADDRESSED,
    CONFIGURED,
    SUSPENDED,
    RESUMED,
    SELF_POWERED_STATE,
    BUS_POWERED_STATE,
    REMOTE_WAKEUP,
    BUS_RESET_COMPLETED,
    REMOVED,
    FORCE_DISCONNECT
  };

  static constexpr uint32_t PERIODIC_RATE = 100;

  static inline uint32_t ms_to_ticks(uint32_t ms)
  {
    return (ms * PERIODIC_RATE) / 1000 + 1;
  }

  class DeviceBase
  {
  protected:
    friend class ControlThread;

    static uint32_t _usbxx_change_notification(uint32_t);

    uint32_t on_change(uint32_t);

    std::vector<USBClass::ptr> classes;
    std::map<uint8_t, USBClass::ptr> iface_to_class;

    Descriptor  fs_desc;
    Descriptor  hs_desc;
    Strings     strings;
    LanguageIDs lang_ids;

    DeviceState state;

    DCD *dcd;

    class ControlThread : public TXX::Thread<8192> {
      DeviceBase *_dev;
    public:
      ControlThread(DeviceBase *dev) : TXX::Thread<8192>("USB Control Thread"), _dev(dev)
      {
        _priority = 1;
        _preempt = 1;
      }

      void main() override { _dev->control_thread_main(); }
    };

    TXX::Semaphore control_request_sema;
    std::deque<Transfer *> control_requests;
    Transfer *control_xfer;

    ControlThread control_thread;
    void control_thread_main();
    void handle_control_request(const ControlRequest &req);
    uint32_t get_entity_status(const ControlRequest &req);
    uint32_t set_feature(const ControlRequest &req);
    uint32_t clear_feature(const ControlRequest &req);


  public:

    DeviceDescriptor descriptor;
    uint32_t            configuration_selected;
    ConfigurationDescriptor
                    configuration_descriptor;

    std::vector<std::shared_ptr<Interface> > interfaces;
    uint32_t           power_state;

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

    virtual uint32_t on_lpm_event(LPMEvent) { return 0; }

    void start();
    uint32_t transfer_request(Transfer *transfer_request,
                                                uint32_t slave_length,
                                                uint32_t host_length);

    uint16_t get_interface_number(uint8_t class_type, uint8_t interface_type) {
      return fs_desc.get_interface_number(class_type, interface_type);
    }

    uint16_t get_configuration_number(uint8_t class_type, uint8_t interface_type) {
      return 1;
    }

    Transfer *get_control_transfer() { return dcd->get_control_transfer(); };
    USBXX::Endpoint::ptr get_control_endpoint() { return dcd->get_control_endpoint(); }

    void set_state(DeviceState _state) { state = _state; }
    DeviceState get_state() { return state; }

    bool is_configured() { return state == DeviceState::CONFIGURED; }

    const Descriptor &get_current_descriptor() { return fs_desc; }

    uint32_t send_device_descriptor(uint32_t descriptor_type, uint32_t request_index, uint32_t host_length);
    uint32_t send_compound_descriptor(uint32_t descriptor_type, uint32_t descriptor_index, uint32_t request_index, uint32_t host_length);
    uint32_t send_descriptor(const ControlRequest &);

    void disconnect();


    uint32_t on_get_alternate_setting(uint32_t interface_value);
    uint32_t on_set_alternate_setting(uint32_t interface_value, uint32_t alternate_setting_value);
    uint32_t on_get_configuration();
    uint32_t on_set_configuration(uint32_t configuration_value);

    void process_control_event(Transfer *transfer_request);
    uint32_t set_interface(DescriptorIterator &,
        uint32_t alternate_setting_value);

    uint32_t on_vendor_request(const ControlRequest &, uint8_t *, uint32_t *) { return 0; };

    uint32_t get_interface(uint8_t interface_value);

    uint32_t register_class(USBClass::ptr p_class,
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

    virtual USBClass::ptr get_interface_class(uint8_t ifnum) {
      return iface_to_class[ifnum];
    }


    // MOVE THESE HORRIBLE FUNCTIONS
    virtual uint32_t class_initialize() = 0;
    virtual uint32_t class_uninitialize() = 0;
    virtual uint32_t class_activate(Interface::ptr) = 0;
    virtual uint32_t class_deactivate() = 0;
    virtual bool class_query(Interface::ptr) = 0;
    virtual uint32_t class_command_request(const ControlRequest &) = 0;
    virtual uint32_t class_on_change() { return 0; }
  };



  //uint32_t    ux_device_class_storage_entry(USBClass_COMMAND *command);

  /*
  uint32_t    ux_device_stack_class_unregister(uint8_t *class_name,
                                      uint32_t (*class_entry_function)(USBClass_COMMAND *));
  uint32_t    _ux_device_stack_class_unregister(uint8_t *class_name, uint32_t (*class_entry_function)(USBClass_COMMAND *));
  */


  template <size_t system_stack_size, size_t app_stack_size>
  class Device : public DeviceBase
  {
    System<system_stack_size> system;


  public:
    Device() {}

    void start_system() override {
      system.start();
    }

    void start_app() override {
    }
  };
};


#endif /* USBXX_USBXXDEVICE_HPP_ */
