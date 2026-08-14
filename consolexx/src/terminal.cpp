#include <consolexx/terminal.hpp>

using namespace consolexx;

void terminal::_rx_thread()
{
  while(1) {
    on_char(io->getc());
  }
}
