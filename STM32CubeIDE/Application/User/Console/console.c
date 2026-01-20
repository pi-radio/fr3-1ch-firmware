#include "ux_device_cdc_acm.h"

#include "console.h"
#include "lexer.h"
#include "terminal.h"
#include "app_threadx.h"
#include "main.h"

#define TX_BUF_LEN 128

#define TX_QUEUE_LEN 1024
#define RX_QUEUE_LEN 1024

TX_QUEUE console_rx_queue;

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

  result = tx_queue_create(&console_rx_queue,
      "console_rx_queue",
      1,
      rx_queue_data,
      RX_QUEUE_LEN * sizeof(ULONG));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  tx_byte_pool_create(&console_pool, "console memory pool", console_pool_data, CONSOLE_POOL_SIZE);

  tx_byte_allocate(&console_pool, (VOID **)&pStack, 1024, TX_NO_WAIT);

  tx_thread_create(&console_rx_thread, "console_rx_thread_entry", console_rx_thread_entry, 1, pStack, 1024, 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

  terminal_init(&console_pool);
}


void usb_connect(void) {
  tx_event_flags_set(&app_events, USB_AVAILABLE_FLAG, TX_OR);
}

void usb_disconnect(void) {
  tx_event_flags_set(&app_events, ~USB_AVAILABLE_FLAG, TX_AND);
}

void wait_usb(void) {
  ULONG flags;

  tx_event_flags_get(&app_events, USB_AVAILABLE_FLAG, TX_AND, &flags, TX_WAIT_FOREVER);
}



VOID console_rx_thread_entry(ULONG _a) {
  token_t cur_tok;

  int a = 0;

  while(1) {
    get_token(&cur_tok);

    a++;
  }
}




