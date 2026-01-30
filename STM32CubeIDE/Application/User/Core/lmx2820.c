/*
 * lmx2820.c
 *
 *  Created on: Jan 28, 2026
 *      Author: zapman
 */
#include <stdint.h>
#include <fr3_1ch_hw.h>
#include <lmx.h>

#define N_LMX_REGS 123

extern uint16_t lmx_default_regs[N_LMX_REGS];

uint16_t lmx_regs[N_LMX_REGS];

#define DECLARE_BITMAP(name, n)   char name[(n+7)/8]

DECLARE_BITMAP(lmx_reg_dirty, N_LMX_REGS);

static inline int bitmap_check(char *bitmap, int bit)
{
  return bitmap[bit / 8] & (1 << bit);
}

static inline void bitmap_set(char *bitmap, int bit)
{
  bitmap[bit / 8] |= (1 << bit);
}

static inline void bitmap_clear(char *bitmap, int bit)
{
  bitmap[bit / 8] &= ~(1 << bit);
}

static inline void lmx_program_reg(int reg) {
  uint32_t v = (reg << 16) | lmx_regs[reg];

  spi_transmit(SPI_DEVICE_LMX, 3, v);
  bitmap_clear(lmx_reg_dirty, reg);
}

int lmx_read_reg(int reg, uint16_t *val) {
  int retval;

  uint32_t v = (reg << 16) | lmx_regs[reg];

  retval = spi_transfer(SPI_DEVICE_LMX, 3, &v);

  *val = (v & 0xFFFF);

  return retval;
}

void lmx_program(void)
{
  int i;

  for (i = N_LMX_REGS - 1; i >= 0; i--) {
    if (bitmap_check(lmx_reg_dirty, i)) {
      lmx_program_reg(i);
    }
  }
}



int lmx_locked(void)
{
  uint16_t v;
  int retval;

  retval = lmx_read_reg(74, &v);

  if (retval < 0) {
    return retval;
  }

  v = (v >> 14) & 0x3;

  switch (v) {
  case 0:
  case 1:
    retval = 0;
    break;
  case 2:
    retval = 1;
    break;
  default:
    retval = -1;
    break;
  }

  return retval;
}

void lmx_init(void)
{
  memset(lmx_reg_dirty, 0xFF, sizeof(lmx_reg_dirty));

  memcpy(lmx_regs, lmx_default_regs, sizeof(lmx_regs));
}



uint16_t lmx_default_regs[N_LMX_REGS] = {
    0x6470,
    0x57a0,
    0x81f4,
    0x0041,
    0x4204,
    0x0032,
    0x0a43,
    0x0000,
    0xc802,
    0x0005,
    0x0000,
    0x0612,
    0x0408,
    0x0038,
    0x3001,
    0x2001,
    0x171c,
    0x15c0,
    0x0000,
    0x2120,
    0x272c,
    0x1c64,
    0xe2bf,
    0x1102,
    0x0e34,
    0x0624,
    0x0db0,
    0x8001,
    0x0639,
    0x318c,
    0xb18c,
    0x0401,
    0x1001,
    0x0000,
    0x0010,
    0x3100,
    0x0032,
    0x0500,
    0x0000,
    0x03e8,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0300,
    0x0300,
    0x4180,
    0x0000,
    0x0080,
    0x203f,
    0x0000,
    0x0000,
    0x0000,
    0x0002,
    0x0001,
    0x0001,
    0x0000,
    0x1388,
    0x01f4,
    0x03e8,
    0x0000,
    0xc350,
    0x0080,
    0x0000,
    0x003f,
    0x1000,
    0x0020,
    0x0011,
    0x000e,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0608,
    0x0001,
    0x011e,
    0x01c0,
    0x0000,
    0x0000,
    0x0f00,
    0x0040,
    0x0000,
    0x0040,
    0xff00,
    0x03ff,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x1000,
    0x0000,
    0x0000,
    0x17f8,
    0x0000,
    0x1c80,
    0x19b9,
    0x0533,
    0x03e8,
    0x0028,
    0x0014,
    0x0014,
    0x000a,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x001f,
    0x0000,
    0xffff,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000
};
