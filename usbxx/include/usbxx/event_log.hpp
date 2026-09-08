#pragma once

#include <cstdint>

#include <threadxx/ring_buffer.hpp>

namespace USBXX
{
  enum class UsbEvent
  {
    DEVICE_RESET,
    DEVICE_ATTACHED,
    DEVICE_ADDRESSED,
    DEVICE_CONFIGURED,

    START_IRQ,
    END_IRQ,

    ENDPOINT_IRQ,

    START_CONTROL_REQUEST,
    END_CONTROL_REQUEST,

    SET_CONFIGURATION_START,
    SET_CONFIGURATION_END,

    HW_PACKET_OUT_START,
    HW_PACKET_OUT_CMPL,
    HW_PACKET_OUT_TIMEOUT,
    HW_PACKET_IN_START,
    HW_PACKET_IN_CMPL,
    HW_PACKET_IN_TIMEOUT,

    HW_ENDPOINT_INTR,


    HW_ENDPOINT_INTR_ERROR,
    BAD_SETUP_ERROR,

    CONTROL_EP_EVENT,
    CONTROL_DATA_IN,
    CONTROL_DATA_OUT,
    CONTROL_NO_TRANSFER_BUF,
    HW_PACKET_TX_CTRL,
    HW_PACKET_RX_CTRL,
  };

  struct EventLogEntry
  {
    uint32_t cycle;
    UsbEvent evt;
    uint32_t data;
    uint32_t frame;
  };

  class EventLog : private TXX::ring_buffer_base<EventLogEntry, 64>
  {
    uint32_t frame;
  public:
    void set_frame(uint32_t _frame) { frame = _frame; }
    void push_event(UsbEvent evt_type, uint32_t data = 0);
  };

  extern EventLog event_log;
}
