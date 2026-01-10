#include "ux_device_cdc_acm.h"

#include "console.h"
#include "app_threadx.h"

#define TX_BUF_LEN 128

#define TX_QUEUE_LEN 1024
#define RX_QUEUE_LEN 1024

TX_QUEUE console_tx_queue, console_rx_queue;

static ULONG tx_queue_data[TX_QUEUE_LEN];
static ULONG rx_queue_data[RX_QUEUE_LEN];

#define CONSOLE_POOL_SIZE 16384

static UCHAR console_pool_data[CONSOLE_POOL_SIZE];
static TX_BYTE_POOL console_pool;

static TX_THREAD console_tx_thread;
static TX_THREAD console_rx_thread;
static TX_THREAD console_vcp_rx_thread;

VOID console_tx_thread_entry(ULONG _a);
VOID console_rx_thread_entry(ULONG _a);
VOID console_vcp_rx_thread_entry(ULONG _a);

void console_init(void) {
  UINT result;
  UCHAR *pStack;

  result = tx_queue_create(&console_tx_queue,
      "console_tx_queue",
      1,
      tx_queue_data,
      TX_QUEUE_LEN);

  result = tx_queue_create(&console_rx_queue,
      "console_rx_queue",
      1,
      rx_queue_data,
      RX_QUEUE_LEN);

  tx_byte_pool_create(&console_pool, "console memory pool", console_pool_data, CONSOLE_POOL_SIZE);

  tx_byte_allocate(&console_pool, (VOID **)&pStack, 1024, TX_NO_WAIT);

  tx_thread_create(&console_tx_thread, "console_tx_thread_entry", console_tx_thread_entry, 1, pStack, 1024, 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

  tx_byte_allocate(&console_pool, (VOID **)&pStack, 1024, TX_NO_WAIT);

  tx_thread_create(&console_vcp_rx_thread, "console_vcp_rx_thread_entry", console_vcp_rx_thread_entry, 1, pStack, 1024, 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

  tx_byte_allocate(&console_pool, (VOID **)&pStack, 1024, TX_NO_WAIT);

  tx_thread_create(&console_rx_thread, "console_rx_thread_entry", console_rx_thread_entry, 1, pStack, 1024, 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);
}

static void wait_usb(void) {
  ULONG flags;

  tx_event_flags_get(&app_events, 1, TX_AND, &flags, TX_WAIT_FOREVER);
}

VOID console_rx_thread_entry(ULONG _a) {
  ULONG last = 0;

  while(1) {
    ULONG c;

    tx_queue_receive(&console_rx_queue, &c, TX_WAIT_FOREVER);

    if ((last != '\r') && (c == '\n')) {
      ULONG cc = '\r';
      tx_queue_send(&console_tx_queue, &cc, TX_WAIT_FOREVER);
    }

    if ((last == '\r') && (c != '\n')) {
      ULONG cc = '\n';
      tx_queue_send(&console_tx_queue, &cc, TX_WAIT_FOREVER);
    }


    tx_queue_send(&console_tx_queue, &c, TX_WAIT_FOREVER);

    last = c;
  }
}

VOID console_vcp_rx_thread_entry(ULONG _a) {
  UCHAR c;
  ULONG e;
  ULONG len;
  UINT status;

  wait_usb();

  while(1) {
    status = ux_device_class_cdc_acm_read(cdc_acm, &c, 1, &len);

    if ((status == TX_SUCCESS) && (len == 1)) {
      e = c;

      tx_queue_send(&console_rx_queue, &e, TX_WAIT_FOREVER);
    }
  }
}

VOID console_tx_thread_entry(ULONG _a) {
  UINT result;
  ULONG n, c;
  ULONG tx_len;
  UCHAR buf[TX_BUF_LEN];
  UCHAR *pcur;

  wait_usb();

  while (1) {
    n = 0;

    pcur = buf;
    result = tx_queue_receive(&console_tx_queue, &c, TX_WAIT_FOREVER);
    *pcur++ = c;
    n++;

    while (n < TX_BUF_LEN) {
      result = tx_queue_receive(&console_tx_queue, &c, 1);

      if (result != TX_SUCCESS) {
        break;
      }

      *pcur++ = c;
      n++;
    }

    pcur = buf;

    while (n) {
      result = ux_device_class_cdc_acm_write(cdc_acm, pcur, n, &tx_len);

      if (result != TX_SUCCESS) {
        while(1);
      }

      pcur += tx_len;
      n -= tx_len;
    }


  }
}

int _write(int file, char *ptr, int len)
{
  int retval = len;
  int result;

  while (len) {
    ULONG c = *ptr;

    if (c == '\n') {
      c = '\r';
      result = tx_queue_send(&console_tx_queue, &c, TX_WAIT_FOREVER);
      c = '\n';
    }

    result = tx_queue_send(&console_tx_queue, &c, TX_WAIT_FOREVER);
    ptr++;
    len--;
  }

  return retval;
}

