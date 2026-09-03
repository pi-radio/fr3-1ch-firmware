#include <usbxx/endpoint.hpp>

using namespace USBXX;


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
