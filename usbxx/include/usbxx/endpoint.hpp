#pragma once

#include <cstdint>
#include <memory>

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
        throw USBXX::runtime_error("Invalid request recipient");
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
        throw USBXX::runtime_error("Invalid request type");
      }

      code = buffer[REQUEST_CODE];
      value = usb_get_short(buffer + REQUEST_VALUE);
      index = usb_get_short(buffer + REQUEST_INDEX);
      length = usb_get_short(buffer + REQUEST_LENGTH);
    }
  };

  struct Endpoint : public std::enable_shared_from_this<Endpoint>
  {
    using ptr = std::shared_ptr<Endpoint>;

    EndpointDescriptor         descriptor;
    USBXX::DeviceBase          *device;
    std::shared_ptr<Interface> interface;

    Endpoint(USBXX::DeviceBase *_device);

    virtual ~Endpoint();

    virtual void reset_flags()
    {
    }

    virtual void abort_all_transfers()
    {

    }

    bool is_control() { return (descriptor.bEndpointAddress & 0x7F) == 0; }

    virtual Transfer *get_transfer() = 0;

    virtual UINT create() = 0;
    virtual UINT destroy() = 0;
    virtual bool is_stalled() = 0;
    virtual UINT reset() = 0;
    virtual void stall() = 0;
    virtual void abort_all_transfers(uint32_t) = 0;
    virtual void ack_ctrl() = 0;
  };
}
