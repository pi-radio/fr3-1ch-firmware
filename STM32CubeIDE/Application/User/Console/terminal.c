#include <terminal.h>
#include <console.h>
#include <main.h>
#include <stdio.h>
#include <stdarg.h>

#include <config_data.h>

#include "ux_device_cdc_acm.h"

TX_QUEUE terminal_rx_queue;
TX_QUEUE terminal_tx_queue;
TX_QUEUE terminal_usb_tx_queue;

#define RX_QUEUE_LEN 256
#define TX_QUEUE_LEN 256
#define USB_TX_QUEUE_LEN 256

#define TX_BUF_LEN 256

static ULONG rx_queue_data[RX_QUEUE_LEN];
static ULONG tx_queue_data[TX_QUEUE_LEN];
static ULONG usb_tx_queue_data[USB_TX_QUEUE_LEN];

static TX_MUTEX terminal_tx_mutex;

static TX_THREAD terminal_tx_thread;
static TX_THREAD terminal_usb_tx_thread;
static TX_THREAD terminal_rx_thread;

typedef enum {
  TERMINAL_STATE_GROUND,
  TERMINAL_STATE_ESC,
  TERMINAL_STATE_CSI_ENTRY,
  TERMINAL_STATE_CSI_PARAM,
  TERMINAL_STATE_CSI_INTER,

  NUM_TERMINAL_STATES
} terminal_state_t;

struct terminal;

typedef void (*terminal_handler_t)(struct terminal *, int);

#define TERMINAL_MAX_CSI_INTERMEDIATES  16
#define TERMINAL_MAX_CSI_PARAMETERS     16

struct csi {
  int command;
  int private_leader;
  int n_intermediates;
  int intermediates[TERMINAL_MAX_CSI_INTERMEDIATES];
  int intermediate_overflow;
  int n_parameters;
  int parameters[TERMINAL_MAX_CSI_PARAMETERS];
  int parameter_overflow;
};

struct terminal {
  int rx_count;
  int tx_count;
  int csi_count;
  int error_count;
  int invalid_char;

  int error_char;
  terminal_state_t error_state;

  int echo;
  int onlcr;

  terminal_state_t state;

  terminal_handler_t handlers[NUM_TERMINAL_STATES];

  struct csi csi;
};

typedef struct terminal terminal_t;

terminal_t term;

// Peek buffer
static int c_peek = 0;


static inline void lock_tx(void) {
  tx_mutex_get(&terminal_tx_mutex, TX_WAIT_FOREVER);
}

static inline void unlock_tx(void) {
  tx_mutex_put(&terminal_tx_mutex);
}

int _write(int file, char *ptr, int len);

void terminal_cs(const char *fmt, ...)
{
  int n;
  char str[128];
  va_list args;

  va_start(args, fmt);

  str[0] = 0x1B;
  str[1] = '[';

  n = vsnprintf(str + 2, sizeof(str) - 2, fmt, args);

  va_end(args);

  n += 2;

  if (n > sizeof(str)) {
    n = sizeof(str);
  }

  _write(0, str, n);
}

void terminal_reset() {
  terminal_cs("c");
}

void terminal_move_to(int y, int x)
{
  terminal_cs("%d;%dH", y, x);
}

void terminal_clear_screen() {
  terminal_cs("2J");
}

void terminal_cursor_save() {

}

void terminal_cursor_restore() {

}

void terminal_clear_line() {

}

void terminal_query_id() {
  terminal_cs("c");
}

void terminal_query_position() {
  terminal_cs("6n");
}

void terminal_refresh() {
#if 0
  terminal_query_id();
#endif

#if 0
  terminal_reset();
#endif

#if 0
  lock_tx();
  txchar(0x05);
  unlock_tx();

  lock_tx();
  txchar(0x1B); txchar('Z');
  unlock_tx();
#endif

  terminal_clear_screen();

#if 1
  terminal_query_position();
#endif

  terminal_move_to(5, 1);
  printf("Status: RX: %d TX: %d CSI: %d Errors: %d",
      term.rx_count, term.tx_count, term.csi_count, term.error_count);
  fflush(stdout);

  terminal_move_to(6, 1);
  printf("Ptr: 0x%8p UID %s,%ld,%ld,%ld Flash Size: %d Package Code: %4.4x",
      (VOID *)_board_config_data,
      U_ID.lot_id, U_ID.wafer_no, U_ID.wafer_x, U_ID.wafer_y,
      config_get_flash_size(), config_get_package_code());
  fflush(stdout);
}

// NO MUTEX
int txchar(int c) {
  int result;
  ULONG flags;

  tx_event_flags_get(&app_events, 1, TX_AND, &flags, TX_NO_WAIT);

  if (dtr_is_set()) {
    result = tx_queue_send(&terminal_tx_queue, &c, 1);
  } else {
    result = 1;
  }

  return result;
}

int rxchar_raw(void) {
  ULONG c;

  tx_queue_receive(&terminal_rx_queue, &c, TX_WAIT_FOREVER);

  if ((c < 0x20 || c > 0x7F) && c != 0x0A && c != 0x0D && c != 0x09) {
    printf("Invalid character: %ld\n", c);
    term.invalid_char++;
  }

  return c;
}


