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
    uint32_t           completion_code;

    TransferType           type;
    std::shared_ptr<USBXX::Endpoint> endpoint;
    uint32_t          buffer_size;
    uint8_t           *data;
    uint8_t           *current_data_pointer;
    uint32_t           requested_length;
    uint32_t           actual_length;
    uint32_t           in_transfer_length;
    uint32_t           transfer_length;
    TransferPhase   phase;
    VOID            (*completion_function) (Transfer *);
    uint32_t           timeout;
    uint32_t           force_zlp;
    uint8_t           setup[8];
    uint32_t           status_phase_ignore;
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
