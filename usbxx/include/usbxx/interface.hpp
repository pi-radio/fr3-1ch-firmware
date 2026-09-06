#pragma once

#include <usbxx/endpoint.hpp>
#include <usbxx/descriptor.hpp>
/* Define USBX Device Controller Interface structure.  */

#include <memory>
#include <vector>

namespace USBXX
{
  class DeviceBase;
  class USBClass;

  struct Interface : public std::enable_shared_from_this<Interface>
  {
    using ptr = std::shared_ptr<Interface>;

    DeviceBase *device;
    ULONG           status;
    USBClass  *usb_class;
    VOID            *class_instance;

    USBXX::InterfaceDescriptor descriptor;


    std::vector<USBXX::Endpoint::ptr> endpoints;

    Interface(DeviceBase *_dev) :
      device(_dev),
      status(0),
      usb_class(nullptr)
    {

    }

    virtual uint32_t start();
    virtual uint32_t stop();
  };
}
