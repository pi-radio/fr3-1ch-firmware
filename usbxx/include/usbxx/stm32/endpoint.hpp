#pragma once

#include <usbxx/endpoint.hpp>
#include <usbxx/stm32/transfer.hpp>

#include <usbxx/ux_stm32_config.h>

namespace USBXX
{
  namespace STM32
  {
    class DCD;

    enum class EndpointState
    {
      IDLE,
      DATA_TX,
      DATA_RX,
      STATUS_TX,
      STATUS_RX
    };

    class Endpoint : public USBXX::Endpoint
    {
    public:
      bool in_transfer;
      bool stalled;
      bool done;
      bool setup_in;
      bool setup_status;
      bool setup_out;
      bool setup;
      bool task_pending;
      STM32::Transfer transfer;

      EndpointState           state;
      UCHAR           index;
      UCHAR           direction;
      DCD      *dcd;

      Endpoint() :
        state(EndpointState::IDLE),
        index(0),
        direction(0)
      {
        reset_flags();
      }



      void reset_flags() override {
        USBXX::Endpoint::reset_flags();
        direction = 0;
        in_transfer = false; // NB -- this is not being used properly
        stalled = false;
        done = false;
        setup_in = false;
        setup_status = false;
        setup_out = false;
        setup = false;
        task_pending = false;
      }

      Transfer *get_transfer() override { return &transfer; };

      void abort_transfer();

      UINT create() override;
      UINT destroy() override;
      bool is_stalled() override;
      UINT reset() override;
      void stall() override;
      void abort_all_transfers(uint32_t code) override { transfer.abort(code); };
    };
  }
}

