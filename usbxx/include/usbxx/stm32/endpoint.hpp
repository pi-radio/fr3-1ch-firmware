#pragma once

#include <usbxx/endpoint.hpp>
#include <usbxx/stm32/transfer.hpp>
#include <usbxx/stm32/pcd.hpp>

namespace USBXX
{
  namespace STM32
  {
    class DCD;

    struct XferState
    {
      uint16_t  pmaaddress;
      uint8_t   *xfer_buff;
      uint32_t  xfer_len;
      uint32_t  xfer_count;
    } ;

    class Endpoint : public USBXX::Endpoint
    {
    public:
      using ptr = std::shared_ptr<Endpoint>;

#define UX_MAX_NUMBER_OF_TRANSACTIONS_MASK                              0x1800u
#define UX_MAX_NUMBER_OF_TRANSACTIONS_SHIFT                             11

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
      DCD            *dcd;

      Endpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr);

      void reset_flags() override {
        USBXX::Endpoint::reset_flags();
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

      uint32_t pmaaddr() { return 0x40 + 0x80 * epindex() + (is_in() ? 0x40 : 0x00); }

public:


      Transfer *get_transfer() override { return &transfer; };

      void set_descriptor(const EndpointDescriptor &_desc) { descriptor = _desc; }
      void set_interface(std::shared_ptr<Interface> _iface) { interface = _iface; }

      virtual void init();

      virtual void abort_transfer() = 0;


      void start_transfer_in(XferState *);
      void start_transfer_out(XferState *);

      virtual void open();

      uint32_t create() override;
      uint32_t destroy() override;
      bool is_stalled() override;
      uint32_t reset() override;
      //void stall() override;
      void abort_all_transfers(uint32_t code) override { transfer.abort(code); };
      void ack_ctrl() override { throw USBXX::runtime_error("Incorrect endpoint for control acknowledgement"); };

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
      XferState ep;

    protected:
      void activate() override;
      void deactivate() override;

      void clear_stall() override;

      void ll_transmit(uint8_t *buf, uint32_t sz) override;

    public:
      InEndpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr);

      void abort_transfer();
      void stall() override;

      virtual void on_data_in();
    };

    class OutEndpoint : public Endpoint
    {
      XferState ep;

    protected:
      void activate() override;
      void deactivate() override;

      void clear_stall() override;

      void ll_receive(uint8_t *buf, uint32_t sz) override;

    public:
      OutEndpoint(DeviceBase *_device, DCD *_dcd, uint8_t _epaddr);

      void abort_transfer();
      void stall() override;

      virtual void on_data_out();
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

      enum class Direction {
        OUT,
        IN
      };

      static constexpr uint32_t CONTROL_TRANSFER_TIMEOUT = 10000;

      ControlEndpointState state;
      AckMode ack_mode;
      Direction direction;


      void activate() override;
      void deactivate() override;

      void clear_stall() override;

      void ll_receive(uint8_t *buf, uint32_t sz) override;
      void ll_transmit(uint8_t *buf, uint32_t sz) override;

      XferState in_ep;
      XferState out_ep;


    public:
      using ptr = std::shared_ptr<ControlEndpoint>;


      ControlEndpoint(DeviceBase *_device, DCD *_dcd);

      void init() override;

      void open() override;
      uint32_t create() override;
      uint32_t destroy() override;

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

