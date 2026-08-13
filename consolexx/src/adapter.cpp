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


terminal_adapter::terminal_adapter(termio *_main_io) : main_io(_main_io),
    rx_thread("Terminal Adapter RX Thread", this, &terminal_adapter::_rx_thread),
    cooked_io(this, "Cooked IO", false),
    cooked(&cooked_io)
{

}

void terminal_adapter::_rx_thread()
{
  while(1) {
    cooked_io.sendc(main_io->getc());
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

