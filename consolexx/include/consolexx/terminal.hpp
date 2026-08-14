#pragma once

#include <consolexx/termobj.hpp>
#include <consolexx/io.hpp>

namespace consolexx
{
  class terminal : public termobj
  {
  protected:
    termio *io;

  public:
    terminal(termobj *parent, termio *_io) : termobj(parent), io(_io) {}

    virtual int output_handler(const char *buffer, size_t size) = 0;
  };
}
