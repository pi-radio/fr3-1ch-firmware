#pragma once

#include <consolexx/terminal.hpp>

namespace consolexx
{
  class raw_terminal : public terminal
  {
    std::string input_buffer;

  public:
    raw_terminal(termobj *_parent, termio *_io);

    int output_handler(const char *buffer, size_t size) override;
    void on_char(int c) override;
  };
}
