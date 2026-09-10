#pragma once

#include <memory>

#include <threadxx/semaphore.hpp>

#include <usbxx/exceptions.hpp>

namespace USBXX {
struct Endpoint;
}

namespace USBXX
{
  enum class TransferPhase {
    IDLE,
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

  public:
    static constexpr uint32_t MAGIC = 0xFEEDF00D;

    uint32_t        begin_magic;
    TXX::Semaphore  semaphore;
    TransferStatus  status;
    ULONG           completion_code;

    TransferType           type;
    std::shared_ptr<USBXX::Endpoint> endpoint;
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
    uint32_t        end_magic;

    Transfer();

    virtual uint32_t transfer() = 0;
    virtual void complete(uint32_t code) = 0;
    virtual void abort(uint32_t code) = 0;
    void set_pending();
    bool is_pending();
    bool is_valid();


    uint32_t wait();
    void reset();
  };
}
