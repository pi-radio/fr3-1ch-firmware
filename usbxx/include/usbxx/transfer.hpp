#pragma once

namespace USBXX {
struct Endpoint;
}

struct UX_SLAVE_TRANSFER
{

    ULONG           ux_slave_transfer_request_status;
    ULONG           ux_slave_transfer_request_type;
    USBXX::Endpoint *ux_slave_transfer_request_endpoint;
    UCHAR           *ux_slave_transfer_request_data_pointer;
    UCHAR           *ux_slave_transfer_request_current_data_pointer;
    ULONG           ux_slave_transfer_request_requested_length;
    ULONG           ux_slave_transfer_request_actual_length;
    ULONG           ux_slave_transfer_request_in_transfer_length;
    ULONG           ux_slave_transfer_request_transfer_length;
    ULONG           ux_slave_transfer_request_completion_code;
    ULONG           ux_slave_transfer_request_phase;
    VOID            (*ux_slave_transfer_request_completion_function) (UX_SLAVE_TRANSFER *);
#if defined(UX_DEVICE_STANDALONE)
    ULONG           ux_slave_transfer_request_state;
#else
    UX_SEMAPHORE    ux_slave_transfer_request_semaphore;
#endif
    ULONG           ux_slave_transfer_request_timeout;
    ULONG           ux_slave_transfer_request_force_zlp;
    UCHAR           ux_slave_transfer_request_setup[UX_SETUP_SIZE];
    ULONG           ux_slave_transfer_request_status_phase_ignore;
};
