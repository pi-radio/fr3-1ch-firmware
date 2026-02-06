#include <console.h>
#include <main.h>
#include <stdio.h>
#include <stdarg.h>
#include <config_data.h>
#include <lexer.h>
#include <parser.h>
#include <dts.h>
#include <terminal.hpp>
#include <text_field.hpp>
#include <window.hpp>
#include <vtparser.hpp>
#include <usart.h>

#include "ux_device_cdc_acm.h"
#include <main.h>


int force_redraw;


terminal::terminal(TX_BYTE_POOL *_pool) :
    rx_count(0), tx_count(0), invalid_char(0),
    buffer_input(1), input_len(0),
    tx_len(0),  echo(0), onlcr(1),
    pool(_pool), vtp(this),
    rx_thread(this, &terminal::_rx_thread),
    tx_thread(this, &terminal::_tx_thread),
    refresh_thread(this, &terminal::_refresh_thread),
    c_peek(0), last_dtr(0)
{
  int result;
  //void *pstack;
  memset(input_buf, 0, sizeof(input_buf));
  memset(tx_buf, 0, sizeof(tx_buf));

  tx_semaphore_create(&flush_sema, (char *)"Terminal Flush Semaphore", 0);

  result = tx_queue_create(&rx_queue,
      (char *)"terminal_rx_queue",
      1,
      rx_queue_data,
      sizeof(rx_queue_data));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  result = tx_queue_create(&tx_queue,
      (char *)"terminal_tx_queue",
      1,
      tx_queue_data,
      sizeof(tx_queue_data));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  result = tx_queue_create(&cmd_queue,
      (char *)"terminal_cmd_queue",
      1,
      cmd_queue_data,
      sizeof(cmd_queue_data));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  rx_thread.create(this, pool, "RX thread");
  tx_thread.create(this, pool, "TX thread");
  refresh_thread.create(this, pool, "Refresh thread");

  tx_cur = tx_buf;

  _outbuf = new termbuf(pool, 40, 132);

  _outbuf->set_render_engine(this);
}

void terminal::draw(position p, const uint8_t *buf, size_t len)
{
  ord_t end = p.col + len;
  ord_t right = _outbuf->get_rect().right();

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

void terminal::usb_flush_buffer() {
  int result;
  UCHAR *p = tx_buf;
  ULONG l = tx_len;

  while (l) {
    do {
      wait_dtr();
      //tx_event_flags_get(&app_events, 1, TX_AND, &flags, TX_WAIT_FOREVER);

      result = ux_device_class_cdc_acm_write(cdc_acm, p, l, &tx_len);
    } while(result != TX_SUCCESS);

    l -= tx_len;
    p += tx_len;
  }

  tx_cur = tx_buf;
  tx_len = 0;
}

void terminal::usb_txchar(UCHAR c)
{
  *tx_cur++ = c;
  tx_len++;

  if (tx_len == TX_BUF_LEN) {
    usb_flush_buffer();
  }
}

int terminal::txchar(uint32_t c)
{
  return tx_queue_send(&tx_queue, &c, 1);
}

int terminal::rxchar_raw(void)
{
  ULONG c;

  tx_queue_receive(&rx_queue, &c, TX_WAIT_FOREVER);

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
    UCHAR rxbuf[64];
    UCHAR *p;
    ULONG len;
    UINT status;

    while(1) {
      do {
        wait_usb();

        status = ux_device_class_cdc_acm_read(cdc_acm, rxbuf, 64, &len);
      } while(status == UX_TRANSFER_BUS_RESET);

      if (status != TX_SUCCESS) {
        printf("Read status: %d\n", status);
        continue;
      }

      p = rxbuf;

      while(len--) {
        vtp.process(*p++);
      }
    }
  }
}

