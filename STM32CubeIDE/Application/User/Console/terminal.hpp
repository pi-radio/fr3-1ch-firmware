#include <main.h>
#include <app_threadx.h>

#define TERMINAL_CMD_NOOP          0
#define TERMINAL_CMD_REDRAW        1
#define TERMINAL_CMD_FLUSH         2
#define TERMINAL_CMD_CLEAR_OUTPUT  3


EXTERN_C void terminal_init();
EXTERN_C int terminal_send_command(int cmd);

#if defined(__cplusplus)
#include <termbuf.hpp>
#include <text_field.hpp>
#include <vtparser.hpp>
#include <functional>

#define UP      0x80000000
#define DOWN    0x80000001
#define LEFT    0x80000002
#define RIGHT   0x80000003

#define TERMINAL_FLUSH  0xFFFF0000

#define RX_QUEUE_LEN 64
#define TX_QUEUE_LEN 64
#define CMD_QUEUE_LEN 8
#define USB_TX_QUEUE_LEN 64

#define TX_BUF_LEN 256


template <class C, size_t stack_size, int prio=20, int prempt=20> class TXThread {
  TX_THREAD _thread;
  C *_p;
  void (C::*_mf)();

  static void s_entry(ULONG _p) {
    TXThread *t = (TXThread *)_p;
    t->m_entry();
  }

  void m_entry() {
    (_p->*_mf)();
  }

public:
  TXThread(C *x, void (C::*mf)()) {
    _p = x;
    _mf = mf;
  }

  void create(C *x, TX_BYTE_POOL *pool, const char *name) {
    void *pstack;

    tx_byte_allocate(pool, &pstack, stack_size, TX_NO_WAIT);

    tx_thread_create(&_thread, (char *)name, s_entry, (ULONG)this, pstack, stack_size,
        prio, prempt,
        TX_NO_TIME_SLICE, TX_AUTO_START);
  }
};

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

  TX_SEMAPHORE flush_sema;
  TX_BYTE_POOL *pool;

  vtparser vtp;

  termbuf *_outbuf;

  ULONG rx_queue_data[RX_QUEUE_LEN];
  ULONG tx_queue_data[TX_QUEUE_LEN];
  ULONG cmd_queue_data[CMD_QUEUE_LEN];
  TX_QUEUE rx_queue;
  TX_QUEUE tx_queue;
  TX_QUEUE cmd_queue;

  // Should be protected/private
  void _rx_thread();
  void _tx_thread();
  void _refresh_thread();

  //static TX_MUTEX tx_mutex;

  TXThread<terminal, 4096> rx_thread;
  TXThread<terminal, 4096> tx_thread;
  TXThread<terminal, 4096> refresh_thread;

  //TX_THREAD rx_thread;
  //TX_THREAD tx_thread;
  //TX_THREAD refresh_thread;

  int c_peek;
  int last_dtr;

  friend int terminal_send_command(int cmd);

  int rxchar_raw(void);

  void usb_flush_buffer();
  void usb_txchar(uint8_t);

  void emit_cs(const char *fmt, ...);
  int printf(const char *fmt, ...);

  void enqueue_cmd(int cmd) {
    tx_queue_send(&cmd_queue, &cmd, TX_WAIT_FOREVER);
  }

  void redraw(void);

public:
  terminal(TX_BYTE_POOL *_pool);

  termbuf *outbuf() { return _outbuf; }

  int txchar(uint32_t c);
  void lock() {};
  void unlock() {};

  void strout(const uint8_t *s, int len);

  void strout(const char *s, int len) {
    strout((const uint8_t *)s, len);
  }

  void flush() {
    txchar(TERMINAL_FLUSH);

    tx_semaphore_get(&flush_sema, TX_WAIT_FOREVER);
  }

  void on_input(int c) { _outbuf->on_input(c); };

  void refresh(void);

  int peekchar();
  int rxchar();

  position query_position();

  void clear_line() { emit_cs("K"); };

  void clear_screen() { emit_cs("2J"); };

  void query_id() { emit_cs("c"); }

  void erase_right(uint32_t cols) { emit_cs("%dX", cols); };

  void move_to(uint32_t row, uint32_t col) { emit_cs("%d;%dH", row + 1, col + 1); };
  void move_to(position p) { move_to(p.row, p.col); };

  void echo_off() { emit_cs("12h"); };

  void reset() { emit_cs("c"); };

  template <class T> T *create(uint32_t start_row, uint32_t start_col, uint32_t height, uint32_t width) {
    return _outbuf->create<T>(start_row, start_col, height, width);
  }

  void set_focus(window *win) { _outbuf->set_focus(win); }
  //window *create_window(uint32_t start_row, uint32_t start_col, uint32_t height, uint32_t width);
  //text_field *create_text_field(uint32_t start_row, uint32_t start_col, uint32_t height, uint32_t width);

  void draw(position p, const uint8_t *buf, size_t len) override;
};

#endif

extern int force_redraw;


#if 0
extern struct window output_win;

void terminal_reset();
void terminal_cursor_move(int x, int y);
void terminal_cursor_save();
void terminal_cursor_restore();
void terminal_clear_line();

void txflush(void);

extern void terminal_txchar(int c);
extern int terminal_rxchar();

#endif
