#include <app_threadx.h>
#include <termbuf.h>

extern TX_QUEUE terminal_rx_queue;
extern TX_QUEUE terminal_tx_queue;

extern int force_redraw;

extern void terminal_txchar(int c);
extern int terminal_rxchar();

extern void terminal_init();

extern struct window output_win;

void terminal_reset();
void terminal_cursor_move(int x, int y);
void terminal_cursor_save();
void terminal_cursor_restore();
void terminal_clear_line();
