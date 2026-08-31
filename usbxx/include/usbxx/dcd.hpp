#pragma once

#include <cstdint>

#include <usbxx/endpoint.hpp>

namespace USBXX
{
  class DeviceBase;

  class DCD
  {
  protected:
    DeviceBase *device;

  public:
    UINT            ux_slave_dcd_status;
    UINT            ux_slave_dcd_controller_type;
    UINT            ux_slave_dcd_otg_capabilities;
    UINT            ux_slave_dcd_irq;
    ULONG           ux_slave_dcd_io;
    ULONG           ux_slave_dcd_device_address;
    void            *ux_slave_dcd_controller_hardware;

    virtual uint32_t initialize() = 0;
    virtual UINT uninitialize() = 0;

    virtual Endpoint *get_endpoint(uint8_t) = 0;
    virtual Endpoint *get_control_endpoint() = 0;
    virtual UX_SLAVE_TRANSFER *get_control_transfer() = 0;

    virtual Endpoint *allocate_endpoint(const EndpointDescriptor &) = 0;
    virtual UINT create_endpoint(Endpoint *endpoint) = 0;
    virtual UINT destroy_endpoint(Endpoint *endpoint) = 0;
    virtual UINT reset_endpoint(Endpoint *endpoint) = 0;
    virtual uint32_t get_frame_number() = 0;
    virtual UINT stall(Endpoint *endpoint) = 0;
    virtual UINT get_endpoint_status(ULONG endpoint_index) = 0;
    virtual UINT abort_transfer(UX_SLAVE_TRANSFER *xfer) = 0;
    virtual UINT transfer_request(UX_SLAVE_TRANSFER *xfer) = 0;
    virtual UINT complete_initialization() = 0;

    void set_device(DeviceBase *_dev) { device = _dev; }



    virtual void setup() = 0;
    virtual void on_data_in(uint8_t epnum) = 0;
    virtual void on_data_out(uint8_t epnum) = 0;
    virtual void reset() = 0;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;
    virtual void on_sof() = 0;
    virtual void on_state_change(uint32_t state) = 0;
    virtual void set_device_address(uint8_t) = 0;
  };
};
