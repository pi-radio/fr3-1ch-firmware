#pragma once

#include <consolexx/termobj.hpp>
#include <consolexx/io.hpp>

namespace consolexx
{
  class terminal : public termobj
  {
  protected:
    termio *io;

    TXX::MemberThread<terminal, 8192> rx_thread;

    void _rx_thread();

  public:
    terminal(const std::string &name, termobj *parent, termio *_io) :
      termobj(parent),
      io(_io),
      rx_thread(name + " Thread", this, &terminal::_rx_thread) {}

    virtual void startup() { rx_thread.create(); }

    virtual int output_handler(const char *buffer, size_t size) = 0;

    virtual void on_char(int c) = 0;
  };
}
