#pragma once

#include <usbxx/endpoint.hpp>
#include <usbxx/stm32/transfer.hpp>
#include <usbxx/stm32/pcd.hpp>

#include <usbxx/ux_stm32_config.h>

namespace USBXX
{
  namespace STM32
  {
    class DCD;

    enum class EndpointState
    {
      RESET,
      IDLE,
      DATA_TX,
      DATA_RX,
      STATUS_TX,
      STATUS_RX
    };

    class Endpoint : public USBXX::Endpoint
    {
    public:
      using ptr = std::shared_ptr<Endpoint>;

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

      Endpoint(DeviceBase *_device, DCD *_dcd, uint8_t _index);



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
      void ack_ctrl() override { throw std::runtime_error("Incorrect endpoint for control acknowledgement"); };

      uint32_t epindex() { return descriptor.bEndpointAddress & 0xF; }
      virtual HAL_StatusTypeDef transmit(PCD_EPTypeDef *ep, uint16_t wEPVal);
      virtual uint16_t receive(PCD_EPTypeDef *ep, uint16_t wEPVal);

      virtual void on_data_out();
      virtual void on_data_in();
      virtual void on_interrupt();
    };

    class ControlEndpoint : public Endpoint
    {
      enum class AckMode {
        NONE,
        DATA_IN,
        SETUP,
        DATA_OUT
      };

      AckMode ack_mode;
    public:
      using ptr = std::shared_ptr<ControlEndpoint>;


      ControlEndpoint(DeviceBase *_device, DCD *_dcd, uint8_t _index) :
        STM32::Endpoint(_device, _dcd, _index),
        ack_mode(AckMode::NONE)
      {
      }

      void on_setup();
      void on_data_out() override;
      void on_data_in() override;
      void on_interrupt() override;
      void ack_ctrl() override;

    };
  }
}