int peekchar(void) {
  if (c_peek != 0) {
    return c_peek;
  }

  c_peek = rxchar_raw();

  return c_peek;
}


int rxchar(void) {
  int c;

  if (c_peek != 0) {
    c = c_peek;
    c_peek = 0;
  } else {
    c = rxchar_raw();
  }

  return c;
}



int _write(int file, char *ptr, int len)
{
  // Set to finite timeout to
  int retval = len;

  lock_tx();

  while (len) {
    ULONG c = *ptr;

    if (c == '\n') {
      txchar('\r');
    }

    txchar(c);

    ptr++;
    len--;
  }

  unlock_tx();

  return retval;
}

void terminal_reset_csi(struct csi *csi)
{
  memset(csi, 0, sizeof(*csi));
}

void terminal_set_state(struct terminal *term, terminal_state_t state)
{
  term->state = state;

  if (state == TERMINAL_STATE_CSI_ENTRY) {
    terminal_reset_csi(&term->csi);
  }
}

void terminal_handle_vt100_esc(struct terminal *term, int c) {

  terminal_set_state(term, TERMINAL_STATE_GROUND);
}

void terminal_handle_error(struct terminal *term, int c) {
  term->error_count++;
  term->error_char = c;
  term->error_state = term->state;

  terminal_set_state(term, TERMINAL_STATE_GROUND);
}

void terminal_handle_csi(struct terminal *term) {
  terminal_set_state(term, TERMINAL_STATE_GROUND);
  term->csi_count++;

  if (term->csi.n_parameters == 0) {
    term->csi.n_parameters = 1; // Default 0 parameter
  }


}


void terminal_handle_ground(struct terminal *term, int c)
{
  if (c == 0x1B) {
    terminal_set_state(term, TERMINAL_STATE_ESC);
  } else if (c == 0x9B) {
    terminal_set_state(term, TERMINAL_STATE_CSI_ENTRY);
  } else if (c == '\n') {
    if (term->echo) {
      lock_tx();

      if (term->onlcr) {
        txchar('\r');
      }
      txchar('\n');

      unlock_tx();
    }

    // EOL callback?
  } else if (c == '\t') {
    // Check for tab callback
    // Absorb tab
  } else if (c == '\r') {
    // Check for tab callback
    // Absorb tab
  } else if (c >= 0x20 && c <= 0x7F) {
    if (term->echo) {
      lock_tx();
      txchar(c);
      unlock_tx();
    }
  } else {
    // Invalid char callback
    terminal_handle_error(term, c);
  }
}


void terminal_handle_esc(struct terminal *term, int c)
{
  if (c == '[') {
    terminal_set_state(term, TERMINAL_STATE_CSI_ENTRY);
  } else if (c >= 0x30 && c <= 0x7F) {
    terminal_handle_vt100_esc(term, c);
    terminal_set_state(term, TERMINAL_STATE_GROUND);
  } else {
    // Invalid char callback
    terminal_handle_error(term, c);
  }
}

void terminal_handle_csi_inter(struct terminal *term, int c)
{
  if (c >= 0x20 && c <= 0x2F) {
    if (!term->csi.intermediate_overflow) {
      if (term->csi.n_intermediates <= TERMINAL_MAX_CSI_INTERMEDIATES) {
        term->csi.intermediates[term->csi.n_intermediates-1] = c;
        term->csi.n_intermediates++;
      } else {
        term->csi.intermediate_overflow = 1;
      }
    }
  } else if (c >= 0x40 && c <= 0x7E) {
    term->csi.command = c;
    terminal_handle_csi(term);
  } else {
    terminal_handle_error(term, c);
  }
}

void terminal_handle_csi_param(struct terminal *term, int c)
{
  if (c >= '0' && c <= '9') {
    if (!term->csi.parameter_overflow) {
      term->csi.parameters[term->csi.n_parameters - 1] *= 10;
      term->csi.parameters[term->csi.n_parameters - 1] += c - '0';
    }
  } else if (c == ';') {
    if (term->csi.n_parameters < TERMINAL_MAX_CSI_PARAMETERS) {
      term->csi.n_parameters++;
    } else {
      term->csi.parameter_overflow = 1;
    }
  } else if (c >= 0x20 && c <= 0x2F) {
    terminal_set_state(term, TERMINAL_STATE_CSI_INTER);
    terminal_handle_csi_inter(term, c);
  } else if (c >= 0x40 && c <= 0x7E) {
    term->csi.command = c;
    terminal_handle_csi(term);
  } else {
    terminal_handle_error(term, c);
  }
}

void terminal_handle_csi_entry(struct terminal *term, int c)
{
  if (c >= 0x3C && c <= 0x3F) {
    term->csi.private_leader = c;
    terminal_set_state(term, TERMINAL_STATE_CSI_PARAM);
  } else if (c >= '0' && c <= '9') {
    terminal_set_state(term, TERMINAL_STATE_CSI_PARAM);
    term->csi.n_parameters = 1;
    terminal_handle_csi_param(term, c);
  } else if (c >= 0x20 && c <= 0x2F) {
    terminal_set_state(term, TERMINAL_STATE_CSI_INTER);
    terminal_handle_csi_inter(term, c);
  } else if (c >= 0x40 && c <= 0x7F) {
    term->csi.command = c;
    terminal_handle_csi(term);
  } else {
    terminal_handle_error(term, c);
  }
}


