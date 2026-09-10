#include <consolexx/terminal.hpp>

using namespace consolexx;

void terminal::_rx_thread()
{
  io->wait_started();

  while(1) {
    on_char(io->getc());
  }
}
