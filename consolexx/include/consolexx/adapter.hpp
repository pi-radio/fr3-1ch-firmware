#pragma once

#include <halxx/extern.hpp>
#include <threadxx/thread.hpp>
#include <threadxx/mutex.hpp>
#include <usbxx/cdcacm.hpp>
#include <consolexx/io.hpp>
#include <consolexx/cooked.hpp>
#include <consolexx/raw.hpp>
#include <threadxx/queue.hpp>
#include <threadxx/eventflags.hpp>

namespace consolexx
{
  class terminal_adapter;

  class queue_io : public termio
  {
    TXX::Mutex in_mutex;
    TXX::Semaphore in_sema;

    std::deque<uint8_t> in_queue;

    bool drop;

    terminal_adapter *adapter;
    std::string name;

  public:
    queue_io(terminal_adapter *_adapter, const std::string &_name, bool _drop) :
      in_mutex(_name + " in mutex"),
      in_sema(_name + " in sema"),
      adapter(_adapter), name(_name), drop(_drop)  {

    }

    void create();

    void sendc(int c);

    virtual void wait_started();

    virtual void flush();
    virtual void putc(int c);
    virtual int getc();
  };

  class terminal_adapter
  {
    friend class queue_io;

    enum {
      RAW,
      COOKED
    } mode;

    termio *main_io;

    //raw_terminal raw;
    cooked_terminal cooked;
    queue_io cooked_io;

    //TXX::queue<uint8_t> raw_in_queue;

    TXX::MemberThread<terminal_adapter, 4096> rx_thread;

    void _rx_thread();

    void flush() { main_io->flush(); }
    void wait_started() { main_io->wait_started(); }
    void putc(int c) { main_io->putc(c); };

  public:
    terminal_adapter(termio *_main_io);

    cooked_terminal &get_cooked() { return cooked; }

    void startup();
    void beep();
  };
};
