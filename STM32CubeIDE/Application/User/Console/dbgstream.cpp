/*
 * dbgstream.cpp
 *
 *  Created on: Feb 18, 2026
 *      Author: zapman
 */
#include <vector>

#include <dbgstream.hpp>
#include <usart.h>
#include <stdarg.h>

template <size_t n>
class dbgbuf : public std::streambuf
{
  char_type *_buffer;
  char_type *_pcur;
  char_type last;

  bool uart_ready;

protected:
  int sync() override
  {
    if (uart_ready && pready()) {
      HAL_UART_Transmit(&huart1, (const uint8_t *)_buffer, pready(), 0xFFFF);
      _pcur = _buffer;
    }
    return 0;
  }

  bool full() { return pready() == n; }

  std::streamsize pready() { return _pcur - _buffer; }

  inline bool test_eof(const char_type c)
  {
    return traits_type::eq_int_type(c, traits_type::eof());
  }

  void pushc(char_type c)
  {
    if (pready() + 1 > n) {
      sync();
    }

    if (full()) {
      // Just drop it on the floor for now...
      return;
    }

    last = *_pcur++ = c;
  }

  virtual int_type overflow(int_type c) override
  {
    if (test_eof(c)) {
      sync();
    }

    if (c == '\n' && last != '\r') {
      pushc('\r');
    }

    pushc(c);

    return 1;
  }

public:
  dbgbuf() : uart_ready(false)
  {
    _pcur = _buffer = new char_type[n];

    for (auto c : "\e[2J\e[H") {
      if (c) {
        pushc(c);
      }
    }
  }

  void set_uart_ready()
  {
    uart_ready = true;
    sync();
  }
};

auto dbgbuffer = new dbgbuf<1024>();
std::ostream dbgout(dbgbuffer);

extern "C" void set_uart_ready()
{
  dbgbuffer->set_uart_ready();
}

extern "C" int dbgprint(const char *fmt, ...)
{
  size_t retval;

  va_list args;

  va_start(args, fmt);

  retval = vsnprintf(NULL, 0, fmt, args);

  va_end(args);

  std::vector<char> serbuf(retval + 1);

  va_start(args, fmt);

  retval = vsnprintf(&serbuf[0], retval + 1, fmt, args);

  va_end(args);

  char prev = 0;

  for (size_t i = 0; i < retval; i++) {
    if (serbuf[i] == '\n' && prev != '\r') {
      dbgout.put('\r');
    }

    dbgout.put(serbuf[i]);

    if (serbuf[i] == '\n')
      dbgout.flush();

    prev = serbuf[i];
  }

  return retval;

}
