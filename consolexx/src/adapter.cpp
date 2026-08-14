#include <consolexx/adapter.hpp>

using namespace consolexx;


void queue_io::create()
{
  in_mutex.create();
  in_sema.create();
}

void queue_io::flush()
{
  if (!drop) {
    adapter->flush();
  }
}

void queue_io::putc(int c)
{
  if (!drop) {
    adapter->putc(c);
  }
}

void queue_io::sendc(int c)
{
  if (drop)
    return;

  {
    TXX::Mutex::guard g(in_mutex);

    in_queue.emplace_back(c);
  }

  in_sema.put();
}

int queue_io::getc()
{
  while (true)
  {
    in_sema.get();

    {
      TXX::Mutex::guard g(in_mutex);

      auto retval = in_queue.front();

      in_queue.pop_front();

      if (!drop)
        return retval;
    }
  }
}

void queue_io::wait_started()
{
  adapter->wait_started();
};


terminal_adapter::terminal_adapter(termobj *parent, termio *_main_io) :
    terminal(parent, _main_io),
    mode(COOKED),
    rx_thread("Terminal Adapter RX Thread", this, &terminal_adapter::_rx_thread),
    cooked_io(this, "Cooked IO", false),
    cooked(this, &cooked_io)
{

}

void terminal_adapter::_rx_thread()
{
  while(1) {
    cooked_io.sendc(io->getc());
  }
}

void terminal_adapter::startup()
{
  rx_thread.create();

  cooked_io.create();

  cooked.startup();
}


void terminal_adapter::beep()
{
  cooked.beep();
}

int terminal_adapter::output_handler(const char *buffer, size_t size)
{
  switch(mode)
  {
  case COOKED:
    return cooked.output_handler(buffer, size);
  case RAW:
    //return raw.output_handler(buffer, size);
  default:
    throw std::runtime_error("Invalid Mode");
  }
}
