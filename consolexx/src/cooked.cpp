#include <console.h>
#include <consolexx/cooked.hpp>
#include <main.h>
#include <stdio.h>
#include <stdarg.h>
#include <dts.h>
#include <consolexx/text_field.hpp>
#include <consolexx/window.hpp>
#include <consolexx/vtparser.hpp>
#include <consolexx/termbuf.hpp>
#include <usart.h>

#include "ux_device_cdc_acm.h"
#include <main.h>


int force_redraw;

using namespace consolexx;

cooked_terminal::cooked_terminal(termobj *parent, termio *_io) :
    terminal(parent, _io),
    rx_count(0), tx_count(0), invalid_char(0),
    buffer_input(1), input_len(0),
    tx_len(0),  echo(0), onlcr(1),
    vtp(this),
    cmd_queue("Terminal Command Queue"),
    rx_thread("Terminal RX Thread", this, &cooked_terminal::_rx_thread),
    refresh_thread("Terminal Refresh Thread", this, &cooked_terminal::_refresh_thread),
    input_mutex("Terminal Input Mutex"),
    c_peek(0), last_dtr(0),
    default_output(nullptr)
{
  //void *pstack;
  memset(input_buf, 0, sizeof(input_buf));
  memset(tx_buf, 0, sizeof(tx_buf));

  //rx_queue.create("Terminal RX Queue");
  //tx_queue.create("Terminal TX Queue");
  cmd_queue.create();
  
  tx_cur = tx_buf;

  _outbuf = new termbuf(this, 40, 132);
}

void cooked_terminal::startup()
{
  rx_thread.create();
  //tx_thread.create();
  refresh_thread.create();
}

void cooked_terminal::draw(position p, const uint8_t *buf, size_t len)
{
  ord_t end = p.col + len;
  ord_t right = _outbuf->rlocal().right();

  if (end > right) {
    len = right - p.col;
  }

  move_to(p);

  erase_right(len);

  int last_col = -1, cur_col = 0;

  while(len--) {
    uint8_t c = *buf++;

    if (c == 0) {
      cur_col++;
      continue;
    }

    if (last_col + 1 != cur_col) {
      move_right(cur_col - last_col);
    }

    txchar(c);

    last_col = cur_col;
    cur_col++;
  }
}


position cooked_terminal::query_position()
{
  position pos;

  emit_cs("6n");

  return pos;
}


void cooked_terminal::emit_cs(const char *fmt, ...)
{
  size_t n;
  const char prefix[] = { '\e', '[' };
  char str[128];
  char *p;
  va_list args;

  va_start(args, fmt);

  memcpy(str, prefix, sizeof(prefix));

  n = vsnprintf(str + sizeof(prefix), sizeof(str) - sizeof(prefix), fmt, args);

  va_end(args);

  n += sizeof(prefix);

  if (n > sizeof(str)) {
    n = sizeof(str);
  }

  p = str;

  while(n--) {
    txchar(*p++);
  }
}

#include <threadxx/ring_buffer.hpp>

TXX::ring_buffer_base<int, 32> tx_char_ring;
TXX::ring_buffer_base<int, 32> rx_char_ring;

void cooked_terminal::txchar(uint32_t c)
{
  if(onlcr && c == 0x0A) {
    tx_char_ring.pushc(0x0D);
    io->putc(0x0D);
  }
  
  tx_char_ring.pushc(c);
  io->putc(c);
}


void cooked_terminal::_rx_thread()
{
  while(1) {
    int c = io->getc();

    rx_char_ring.pushc(c);

    vtp.process(c);
  }
}

void cooked_terminal::redraw()
{
  cmd_queue.send(TERMINAL_CMD_REDRAW);
}

void cooked_terminal::_refresh_thread(void)
{
  ULONG cmd;
  ULONG wait = 40;

  io->wait_started();

  while (1) {
    try {
      cmd = cmd_queue.recv_wait(wait);

      switch (cmd) {
      case TERMINAL_CMD_REDRAW:
        _outbuf->redraw();
        continue;
      }
    } catch (TXX::QueueEmpty e) {
      _outbuf->refresh();
    }
  }
}

int cooked_terminal::output_handler(const char *buffer, size_t size)
{
  if (default_output == nullptr)
    return size;

  return default_output->write(buffer, size);
}



