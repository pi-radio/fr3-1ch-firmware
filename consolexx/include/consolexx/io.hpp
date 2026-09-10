#pragma once

#include <usbxx/cdcacm.hpp>

namespace consolexx
{
  struct termio
  {
    virtual void wait_started() { };

    virtual void flush() = 0;
    virtual void putc(int c) = 0;
    virtual int getc() = 0;
  };

  class usb_io : public termio
  {
    USBXX::CDCACMDevice &acm;

  public:
    usb_io(USBXX::CDCACMDevice &_acm) : acm(_acm) {};

    virtual void wait_started()
    {
      auto cdcacm = acm.get_cdcacm();

      cdcacm->wait_activated();
    };

    virtual void flush()
    {
      auto cdcacm = acm.get_cdcacm();

      cdcacm->flush();
    };

    virtual void putc(int c)
    {
      auto cdcacm = acm.get_cdcacm();

      cdcacm->putc(c);
    };

    virtual int getc()
    {
      auto cdcacm = acm.get_cdcacm();

      return cdcacm->getc();
    };
  };



};
