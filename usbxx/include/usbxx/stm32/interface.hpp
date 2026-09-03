#pragma once

#include <usbxx/interface.hpp>
/* Define USBX Device Controller Interface structure.  */

#include <vector>

namespace USBXX
{
  namespace STM32
  {
    struct Interface : public USBXX::Interface
    {
      Interface(DeviceBase *_dev) : USBXX::Interface(_dev) { }

      virtual uint32_t del();
      virtual uint32_t start();
    };
  }
}
