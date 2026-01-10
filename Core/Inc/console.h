#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "tx_api.h"

extern TX_QUEUE console_tx_queue, console_rx_queue;

extern void console_init(void);

#endif
