#include <consolexx/adapter.hpp>

using namespace consolexx;


void queue_io::create()
{
  in_mutex.create();
  in_sema.create();
}

void queue_io::set_drop(bool _drop)
{
  drop = _drop;
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
    terminal("Adapter Terminal", parent, _main_io),
    mode(RAW),
    cooked_io(this, "Cooked IO", true),
    raw_io(this, "Raw IO", false),
    cooked(this, &cooked_io),
    raw(this, &raw_io)
{

}

void terminal_adapter::on_char(int c)
{
  switch(mode)
  {
  case COOKED:
    cooked_io.sendc(c);
    break;
  case RAW:
    raw_io.sendc(c);
    break;
  default:
    throw std::runtime_error("Invalid Mode");
  }
}

void terminal_adapter::startup()
{
  terminal::startup();

  cooked_io.create();
  raw_io.create();

  cooked.startup();
  raw.startup();
}

void terminal_adapter::set_mode(TerminalMode _mode)
{
  if (mode == _mode) {
    return;
  }

  mode = _mode;

  switch(mode)
  {
  case COOKED:
    cooked_io.set_drop(false);
    raw_io.set_drop(true);

    cooked.redraw();
    break;
  case RAW:
    cooked_io.set_drop(true);
    raw_io.set_drop(false);
    break;
  default:
    throw std::runtime_error("Invalid Mode");
  }
}


//void terminal_adapter::beep()
//{
//  cooked.beep();
//}

int terminal_adapter::output_handler(const char *buffer, size_t size)
{
  switch(mode)
  {
  case COOKED:
    return cooked.output_handler(buffer, size);
  case RAW:
    cooked.output_handler(buffer, size);
    return raw.output_handler(buffer, size);
  default:
    throw std::runtime_error("Invalid Mode");
  }
}
