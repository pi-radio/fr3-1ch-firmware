#pragma once

#include <halxx/extern.hpp>
#include <threadxx/thread.hpp>
#include <threadxx/queue.hpp>
#include <threadxx/mutex.hpp>
#include <usbxx/cdcacm.hpp>

#define TERMINAL_CMD_NOOP          0
#define TERMINAL_CMD_REDRAW        1
#define TERMINAL_CMD_FLUSH         2
#define TERMINAL_CMD_CLEAR_OUTPUT  3


//EXTERN_C int terminal_send_command(int cmd);

#include <consolexx/termbuf.hpp>
#include <consolexx/text_field.hpp>
#include <consolexx/vtparser.hpp>
#include <functional>

#include <deque>

#define UP      0x80000000
#define DOWN    0x80000001
#define LEFT    0x80000002
#define RIGHT   0x80000003

#define TERMINAL_FLUSH  0xFFFF0000

#define CMD_QUEUE_LEN 8
#define USB_TX_QUEUE_LEN 64

#define TX_BUF_LEN 256

class terminal : public text_field_callbacks,
                 public termbuf_render_engine {
  int rx_count;
  int tx_count;
  int invalid_char;
  int buffer_input;

  char input_buf[128];
  int input_len;

  uint8_t tx_buf[TX_BUF_LEN];
  uint8_t *tx_cur;
  uint32_t tx_len;


  int echo;
  int onlcr;

  vtparser vtp;

  termbuf *_outbuf;

  // Should be protected/private
  void _rx_thread();
  void _refresh_thread();

  //static TX_MUTEX tx_mutex;

  TXX::Queue<1, CMD_QUEUE_LEN> cmd_queue;
  
  TXX::MemberThread<terminal, 4096> rx_thread;
  TXX::MemberThread<terminal, 4096> refresh_thread;

  TXX::Mutex input_mutex;
  std::deque<std::string> input_lines;

  //TX_THREAD rx_thread;
  //TX_THREAD tx_thread;
  //TX_THREAD refresh_thread;

  int c_peek;
  int last_dtr;

  friend int terminal_send_command(int cmd);

  int rxchar_raw(void);

  void emit_cs(const char *fmt, ...);

  void enqueue_cmd(int cmd) {
    cmd_queue.send(cmd);
  }

  int peekchar();
  int rxchar();
  void txchar(uint32_t c);

  USBXX::CDCACM &usb_cdc_acm;
  
public:
  terminal(USBXX::CDCACM &_usb);

  void startup();

  termbuf *outbuf() { return _outbuf; }

  void lock() {};
  void unlock() {};

  void on_input(int c) { _outbuf->on_input(c); };

  void refresh(void);

  void beep() { txchar('\a'); }

  position query_position();

  void clear_line() { emit_cs("K"); };

  void clear_screen() { emit_cs("2J"); };

  void query_id() { emit_cs("c"); }

  void erase_right(uint32_t cols) { emit_cs("%dX", cols); };

  void move_to(uint32_t row, uint32_t col) { emit_cs("%d;%dH", row + 1, col + 1); };
  void move_to(position p) { move_to(p.row, p.col); };
  void move_right(uint32_t cols) { emit_cs("%dC", cols); }

  void echo_off() { emit_cs("12h"); };

  void reset() { emit_cs("c"); };

  void redraw();

  template <class T> T *create(uint32_t start_row, uint32_t start_col, uint32_t height, uint32_t width) {
    return _outbuf->create<T>(start_row, start_col, height, width);
  }

  void set_focus(window *win) { _outbuf->set_focus(win); }

  void draw(position p, const uint8_t *buf, size_t len) override;
};

