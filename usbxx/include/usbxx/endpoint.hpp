#pragma once

#include <cstdint>

/* Define USBX Device Controller Endpoint structure.  */

struct UX_SLAVE_ENDPOINT
{

    ULONG           ux_slave_endpoint_status;
    ULONG           ux_slave_endpoint_state;
    void            *ux_slave_endpoint_ed;
    UX_ENDPOINT_DESCRIPTOR
                    ux_slave_endpoint_descriptor;
    UX_SLAVE_ENDPOINT
                    *ux_slave_endpoint_next_endpoint;
    UX_SLAVE_INTERFACE
                    *ux_slave_endpoint_interface;
    UX_SLAVE_DEVICE
                    *ux_slave_endpoint_device;
    UX_SLAVE_TRANSFER
                    ux_slave_endpoint_transfer_request;
};

namespace USBXX
{
  class Endpoint
  {
public:
    ULONG           ux_slave_endpoint_status;
    ULONG           ux_slave_endpoint_state;
    void            *ux_slave_endpoint_ed;
    UX_ENDPOINT_DESCRIPTOR
                    ux_slave_endpoint_descriptor;
    UX_SLAVE_ENDPOINT
                    *ux_slave_endpoint_next_endpoint;
    UX_SLAVE_INTERFACE
                    *ux_slave_endpoint_interface;
    UX_SLAVE_DEVICE
                    *ux_slave_endpoint_device;
    UX_SLAVE_TRANSFER
                    ux_slave_endpoint_transfer_request;
  };
}
