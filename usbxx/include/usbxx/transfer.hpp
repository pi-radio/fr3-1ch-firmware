#pragma once

#include <threadxx/semaphore.hpp>

namespace USBXX {
struct Endpoint;
}

namespace USBXX
{
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

  enum class TransferStatus {
    IDLE,
    PENDING,
    COMPLETED,
    ABORTED
  };

  class Transfer
  {
  protected:
    TXX::Semaphore  semaphore;

  public:
    TransferStatus  status;
    ULONG           completion_code;

    TransferType           type;
    USBXX::Endpoint *endpoint;
    UCHAR           *data;
    UCHAR           *current_data_pointer;
    ULONG           requested_length;
    ULONG           actual_length;
    ULONG           in_transfer_length;
    ULONG           transfer_length;
    TransferPhase   phase;
    VOID            (*completion_function) (Transfer *);
    ULONG           timeout;
    ULONG           force_zlp;
    UCHAR           setup[8];
    ULONG           status_phase_ignore;

    Transfer() : semaphore("transfer semaphore") {
      data = (UCHAR *)::malloc(2048);
    };

    virtual void complete(uint32_t code) = 0;
    virtual void abort(uint32_t code) = 0;
    void set_pending();
    bool is_pending();
    bool is_valid();


    uint32_t wait();
    void reset();
  };
}
