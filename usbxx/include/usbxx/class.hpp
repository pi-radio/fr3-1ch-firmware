#pragma once

#include <string>
#include <cstdint>
#include <memory>

#include <usbxx/ux_api.h>

//#include <usbxx/control.hpp>

#define USBD_MAX_CLASS_ENDPOINTS                       9U
#define USBD_MAX_CLASS_INTERFACES                      11U

namespace USBXX
{
  class Interface;
  class DeviceClass;
  class DeviceBase;

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

      VOID            *instance;
      VOID            *client;
      VOID            *interface_parameter;
      ULONG           interface_number;
      ULONG           configuration_number;
      std::shared_ptr<Interface>       interface;

      USBClass(const std::string &_name,
          DeviceBase *_device) :
            name(_name),
            device(_device)
      {

      }
  };

#if 0

  /* USB endpoint handle structure */
  struct EndpointDesc
  {
    uint8_t add;
    uint8_t type;
    uint16_t size;
    uint8_t is_used;
  } ;

  /* USB Composite handle structure */
  struct CompositeElement
  {
    CompositeClass ClassType;
    uint32_t ClassId;
    uint8_t InterfaceType;
    uint32_t Active;
    uint32_t NumEps;
    uint32_t NumIf;
    EndpointDesc Eps[USBD_MAX_CLASS_ENDPOINTS];
    uint8_t Ifs[USBD_MAX_CLASS_INTERFACES];
  };

  class Transfer;

  struct DeviceClass
  {
  protected:

    DeviceBase &device;
    std::string name;
    CompositeClass clsno;

    //void *client;
    //UX_THREAD          thread;
    //VOID               *thread_stack;
    //ULONG              interface_number;
    //ULONG              configuration_number;
    Interface *interface;

    DeviceClass(DeviceBase &_device,
        const std::string &_name,
        CompositeClass _clsno);

  public:
    const std::string &get_name() { return name; }
    CompositeClass get_class_no() { return clsno; }

    void set_interface(Interface *_iface) { interface = _iface; }

    virtual bool class_query(CompositeClass _qcls) { return _qcls == clsno; }

    virtual uint32_t class_get_config_number() { return -1; }
    virtual uint32_t class_get_interface_number() { return -1; }
    virtual uint32_t class_initialize() { return -1; };
    virtual uint32_t class_uninitialize() { return -1; };
    virtual uint32_t class_activate() { return -1; };
    virtual uint32_t class_change() { return 0; }
    virtual uint32_t class_deactivate() { return -1; };
    virtual uint32_t class_control_request(const ControlRequest &req, Transfer *xfer) { return -1; };
  };

#endif
}
