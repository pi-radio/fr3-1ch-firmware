#include <console.h>
#include <main.h>
#include <stdio.h>
#include <stdarg.h>
#include <dts.h>
#include <consolexx/terminal.hpp>
#include <consolexx/text_field.hpp>
#include <consolexx/window.hpp>
#include <consolexx/vtparser.hpp>
#include <usart.h>

#include "ux_device_cdc_acm.h"
#include <main.h>


int force_redraw;


terminal::terminal(USBXX::CDCACM &cdc) :
    rx_count(0), tx_count(0), invalid_char(0),
    buffer_input(1), input_len(0),
    tx_len(0),  echo(0), onlcr(1),
    vtp(this),
    cmd_queue("Terminal Command Queue"),
    rx_thread("Terminal RX Thread", this, &terminal::_rx_thread),
    refresh_thread("Terminal Refresh Thread", this, &terminal::_refresh_thread),
    input_mutex("Terminal Input Mutex"),
    c_peek(0), last_dtr(0),
    usb_cdc_acm(cdc)
{
  //void *pstack;
  memset(input_buf, 0, sizeof(input_buf));
  memset(tx_buf, 0, sizeof(tx_buf));

  //rx_queue.create("Terminal RX Queue");
  //tx_queue.create("Terminal TX Queue");
  cmd_queue.create();
  
  tx_cur = tx_buf;

  _outbuf = new termbuf(40, 132);

  _outbuf->set_render_engine(this);
}

void terminal::startup()
{
  rx_thread.create();
  //tx_thread.create();
  refresh_thread.create();
}


void terminal::draw(position p, const uint8_t *buf, size_t len)
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


position terminal::query_position()
{
  position pos;

  emit_cs("6n");

  return pos;
}


void terminal::emit_cs(const char *fmt, ...)
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

void terminal::txchar(uint32_t c)
{
  if(onlcr && c == 0x0A) {
    usb_cdc_acm.putc(0x0D);
  }
  
  usb_cdc_acm.putc(c);
}

int terminal::rxchar_raw(void)
{
  ULONG c = usb_cdc_acm.getc();

  if ((c < 0x20 || c > 0x7F) && c != 0x0A && c != 0x0D && c != 0x09) {
    printf("Invalid character: %ld\n", c);
    invalid_char++;
  }

  return c;
}


int terminal::peekchar(void)
{
  if (c_peek != 0) {
    return c_peek;
  }

  c_peek = rxchar_raw();

  return c_peek;
}


int terminal::rxchar(void)
{
  int c;

  if (c_peek != 0) {
    c = c_peek;
    c_peek = 0;
  } else {
    c = rxchar_raw();
  }

  return c;
}



void terminal::_rx_thread()
{
  dbgprint("_rxthread\r\n");

  while(1) {
    vtp.process(usb_cdc_acm.getc());
  }
}

void terminal::redraw()
{
  cmd_queue.send(TERMINAL_CMD_REDRAW);
}

void terminal::_refresh_thread(void)
{
  ULONG cmd;
  ULONG wait = 40;

  usb_cdc_acm.wait_started();

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

int input_on_nl(struct text_field *tf, const char *s, int l)
{
  return 0;
}