void terminal::_tx_thread() {
  int result;
  ULONG c;
  ULONG wait;

  dbgprint("_txthread\r\n");

  wait_usb();

  dbgprint("tx usb attach\r\n");


  wait = TX_WAIT_FOREVER;

  while(1) {
    result = tx_queue_receive(&tx_queue, &c, wait);

    if (result == TX_QUEUE_EMPTY) {
      usb_flush_buffer();
      wait = TX_WAIT_FOREVER;
      continue;
    }

    if (result != TX_SUCCESS) {
      Error_Handler();
    }

    wait = 1;

    if (c == TERMINAL_FLUSH) {
      usb_flush_buffer();
      tx_semaphore_put(&flush_sema);
      wait = TX_WAIT_FOREVER;
      continue;
    }

    tx_count++;

    if(onlcr && c == 0x0A) {
      usb_txchar(0x0D);
    }

    usb_txchar(c);
  }
}


void terminal::_refresh_thread(void)
{
  ULONG cmd;
  ULONG wait = 40;
  int dtr;
  int result;

  while (1) {
    result = tx_queue_receive(&cmd_queue, &cmd, wait);

    if (result == 0) {
      switch (cmd) {
      case TERMINAL_CMD_REDRAW:
        _outbuf->redraw();
        continue;
      }
    }

    //struct terminal *term = (struct terminal *)param;

    dtr = dtr_is_set();

    if (last_dtr != dtr) {
      last_dtr = dtr;

      if (dtr) {
        _outbuf->redraw();
        continue;
      }
    }

    if (dtr) {
      _outbuf->refresh();
    }
  }
}

terminal *term = NULL;


static window *output_win = NULL;
static window *status_win = NULL;
static text_field *input_win = NULL;

static TX_TIMER status_timer;

EXTERN_C int terminal_send_command(int cmd)
{
  switch (cmd) {
  case TERMINAL_CMD_NOOP:
    return 0;
  case TERMINAL_CMD_REDRAW:
    term->enqueue_cmd(cmd);
    return 0;
  case TERMINAL_CMD_FLUSH:
    // has to go through the character stream to be sure
    // we flush to this point
    term->flush();
    return 0;
  case TERMINAL_CMD_CLEAR_OUTPUT:
    output_win->clear();
    return 0;
  default:
    return -1;
  }
}


EXTERN_C int _write(int file, char *ptr, int len);

class input_callbacks : public text_field_callbacks
{
  terminal *_term;

public:
  input_callbacks(terminal *term) : _term(term) {}

  void beep() override { _term->txchar('\a'); }

  void on_cr(const uint8_t *s, size_t l) {
    output_win->printf("%.*s\n", l, s);

    lexer_set_line((const char *)s, l);

    console_cmd_ready();

    int result = parser_parse_statement();

    switch(result) {
    case PARSER_OK:
      printf("OK\n");
      break;
    case PARSER_SYNTAX_ERROR:
      printf("Syntax Error: (%d) %s\n", l, s);
      break;
    }
  }
};




EXTERN_C int _write(int file, char *ptr, int len)
{
  if (output_win == NULL) {
    return len;
  }
  return output_win->write(ptr, len);
}

void status_update(ULONG a)
{
  struct window *win = (struct window *)a;
  int result;
  int32_t temp;

  result = HAL_DTS_GetTemperature(&hdts, &temp);

  if (result == TX_SUCCESS) {
    win->printf(position(0,0), "Temp: %dC", temp);
  } else {
    win->printf(position(0,0), "Temp: ERROR");
  }
}

int input_on_nl(struct text_field *tf, const char *s, int l)
{
  return 0;
}


#define TERMINAL_POOL_SIZE 32768

static UCHAR terminal_pool_data[TERMINAL_POOL_SIZE];
static TX_BYTE_POOL terminal_pool;

extern "C" void terminal_init() {
  tx_byte_pool_create(&terminal_pool, (char *)"terminal memory pool", terminal_pool_data, TERMINAL_POOL_SIZE);

  term = new terminal(&terminal_pool);


  output_win = term->create<window>(8, 0, 8, 132);
  status_win = term->create<window>(20, 0, 1, 132);

  input_win = term->create<text_field>(17, 0, 1, 132);

  term->set_focus(input_win);

  input_callbacks *callbacks = new input_callbacks(term);

  input_win->set_callbacks(callbacks);

  /*
  tx_timer_create(&status_timer,
      (char *)"Status Timer",
      status_update,
      (ULONG)status_win,
      1000,
      1000,
      TX_AUTO_ACTIVATE);
      */
}
