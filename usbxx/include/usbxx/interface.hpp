#pragma once

#include <usbxx/endpoint.hpp>
#include <usbxx/descriptor.hpp>
/* Define USBX Device Controller Interface structure.  */

#include <memory>
#include <vector>

namespace USBXX
{
  class DeviceBase;
  class UX_SLAVE_CLASS;

  struct Interface : public std::enable_shared_from_this<Interface>
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

    virtual uint32_t start();
    virtual uint32_t stop();
  };
}
