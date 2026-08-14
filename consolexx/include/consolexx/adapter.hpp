#pragma once

#include <halxx/extern.hpp>
#include <threadxx/thread.hpp>
#include <threadxx/mutex.hpp>
#include <threadxx/queue.hpp>
#include <threadxx/eventflags.hpp>
#include <consolexx/io.hpp>
#include <consolexx/cooked.hpp>
#include <consolexx/raw.hpp>

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
      drop(_drop), adapter(_adapter), name(_name)  {

    }

    void set_drop(bool);

    void create();

    void sendc(int c);

    virtual void wait_started();

    virtual void flush();
    virtual void putc(int c);
    virtual int getc();
  };

  class terminal_adapter : public terminal
  {
  public:
    enum TerminalMode {
      RAW,
      COOKED
    };

  private:
    friend class queue_io;


    TerminalMode mode;

    //raw_terminal raw;
    cooked_terminal cooked;
    queue_io cooked_io;

    raw_terminal raw;
    queue_io raw_io;

    //TXX::queue<uint8_t> raw_in_queue;

    void flush() { io->flush(); }
    void wait_started() { io->wait_started(); }
    void putc(int c) { io->putc(c); };

    void on_char(int c) override;

  public:
    terminal_adapter(termobj *parent, termio *_main_io);

    cooked_terminal &get_cooked() { return cooked; }

    int output_handler(const char *buffer, size_t size) override;

    void set_mode(TerminalMode);

    void startup() override;
  };
};
