#ifndef __LMX_H__
#define __LMX_H__

#include <stdint.h>



#if defined(__cplusplus)
extern "C" void lmx_program(void);

template <int N>
class bitmap
{
  static const int NBYTES = (N+7)/8;
  uint8_t bits[NBYTES];

public:
  bitmap() { memset(bits, 0, sizeof(bits)); };

  void set_all(void) { memset(bits, 0xFF, sizeof(bits)); }
  void clear_all(void) { memset(bits, 0, sizeof(bits)); }

  void set(int bit) { bits[bit / 8] |= (1 << (bit & 7)); }
  void clear(int bit) { bits[bit / 8] &= ~(1 << (bit & 7)); }

  bool get(int bit) { return (bits[bit / 8] >> (bit & 7)) & 1; }
};

class LMX2820
{
public:
  static const int N_REGS = 123;

private:
  uint32_t regs[N_REGS];
  bitmap<N_REGS> dirty;

  void program_reg(int reg);

public:
  LMX2820();

  void reprogram();
  void program();

  void set_reg(int reg, uint16_t val);
  uint16_t get_reg(int reg);

  int read_reg(int reg, uint16_t *v);
  int write_reg(int reg, uint16_t v);

  int locked();
};

extern LMX2820 lmx;

#else
void lmx_program(void);
#endif

#endif /* __LMX_H__ */
