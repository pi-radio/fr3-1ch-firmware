#pragma once

namespace consolexx
{
  struct io
  {
    virtual void wait_started() { };

    virtual void flush() = 0;
    virtual void putc(int c) = 0;
    virtual int getc() = 0;
  };

  class usb_io : public io
  {
    USBXX::CDCACM &acm;

  public:
    usb_io(USBXX::CDCACM &_acm) : acm(_acm) {};

    virtual void wait_started() { acm.wait_started(); };

    virtual void flush() { acm.flush(); };
    virtual void putc(int c) { acm.putc(c); };
    virtual int getc() { return acm.getc(); };
  };



};
