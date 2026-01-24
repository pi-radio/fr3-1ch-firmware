#include "ux_device_cdc_acm.h"

#include "console.h"
#include "lexer.h"
#include "terminal.h"
#include "app_threadx.h"
#include "main.h"
#include "parser.h"

#define TX_BUF_LEN 128

#define TX_QUEUE_LEN 1024
#define RX_QUEUE_LEN 1024

static TX_THREAD console_rx_thread;

VOID console_tx_thread_entry(ULONG _a);
VOID console_rx_thread_entry(ULONG _a);
VOID console_vcp_rx_thread_entry(ULONG _a);

char console_stack[1024];

void console_init(void) {
  tx_thread_create(&console_rx_thread, "console_rx_thread_entry", console_rx_thread_entry, 1, console_stack, sizeof(console_stack), 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

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
  while(1) {
    int result = parser_parse_statement();

    switch(result) {
    case PARSER_OK:
      printf("OK\n");
      break;
    case PARSER_SYNTAX_ERROR:
      printf("Syntax Error\n");
      break;
    }
  }
}




