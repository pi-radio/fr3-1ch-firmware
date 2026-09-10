#pragma once

#include <string>
#include <cstdint>
#include <memory>

//#include <usbxx/control.hpp>

//#define USBD_MAX_CLASS_ENDPOINTS                       9U
//#define USBD_MAX_CLASS_INTERFACES                      11U

namespace USBXX
{
  class Interface;
  class DeviceClass;
  class DeviceBase;
  class ControlRequest;

  enum CompositeClass
  {
    CLASS_TYPE_NONE     = 0,
    CLASS_TYPE_HID      = 1,
    CLASS_TYPE_CDC_ACM  = 2,
    CLASS_TYPE_MSC      = 3,
    CLASS_TYPE_CDC_ECM  = 4,
    CLASS_TYPE_DFU      = 5,
    CLASS_TYPE_PIMA_MTP = 6,
    CLASS_TYPE_RNDIS    = 7,
    CLASS_TYPE_VIDEO    = 8,
    CLASS_TYPE_CCID     = 9,
    CLASS_TYPE_PRINTER  = 10,
  } ;

  /* Define USBX Device Class container structure.  */

  struct USBClass : public std::enable_shared_from_this<USBClass>
  {
    using ptr = std::shared_ptr<USBClass>;

    std::string  name; /* "+1" for string null-terminator */
    DeviceBase   *device;

    void            *instance;
    void            *client;
    void            *interface_parameter;
    uint32_t           interface_number;
    uint32_t           configuration_number;
    std::shared_ptr<Interface>       interface;

    USBClass(const std::string &_name,
        DeviceBase *_device) :
          name(_name),
          device(_device)
    {

    }

    virtual uint32_t initialize() = 0;
    virtual uint32_t uninitialize() = 0;
    virtual uint32_t activate(std::shared_ptr<Interface>) = 0;
    virtual uint32_t deactivate() = 0;
    virtual bool query(std::shared_ptr<Interface>) = 0;
    virtual uint32_t command_request(const ControlRequest &) = 0;
    virtual uint32_t on_change() { return 0; }
  };
}
