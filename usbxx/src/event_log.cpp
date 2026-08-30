#include <usbxx/event_log.hpp>

extern "C" {
#include "stm32h5xx_hal.h"
}

using namespace USBXX;

#include <threadxx/intr.hpp>

EventLog USBXX::event_log;

void EventLog::push_event(UsbEvent evt_type, uint32_t data)
{
  TXX::lock_intr l;

  push(EventLogEntry(DWT->CYCCNT, evt_type, data, frame));

  if (full())
  {
    int a = 0;
  }
}
