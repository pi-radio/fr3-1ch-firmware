#include <terminal.hpp>
#include "ux_device_cdc_acm.h"

#include "console.h"
#include "lexer.h"
#include "app_threadx.h"
#include "main.h"
#include "parser.h"

#define CONSOLE_CMD_QUEUE_LEN 16

static ULONG cmd_queue_data[CONSOLE_CMD_QUEUE_LEN];

static TX_THREAD console_rx_thread;
static TX_QUEUE console_cmd_queue;

VOID console_cmd_thread_entry(ULONG _a);

char console_stack[4096];

extern "C" void console_init(void) {
  int result;

  result = tx_queue_create(&console_cmd_queue,
      (char *)"console_cmd_queue",
      1,
      cmd_queue_data,
      sizeof(cmd_queue_data));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  tx_thread_create(&console_rx_thread, (char *)"console_rx_thread_entry", console_cmd_thread_entry, 1, console_stack, sizeof(console_stack), 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);
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

void console_cmd_ready(const char *s, size_t l)
{
  int result;

  std::string *ps = new std::string(s, l);

  if (ps == NULL) {
    printf("Unable to process: Out of memory\n");
    return;
  }

  result = tx_queue_send(&console_cmd_queue, &ps, TX_WAIT_FOREVER);

  if (result != TX_SUCCESS) {
    printf("Console failure: %d\n", result);
  }
}


void console_cmd_thread_entry(ULONG _a) {
  while(1) {
    int result;
    ULONG c;

    result = tx_queue_receive(&console_cmd_queue, &c, TX_WAIT_FOREVER);

    std::string *ps = (std::string *)c;

    lexer_set_line(*ps);

    delete ps;

    result = parser_parse_statement();

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




