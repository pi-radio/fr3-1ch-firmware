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
      uint32_t transfer_in();
      uint32_t transfer_out();

    public:
      Transfer() : USBXX::Transfer() {};

      uint32_t transfer() override;
      void complete(uint32_t code) override;
      void abort(uint32_t code) override;
    };
  }
}
