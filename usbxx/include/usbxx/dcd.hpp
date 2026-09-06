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
    UINT            status;
    UINT            controller_type;
    UINT            otg_capabilities;
    UINT            irq;
    ULONG           io;
    ULONG           device_address;
    void            *controller_hardware;

    virtual uint32_t initialize() = 0;
    virtual UINT uninitialize() = 0;

    virtual USBXX::Endpoint::ptr get_endpoint(uint8_t) = 0;
    virtual USBXX::Endpoint::ptr get_control_endpoint() = 0;
    virtual Transfer *get_control_transfer() = 0;

    virtual USBXX::Endpoint::ptr allocate_endpoint(std::shared_ptr<Interface>, const EndpointDescriptor &) = 0;
    virtual uint32_t get_frame_number() = 0;
    virtual UINT complete_initialization() = 0;

    void set_device(DeviceBase *_dev) { device = _dev; }

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
