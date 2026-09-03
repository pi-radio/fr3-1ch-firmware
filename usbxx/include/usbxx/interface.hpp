#pragma once

#include <usbxx/endpoint.hpp>
#include <usbxx/descriptor.hpp>
/* Define USBX Device Controller Interface structure.  */

#include <vector>

namespace USBXX
{
  class DeviceBase;
  class UX_SLAVE_CLASS;

  struct Interface
  {
    DeviceBase *device;
    ULONG           status;
    UX_SLAVE_CLASS  *usb_class;
    VOID            *class_instance;

    USBXX::InterfaceDescriptor descriptor;
    Interface       *next_interface;


    std::vector<USBXX::Endpoint *> endpoints;

    Interface(DeviceBase *_dev) :
      device(_dev)
    {

    }
  };

  UINT    ux_device_stack_interface_delete(Interface *ux_interface);
  UINT    ux_device_stack_interface_start(Interface *ux_interface);
  UINT    _ux_device_stack_interface_delete(Interface *ux_interface);
  UINT    _ux_device_stack_interface_start(Interface *ux_interface);

}
