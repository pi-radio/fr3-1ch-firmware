#pragma once

#include <usbxx/transfer.hpp>

namespace USBXX {
struct Endpoint;
}

namespace USBXX
{
  namespace STM32
  {
    class Transfer : public USBXX::Transfer
    {
    public:
      Transfer() : USBXX::Transfer() {};

      void complete(uint32_t code) override;
      void abort(uint32_t code) override;
    };
  }
}
