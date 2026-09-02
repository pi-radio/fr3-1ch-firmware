#pragma once

namespace USBXX {
struct Endpoint;
}

enum class TransferPhase {
  SETUP,
  DATA_IN,
  DATA_OUT,
  STATUS_IN,
  STATUS_OUT
};

enum class TransferType {
  NORMAL,
  SETUP
};

struct UX_SLAVE_TRANSFER
{

    ULONG           status;
    TransferType           type;
    USBXX::Endpoint *endpoint;
    UCHAR           *data;
    UCHAR           *current_data_pointer;
    ULONG           requested_length;
    ULONG           actual_length;
    ULONG           in_transfer_length;
    ULONG           transfer_length;
    ULONG           completion_code;
    TransferPhase   phase;
    VOID            (*completion_function) (UX_SLAVE_TRANSFER *);
#if defined(UX_DEVICE_STANDALONE)
    ULONG           state;
#else
    UX_SEMAPHORE    semaphore;
#endif
    ULONG           timeout;
    ULONG           force_zlp;
    UCHAR           setup[UX_SETUP_SIZE];
    ULONG           status_phase_ignore;
};
