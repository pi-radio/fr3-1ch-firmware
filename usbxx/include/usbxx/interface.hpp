#pragma once

#include <usbxx/endpoint.hpp>
#include <usbxx/descriptor.hpp>
/* Define USBX Device Controller Interface structure.  */

struct UX_SLAVE_INTERFACE
{
    ULONG           ux_slave_interface_status;
    UX_SLAVE_CLASS
                    *ux_slave_interface_class;
    VOID            *ux_slave_interface_class_instance;

    USBXX::InterfaceDescriptor
                    ux_slave_interface_descriptor;
    UX_SLAVE_INTERFACE
                    *ux_slave_interface_next_interface;
    USBXX::Endpoint        *ux_slave_interface_first_endpoint;
};
