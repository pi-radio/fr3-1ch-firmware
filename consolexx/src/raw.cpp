#include <consolexx/raw.hpp>

using namespace consolexx;

raw_terminal::raw_terminal(termobj *_parent, termio *_io) :
  terminal("Raw Terminal", _parent, _io)
{

}

int raw_terminal::output_handler(const char *buffer, size_t size)
{
  for(size_t i = 0; i < size; i++) {
    io->putc(*buffer++);
  }

  return size;
}

void raw_terminal::on_char(int c)
{
  if (c == '\n' || c == '\r') {
    emit<input_event>(input_buffer);
    input_buffer.clear();
  } else {
    input_buffer += c;
  }
}
