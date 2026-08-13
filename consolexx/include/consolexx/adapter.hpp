#pragma once

#include <halxx/extern.hpp>
#include <threadxx/thread.hpp>
#include <threadxx/queue.hpp>
#include <threadxx/mutex.hpp>
#include <usbxx/cdcacm.hpp>
#include <consolexx/io.hpp>
#include <consolexx/cooked.hpp>
#include <consolexx/raw.hpp>

namespace consolexx
{
  class terminal_adapter;

  class queue_io : public io
  {
    TXX::queue<uint8_t> in_queue;
    terminal_adapter *adapter;
    std::string name;

    queue_io(terminal_adapter *_adapter, const std::string &_name) : adapter(_adapter), name(_name) {}

    void create() { in_queue.create(); }

    void sendc(int c);

    virtual void wait_started() { };

    virtual void flush();
    virtual void putc(int c);
    virtual int getc();
  };

  class terminal_adapter
  {
    enum {
      RAW,
      COOKED
    } mode;

    raw_terminal raw;
    cooked_terminal cooked;

    TXX::queue<uint8_t> raw_in_queue;

    usb_io main_io;

    TXX::MemberThread<terminal_adapter, 4096> rx_thread;

  public:
    terminal_adapter(USBXX::CDCACM &_usb);
  };
};
