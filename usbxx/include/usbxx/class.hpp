#pragma once

#include <string>
#include <cstdint>

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

  struct UX_SLAVE_CLASS
  {
      std::string  name; /* "+1" for string null-terminator */

      UINT            ux_slave_class_status;
      UINT            (*ux_slave_class_entry_function) (struct UX_SLAVE_CLASS_COMMAND *);
      VOID            *ux_slave_class_instance;
      VOID            *ux_slave_class_client;
      UX_THREAD       ux_slave_class_thread;
      VOID            *ux_slave_class_thread_stack;
      VOID            *ux_slave_class_interface_parameter;
      ULONG           ux_slave_class_interface_number;
      ULONG           ux_slave_class_configuration_number;
      Interface       *ux_slave_class_interface;

  };

  struct UX_SLAVE_CLASS_COMMAND
  {

      UINT            ux_slave_class_command_request;
      VOID            *ux_slave_class_command_container;
      VOID            *ux_slave_class_command_interface;
      UINT            ux_slave_class_command_pid;
      UINT            ux_slave_class_command_vid;
      UINT            ux_slave_class_command_class;
      UINT            ux_slave_class_command_subclass;
      UINT            ux_slave_class_command_protocol;
      UX_SLAVE_CLASS
                      *ux_slave_class_command_class_ptr;
      VOID            *ux_slave_class_command_parameter;
      VOID            *ux_slave_class_command_interface_number;

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

    //void *ux_slave_class_client;
    //UX_THREAD          ux_slave_class_thread;
    //VOID               *ux_slave_class_thread_stack;
    //ULONG              ux_slave_class_interface_number;
    //ULONG              ux_slave_class_configuration_number;
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
