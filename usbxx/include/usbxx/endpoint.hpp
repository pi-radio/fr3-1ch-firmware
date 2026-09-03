#pragma once

#include <cstdint>
#include <usbxx/transfer.hpp>
#include <usbxx/descriptor.hpp>

/* Define USBX Device Controller Endpoint structure.  */

namespace USBXX
{
  class DeviceBase;
  class Interface;

  enum class RequestType
  {
    STANDARD,
    CLASS,
    VENDOR
  };

  enum class RequestRecipient
  {
    DEVICE = 0,
    INTERFACE = 1,
    ENDPOINT = 2,
    OTHER = 3,
  };

  struct ControlRequest
  {
    bool is_in;
    RequestRecipient recipient;
    RequestType type;
    uint8_t code;
    uint16_t value;
    uint16_t index;
    uint16_t length;

    static constexpr uint32_t REQUEST_TYPE_OFFSET = 0;
    static constexpr uint32_t REQUEST_CODE = 1;
    static constexpr uint32_t REQUEST_VALUE = 2;
    static constexpr uint32_t REQUEST_INDEX = 4;
    static constexpr uint32_t REQUEST_LENGTH = 6;
    //static constexpr uint32_t UX_SETUP_SIZE = 8;


    ControlRequest(const uint8_t *buffer) {
      if (buffer[0] & 0x80) {
        is_in = true;
      } else {
        is_in = false;
      }

      switch(buffer[0] & 0x1F) {
      case 0:
        recipient = RequestRecipient::DEVICE;
        break;

      case 1:
        recipient = RequestRecipient::INTERFACE;
        break;

      case 2:
        recipient = RequestRecipient::ENDPOINT;
        break;

      case 3:
        recipient = RequestRecipient::OTHER;
        break;

      default:
        throw std::runtime_error("Invalid request recipient");
      }



      switch((buffer[0] >> 5) & 0x3) {
      case 0:
        type = RequestType::STANDARD;
        break;

      case 1:
        type = RequestType::CLASS;
        break;

      case 2:
        type = RequestType::VENDOR;
        break;

      case 3:
        throw std::runtime_error("Invalid request type");
      }

      code = buffer[REQUEST_CODE];
      value = usb_get_short(buffer + REQUEST_VALUE);
      index = usb_get_short(buffer + REQUEST_INDEX);
      length = usb_get_short(buffer + REQUEST_LENGTH);
    }
  };

  struct Endpoint
  {
    bool used;

    ULONG           ux_slave_endpoint_state;
    EndpointDescriptor
                    ux_slave_endpoint_descriptor;
    Endpoint
                    *ux_slave_endpoint_next_endpoint;
    Interface
                    *ux_slave_endpoint_interface;
    USBXX::DeviceBase
                    *ux_slave_endpoint_device;
    UX_SLAVE_TRANSFER
                    ux_slave_endpoint_transfer_request;

    Endpoint() : used(false)
    {

    }

    virtual void reset_flags()
    {
      used = false;
    }

    virtual UINT create() = 0;
    virtual UINT destroy() = 0;
    virtual bool is_stalled() = 0;
    virtual UINT reset() = 0;
    virtual void stall() = 0;
  };
}
