#ifndef __LMX_H__
#define __LMX_H__

#include <stdint.h>

extern void lmx_init(void);
extern void lmx_program(void);

extern int lmx_read_reg(int reg, uint16_t *val);

#endif /* __LMX_H__ */