VOID terminal_rx_process(struct terminal *term, int c)
{
  term->rx_count++;

  term->handlers[term->state](term, c);
}

VOID terminal_rx_thread_entry(ULONG _a) {
  while(1) {
    UCHAR c;
    ULONG e;
    ULONG len;
    UINT status;

    while(1) {
      do {
        wait_usb();

        status = ux_device_class_cdc_acm_read(cdc_acm, &c, 1, &len);
      } while(status == UX_TRANSFER_BUS_RESET);


      if ((status == TX_SUCCESS) && (len == 1)) {
        terminal_rx_process(&term, c);
        e = c;

        if (term.echo) {
          txchar(c);
        }

        tx_queue_send(&terminal_rx_queue, &e, TX_WAIT_FOREVER);
      }
    }
  }
}

void txchar_usb(ULONG c) {
  term.tx_count++;
  tx_queue_send(&terminal_usb_tx_queue, &c, TX_WAIT_FOREVER);
}

VOID terminal_tx_thread_entry(ULONG _a) {
  while(1) {
    ULONG c;

    tx_queue_receive(&terminal_tx_queue, &c, TX_WAIT_FOREVER);

    if(term.onlcr && c == 0x0A) {
      txchar_usb(0x0D);
    }

    txchar_usb(c);
  }
}

VOID terminal_usb_tx_thread_entry(ULONG _a) {
  UINT result;
  ULONG n, c;
  ULONG tx_len;
  UCHAR buf[TX_BUF_LEN];
  UCHAR *pcur;

  wait_usb();

  while (1) {
    n = 0;

    pcur = buf;
    result = tx_queue_receive(&terminal_usb_tx_queue, &c, TX_WAIT_FOREVER);
    *pcur++ = c;
    n++;

    while (n < TX_BUF_LEN) {
      result = tx_queue_receive(&terminal_usb_tx_queue, &c, 1);

      if (result != TX_SUCCESS) {
        break;
      }

      *pcur++ = c;
      n++;
    }

    pcur = buf;

    while (n) {
      do {
        wait_dtr();
        //tx_event_flags_get(&app_events, 1, TX_AND, &flags, TX_WAIT_FOREVER);

        result = ux_device_class_cdc_acm_write(cdc_acm, pcur, n, &tx_len);
      } while(result != TX_SUCCESS);

      pcur += tx_len;
      n -= tx_len;
    }
  }
}

static inline void terminal_struct_init(terminal_t *term) {
  memset(term, 0, sizeof(*term));

  term->echo = 1;
  term->onlcr = 1;

  term->state = TERMINAL_STATE_GROUND;

  term->handlers[TERMINAL_STATE_GROUND] = terminal_handle_ground;
  term->handlers[TERMINAL_STATE_ESC] = terminal_handle_esc;
  term->handlers[TERMINAL_STATE_CSI_ENTRY] = terminal_handle_csi_entry;
  term->handlers[TERMINAL_STATE_CSI_PARAM] = terminal_handle_csi_param;
  term->handlers[TERMINAL_STATE_CSI_INTER] = terminal_handle_csi_inter;
}


void terminal_init(TX_BYTE_POOL *pool) {
  int result;
  VOID *pStack;

  terminal_struct_init(&term);

  result = tx_queue_create(&terminal_rx_queue,
      "terminal_rx_queue",
      1,
      rx_queue_data,
      RX_QUEUE_LEN * sizeof(ULONG));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  result = tx_queue_create(&terminal_tx_queue,
      "terminal_tx_queue",
      1,
      tx_queue_data,
      TX_QUEUE_LEN * sizeof(ULONG));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  result = tx_queue_create(&terminal_usb_tx_queue,
      "terminal_usb_tx_queue",
      1,
      usb_tx_queue_data,
      USB_TX_QUEUE_LEN * sizeof(ULONG));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }


  tx_mutex_create(&terminal_tx_mutex, "Terminal Transmit Mutex", TX_INHERIT);

  tx_byte_allocate(pool, (VOID **)&pStack, 1024, TX_NO_WAIT);

  tx_thread_create(&terminal_rx_thread, "terminal_rx_thread_entry", terminal_rx_thread_entry, 1, pStack, 1024, 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

  tx_byte_allocate(pool, (VOID **)&pStack, 1024, TX_NO_WAIT);

  tx_thread_create(&terminal_tx_thread, "terminal_tx_thread_entry", terminal_tx_thread_entry, 1, pStack, 1024, 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

  tx_byte_allocate(pool, (VOID **)&pStack, 1024, TX_NO_WAIT);

  tx_thread_create(&terminal_usb_tx_thread, "terminal_usb_thread_entry", terminal_usb_tx_thread_entry, 1, pStack, 1024, 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);
}
