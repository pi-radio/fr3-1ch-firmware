#pragma once

#include <cstdint>

namespace USBXX
{
  static inline uint16_t usb_get_short(const uint8_t *address)
  {
    uint16_t retval;

    retval = (uint16_t) *address++;
    retval |=  ((uint16_t)(*address) << 8);

    /* Return to caller.  */
    return  retval;
  }

  static inline uint32_t usb_get_long(const uint8_t * address)
  {
    uint32_t value;

    value =   (uint32_t) *address++;
    value |=  (uint32_t)*address++ << 8;
    value |=  (uint32_t)*address++ << 16;
    value |=  (uint32_t)*address << 24;

    return value;
  }

  static inline uint32_t from_usb32(uint32_t v)
  {
    return usb_get_long((const uint8_t *)&v);
  }

  static inline void  usb_put_long(uint8_t * address, uint32_t value)
  {
    *address++ =  (uint8_t) (value & 0xff);
    *address++ =  (uint8_t) ((value >> 8) & 0xff);
    *address++ =  (uint8_t) ((value >> 16) & 0xff);
    *address =    (uint8_t) ((value >> 24) & 0xff);
  }

  static inline uint32_t to_usb32(uint32_t v)
  {
    uint32_t retval;

    usb_put_long((uint8_t *)&retval, v);

    return retval;
  }
}
