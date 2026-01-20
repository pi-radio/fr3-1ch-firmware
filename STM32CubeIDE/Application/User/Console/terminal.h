#include "app_threadx.h"

extern TX_QUEUE terminal_rx_queue;
extern TX_QUEUE terminal_tx_queue;

extern void terminal_txchar(int c);
extern int terminal_rxchar();

extern void terminal_init(TX_BYTE_POOL *pool);


void terminal_reset();
void terminal_cursor_move(int x, int y);
void terminal_cursor_save();
void terminal_cursor_restore();
void terminal_clear_line();
