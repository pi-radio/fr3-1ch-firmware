#ifndef __LMX_H__
#define __LMX_H__

#include <stdint.h>


#if defined(__cplusplus)
extern "C" void lmx_program(void);

#include <cassert>

#include <vector>
#include <array>
#include <bitset>

extern "C" int dbgprint(const char *fmt, ...);

namespace LMX
{

  class LMX2820;

  struct drange {
    double min;
    double max;

    drange(const double &_min, const double &_max) : min(_min), max(_max) { }

    template <typename T>
    bool contains(T v) const {
      return (v >= min && v < max);
    }

    double to_parametric(double v) const {
      return (v - min) / (max - min);
    }

    double from_parametric(double t) const {
      return (max - min) * t + min;
    }
  };

  template <typename T>
  bool operator>(const drange &r, T o)
  {
    return o >= r.max;
  }

  template <typename T>
  bool operator<(T o, const drange &r)
  {
    return o < r.min;
  }


  struct reserved_bits {
    const int reg;
    const int sbit;
    const int ebit;
    uint16_t val;

    reserved_bits(int r, uint16_t v) : reg(r), sbit(0), ebit(15), val(v) {
    }

    reserved_bits(int r, int s, uint16_t v) : reg(r), sbit(s), ebit(s), val(v) {
    }
    reserved_bits(int r, int s, int e, uint16_t v) : reg(r), sbit(s), ebit(e), val(v) {
    }
  };

  struct reg_reserved {
    uint16_t mask;
    uint16_t value;
  };

  struct reg {
    uint16_t rnum;
    uint16_t value;
    reg_reserved rsrvd;
    LMX2820 *lmx;

    operator uint16_t() {
      return value;
    }
  
    reg &operator =(uint16_t v);
    reg &operator |=(uint16_t v);
    reg &operator &=(uint16_t v);
  };


class LMX2820
{
public:
  static constexpr int N_REGS = 123;
  static constexpr int N_VCOS = 7;
  static constexpr double f_VCO_min = 5.65e9;
  static constexpr double f_VCO_max = 11.3e9;

  static const std::array<drange, N_VCOS> fVCO;
  static const std::array<drange, N_VCOS> VCO_gain_range;

  typedef enum {
    DIVIDER = 0,
    DIRECT = 1,
    DOUBLER = 2
  } output_mux_t;

protected:
  friend class reg;

  template <uint32_t _nreg, uint32_t _sbit, uint32_t _ebit>
  friend struct field;

  template <uint32_t _nreg, uint32_t _sbit>
  friend struct bit;

  template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
  friend class rbfield;
  template <uint32_t nreg>
  friend class regname;

  //uint16_t regs[N_REGS];
  std::array<reg, N_REGS> regs;
  std::bitset<N_REGS> dirty;

  double _fOSC;
  double _fVCO;
  
  bool get_instcal_dblr_en();

  void program_reg(int reg);

  void update_PLL(double);
  void update_fVCO(double _fVCO);

  void update_fcal();

  void dirty_reg(int rno) {
    dirty.set(rno);
  }

#ifndef OCTOLO
  constexpr static std::array<uint16_t, LMX2820::N_REGS> default_regs{
    0x6070, 0x57a0, 0x81f4, 0x0041, 0x4204, 0x0032, 0x0a43, 0x0000,
    0xc802, 0x0005, 0x0000, 0x0612, 0x0408, 0x0038, 0x3001, 0x2001,
    0x171c, 0x15c0, 0x0000, 0x2120, 0x272c, 0x1c64, 0xe2bf, 0x1102,
    0x0e34, 0x0624, 0x0db0, 0x8001, 0x0639, 0x318c, 0xb18c, 0x0401,
    // 32
    0x1001, 0x0000, 0x0010, 0x3100, 0x01f4, 0x0500, 0x0000, 0x03e8,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0300, 0x0300,
    0x4180, 0x0000, 0x0080, 0x203f, 0x0000, 0x0000, 0x0000, 0x0002,
    0x0001, 0x0001, 0x0000, 0x1388, 0x01f4, 0x03e8, 0x0000, 0xc350,
    // 64
    0x0080, 0x0000, 0x003f, 0x1000, 0x0020, 0x0011, 0x000e, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0608, 0x0001, 0x011e,
    0x01c0, 0x0000, 0x0000, 0x0f00, 0x0040, 0x0000, 0x0040, 0xff00,
    0x03ff, 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x0000,
    // 96
    0x17f8, 0x0000, 0x1c80, 0x19b9, 0x0533, 0x03e8, 0x0028, 0x0014,
    0x0014, 0x000a, 0x0000, 0x0000, 0x0000, 0x0000, 0x001f, 0x0000,
    0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000,
};

#else

  constexpr static std::array<uint16_t, LMX2820::N_REGS> default_regs{
     0x6470, 0x57a0, 0x81f4, 0x0041, 0x4204, 0x0032, 0x0a43, 0x0000,
     0xc802, 0x0005, 0x0000, 0x0612, 0x0408, 0x0038, 0x3001, 0x2001,
     0x171c, 0x15c0, 0x0000, 0x2120, 0x272c, 0x1c64, 0xe2bf, 0x1102,
     0x0e34, 0x0624, 0x0db0, 0x8001, 0x0639, 0x318c, 0xb18c, 0x0401,
     0x1001, 0x0000, 0x0010, 0x3100, 0x0032, 0x0500, 0x0000, 0x03e8,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0300, 0x0300,
     0x4180, 0x0000, 0x0080, 0x203f, 0x0000, 0x0000, 0x0000, 0x0002,
     0x0001, 0x0001, 0x0000, 0x1388, 0x01f4, 0x03e8, 0x0000, 0xc350,
     0x0080, 0x0000, 0x003f, 0x1000, 0x0020, 0x0011, 0x000e, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0608, 0x0001, 0x011e,
     0x01c0, 0x0000, 0x0000, 0x0f00, 0x0040, 0x0000, 0x0040, 0xff00,
     0x03ff, 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x0000,
     0x17f8, 0x0000, 0x1c80, 0x19b9, 0x0533, 0x03e8, 0x0028, 0x0014,
     0x0014, 0x000a, 0x0000, 0x0000, 0x0000, 0x0000, 0x001f, 0x0000,
     0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000
};
#endif
  
public:
  LMX2820(double OSC_IN);

  void set_OSCIN(double);

  double get_fVCO();
  double get_fOUTA();
  double get_fPD();

  void set_fOUTA(double);

  double get_fA();
  double get_fB();
  int get_powerA();
  int get_powerB();

  //double set_fOUTA();

  void reprogram();
  void program();

  void setup();

  void dump(std::ostream &os);

  void tune(double f);
  void set_drive(uint8_t d);

  void set_reg(int reg, uint16_t val);
  uint16_t get_reg(int reg);

  int read_reg(int reg, uint16_t *v);
  void write_reg(int reg, uint16_t v);

  int power_up();
  int power_down();

  void set_powerdown(bool b);

  int locked();
};

};

#endif

#endif /* __LMX_H__ */
