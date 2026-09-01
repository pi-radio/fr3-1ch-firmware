#pragma once

#include <cstdint>
#include <usbxx/transfer.hpp>
#include <usbxx/descriptor.hpp>

/* Define USBX Device Controller Endpoint structure.  */

namespace USBXX
{
  class DeviceBase;

  struct Endpoint
  {
    bool used;

    ULONG           ux_slave_endpoint_state;
    EndpointDescriptor
                    ux_slave_endpoint_descriptor;
    Endpoint
                    *ux_slave_endpoint_next_endpoint;
    UX_SLAVE_INTERFACE
                    *ux_slave_endpoint_interface;
    USBXX::DeviceBase
                    *ux_slave_endpoint_device;
    UX_SLAVE_TRANSFER
                    ux_slave_endpoint_transfer_request;

    Endpoint() : used(false)
    {

    }

    virtual void reset_flags()
    {
      used = false;
    }

    virtual UINT create() = 0;
    virtual UINT destroy() = 0;
    virtual bool is_stalled() = 0;
    virtual UINT reset() = 0;
    virtual void stall() = 0;
  };
}
