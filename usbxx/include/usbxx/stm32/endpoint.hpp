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


    class Endpoint : public USBXX::Endpoint
    {
    public:
      using ptr = std::shared_ptr<Endpoint>;

    protected:

      bool in_transfer;
      bool stalled;
      bool done;
      bool setup_in;
      bool setup_status;
      bool setup_out;
      bool setup;
      bool task_pending;
      STM32::Transfer transfer;

      uint8_t        epaddr;
      uint8_t        direction;
      DCD            *dcd;

      virtual PCD_EPTypeDef *get_epdata() = 0;

      Endpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr);

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

      virtual void activate() = 0;
      virtual void deactivate() = 0;

      virtual void clear_stall() = 0;

public:
      uint8_t get_addr() { return descriptor.bEndpointAddress; }

      Transfer *get_transfer() override { return &transfer; };

      void set_descriptor(const EndpointDescriptor &_desc) { descriptor = _desc; }
      void set_interface(std::shared_ptr<Interface> _iface) { interface = _iface; }

      virtual void init();

      virtual void abort_transfer() = 0;


      void start_transfer(PCD_EPTypeDef *);

      virtual void open();

      UINT create() override;
      UINT destroy() override;
      bool is_stalled() override;
      UINT reset() override;
      //void stall() override;
      void abort_all_transfers(uint32_t code) override { transfer.abort(code); };
      void ack_ctrl() override { throw USBXX::runtime_error("Incorrect endpoint for control acknowledgement"); };

      bool is_in() { return (descriptor.bEndpointAddress & 0x80) ? true : false; }
      uint32_t epindex() { return descriptor.bEndpointAddress & 0xF; }
      virtual HAL_StatusTypeDef transmit(PCD_EPTypeDef *ep, uint16_t wEPVal);
      virtual uint16_t receive(PCD_EPTypeDef *ep, uint16_t wEPVal);

      virtual void ll_receive(uint8_t *buf, uint32_t sz);
      virtual void ll_transmit(uint8_t *buf, uint32_t sz);

      virtual void read_pma(uint8_t *buf, uint32_t pmaaddr, uint32_t len);
      virtual void write_pma(uint32_t pmaaddr, const uint8_t *buf, uint32_t len);

      virtual void on_data_out();
      virtual void on_data_in();
      virtual void on_interrupt();
    };

    class InEndpoint : public Endpoint
    {
    protected:
      PCD_EPTypeDef *get_epdata() override;

      void activate() override;
      void deactivate() override;

      void clear_stall() override;

    public:
      InEndpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr);

      void abort_transfer();
      void stall() override;
    };

    class OutEndpoint : public Endpoint
    {
    protected:
      PCD_EPTypeDef *get_epdata() override;

      void activate() override;
      void deactivate() override;

      void clear_stall() override;

    public:
      OutEndpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr);

      void abort_transfer();
      void stall() override;
    };

    enum class ControlEndpointState
    {
      RESET,
      IDLE,
      DATA_TX,
      DATA_RX,
      STATUS_TX,
      STATUS_RX
    };


    class ControlEndpoint : public Endpoint
    {
    protected:
      enum class AckMode {
        NONE,
        DATA_IN,
        SETUP,
        DATA_OUT
      };

      AckMode ack_mode;
      ControlEndpointState state;

      PCD_EPTypeDef *get_epdata() override;

      void activate() override;
      void deactivate() override;

      void clear_stall() override;

    public:
      using ptr = std::shared_ptr<ControlEndpoint>;


      ControlEndpoint(DeviceBase *_device, DCD *_dcd);

      void init() override;

      void open() override;
      UINT create() override;
      UINT destroy() override;

      void stall() override;
      void abort_transfer();


      void on_setup();
      void on_data_out() override;
      void on_data_in() override;
      void on_interrupt() override;
      void ack_ctrl() override;

    };
  }
}

