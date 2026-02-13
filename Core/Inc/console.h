#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "tx_api.h"

#if defined(__cplusplus)
extern "C" void console_init(void);

extern void console_cmd_ready(const char *s, size_t l);

#else
extern void console_init(void);
#endif

#if 0
extern int txchar(int c);

#endif

#endif
