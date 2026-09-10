#include <usbxx/endpoint.hpp>

using namespace USBXX;

Transfer::Transfer() :
  semaphore("transfer semaphore"),
  completion_function(nullptr),
  begin_magic(MAGIC),
  end_magic(MAGIC)
{
  buffer_size = 2048;
  data = (uint8_t *)::malloc(buffer_size);
  current_data_pointer = data;
  requested_length = 0;
  actual_length = 0;
  in_transfer_length = 0;
  transfer_length = 0;
  phase = TransferPhase::IDLE;
  timeout = TX_WAIT_FOREVER;
  force_zlp = false;
  status_phase_ignore = false;
};


void Transfer::set_pending()
{
  status = TransferStatus::PENDING;
}

bool Transfer::is_pending()
{
  return status == TransferStatus::PENDING;
}

bool Transfer::is_valid()
{
  return completion_code == 0;
}

uint32_t Transfer::wait()
{
  uint32_t retval = semaphore.get(timeout);

  if (retval == 0) {
    retval = completion_code;
  }

  return retval;
}


void Transfer::reset() {
  if (semaphore.get_suspended_count())
    semaphore.put();
}
