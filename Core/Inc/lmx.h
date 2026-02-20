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

  constexpr reg_reserved() : mask(0), value(0) {}
  constexpr reg_reserved(uint16_t m, uint16_t v) : mask(m), value(v) {}

  constexpr reg_reserved operator|(const reg_reserved &o) {
    return reg_reserved(mask | o.mask, value | o.value);
  }

  reg_reserved &operator|=(const reg_reserved &o) {
    mask |= o.mask;
    value |= o.value;

    return *this;
  }
};

constexpr reg_reserved rsrvd(uint16_t v) {
  return reg_reserved(0xFFFF, v);
}

constexpr uint16_t _to_mask(int sbit, int ebit) {
  assert(ebit >= sbit);
  return (uint16_t)(((1 << (ebit-sbit+1)) - 1) << sbit);
}

constexpr uint16_t _to_val(int sbit, uint16_t val) {
  return (uint16_t)(val << sbit);
}


constexpr reg_reserved rsrvd(int bit, uint16_t v) {
  const uint16_t mask = _to_mask(bit, bit);
  const uint16_t val = _to_val(bit, v);
  assert((val & ~mask) == 0);
  return reg_reserved(mask, val);
}

constexpr reg_reserved rsrvd(int sbit, int ebit, uint16_t v) {
  const uint16_t mask = _to_mask(sbit, ebit);
  const uint16_t val = _to_val(sbit, v);
  assert((val & ~mask) == 0);
  return reg_reserved(mask, val);
}

struct reg {
  uint16_t rnum;
  uint16_t value;
  reg_reserved rsrvd;
  LMX2820 *lmx;

  reg(int _rn) : rnum(_rn), value(0), rsrvd() {}

  reg(const reg &r) : rnum(r.rnum), value(r.value), rsrvd(r.rsrvd) {
  }

  reg(int _rn, const reg_reserved &_rsrvd) : rnum(_rn), rsrvd(_rsrvd) {
    value = rsrvd.value;
  }

  reg(int _rn, const std::initializer_list<reg_reserved> &_rsrvd) : rnum(_rn), rsrvd() {
    for (auto r : _rsrvd) {
      rsrvd |= r;
    }

    value = rsrvd.value;
  }

  operator uint16_t() {
    return value;
  }

  reg &operator =(uint16_t v);
  reg &operator |=(uint16_t v);
  reg &operator &=(uint16_t v);
};

template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
struct field
{
  LMX2820 *_lmx;

  const static uint16_t mask = ((1 << (ebit - sbit + 1)) - 1);

  field(LMX2820 *_pll);
  field(LMX2820 *_pll, uint16_t val);

  operator uint16_t() const;
  field &operator =(uint16_t v);
  field &operator =(int v) { return (*this = (uint16_t)v); }
};

template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
struct rbfield
{
  LMX2820 *_lmx;
  const static uint16_t mask = ((1 << (ebit - sbit + 1)) - 1);

  rbfield(LMX2820 *_pll);

  operator uint16_t() const;
};


template <uint32_t nreg, uint32_t sbit>
struct bit
{
  LMX2820 *_lmx;

  bit(LMX2820 *_pll) : _lmx(_pll) {}

  operator bool() const;
  bit &operator =(bool b);
};

template <uint32_t nreg>
struct regname
{
  LMX2820 *_lmx;

  regname(LMX2820 *_pll) : _lmx(_pll) {}

  operator uint16_t() const;
  regname &operator =(uint16_t v);
};

template <uint32_t hreg, uint32_t lreg>
struct dreg
{
  regname<hreg> h;
  regname<lreg> l;

  dreg(LMX2820 *_pll) : h(_pll), l(_pll) {}

  operator uint32_t() const {
    return ((uint32_t)h << 16) | (uint16_t)l;
  }

  dreg &operator =(uint32_t v) {
    h = (uint16_t)(v >> 16);
    l = (uint16_t)(v & 0xFFFF);

    return *this;
  }
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
  template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
  friend class field;
  template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
  friend class rbfield;
  template <uint32_t nreg, uint32_t sbit>
  friend class bit;
  template <uint32_t nreg>
  friend class regname;

  //uint16_t regs[N_REGS];
  std::array<reg, N_REGS> regs;
  std::bitset<N_REGS> dirty;

  output_mux_t outAmux;
  output_mux_t outBmux;


  double fOSC;

  struct field_base;

  std::vector<field_base *> fields;



  // Register 0
  bit<0,13> instcal_skip_acal;
  field<0,9,10> fcal_hpfd_adj;
  field<0,7,8> fcal_lpfd_adj;
  bit<0,6> dblr_cal_en;
  bit<0,4> fcal_en;
  bit<0,1> reset;
  bit<0,0> powerdown;

  // Register 1
  bit<1,15> phase_sync_en;
  bit<1,5> ld_vtune_en;
  bit<1,1> instcal_dblr_en;
  bit<1,0> instcal_en;

  // Register 2
  field<2, 12, 14> cal_clk_div;
  field<2, 1, 11> instcal_dly;
  bit<2, 0> quick_recal_en;

  // Register 6
  field<6, 8, 15> acal_cmp_dly;

  // Register 10
  bit<10, 12> pfd_dly_manual;
  bit<10, 11> vco_daciset_force;
  bit<10, 7> vco_capctl_force;

  // Register 11
  bit<11, 4> osc_2x;

  // Register 12
  field<12, 10, 12> mult;

  // Register 13
  field<13, 5, 12> pll_r;

  // Register 14
  field<14, 0, 11> pll_r_pre;

  field<16, 1, 4> cpg;

  bit<17, 6> ld_type;

  regname<18> ld_dly;

  field<19, 3, 4> tempsense_en;

  field<20, 0, 8> vco_daciset;

  field<22, 13, 15> vco_sel;
  field<22, 0, 7> vco_capctl;

  bit<23, 0> vco_sel_force;

  field<32, 9, 11> chdivB;
  field<32, 6, 8> chdivA;

  bit<34, 11> loopback_en;
  bit<34, 4> extvco_div;
  bit<34, 0> extvco_en;

  bit<35, 12> mash_reset_n;
  field<35, 7, 8> mash_order;
  bit<35, 6> mash_seed_en;

  field <36, 0, 14> pll_n;

  field <37, 0, 14> pfd_delay;

  dreg <38, 39> pll_den;

  dreg <40, 41> mash_seed;

  dreg <42, 43> pll_num;

  dreg <44, 45> instcal_pll_num;

  field <56, 0, 5> extpfd_div;

  bit <57, 0> pfd_sel;

  dreg <62, 63> mash_rst_count;

  field <64, 8, 9> sysref_inp_fmt;
  field <64, 5, 7> sysref_div_pre;

  bit <64, 4> sysref_repeat_ns;
  bit <64, 3> sysref_pulse;
  bit <64, 2> sysref_en;
  bit <64, 1> sysref_repeat;

  field <65, 0, 10> sysref_div;

  field <66, 6, 11> jesd_dac2_ctrl;
  field <66, 0, 5>  jesd_dac1_ctrl;

  field <67, 12, 15> sysref_pulse_cnt;
  field <67, 6, 11> jesd_dac4_ctrl;
  field <67, 0, 5> jesd_dac3_ctrl;

  bit<68, 5> inpin_ignore;
  bit<68, 0> psync_inp_fmt;

  bit<69, 4> srout_pd;

  bit<70, 7> dblbuf_outmux_en;
  bit<70, 6> dblbuf_outbuf_en;
  bit<70, 5> dblbuf_chdiv_en;
  bit<70, 4> dblbuf_pll_en;

  rbfield<74, 14, 15> rb_lock_detect;
  rbfield<74, 5, 12> rb_vco_capctrl;
  rbfield<74, 2, 4> rb_vco_sel;

  rbfield<75, 0, 8> rb_vco_daciset;

  rbfield<76, 0, 10> rb_temp_sense;

  bit<77, 8> pinmute_pol;

  bit<78, 4> outa_pd;
  field<78, 0, 1> outa_mux;

  bit<79, 8> outb_pd;
  field<79, 4, 5> outb_mux;
  field<79, 1, 3> outa_pwr;

  field<80, 6, 8> outb_pwr;

  bool get_instcal_dblr_en() {
    return (get_OUTAMux() == DOUBLER || get_OUTBMux() == DOUBLER);
  }


  void program_reg(int reg);

  void update_fVCO(double _fVCO);

  void update_fcal() {
    if (get_fPD() <= 100e6) fcal_hpfd_adj = 0;
    else if (get_fPD() <= 150e6) fcal_hpfd_adj = 1;
    else if (get_fPD() <= 200e6) fcal_hpfd_adj = 2;
    else fcal_hpfd_adj = 3;

    if (get_fPD() >= 10e6) fcal_lpfd_adj = 0;
    else if (get_fPD() >= 5e6) fcal_lpfd_adj = 1;
    else if (get_fPD() >= 2.5e6) fcal_lpfd_adj = 2;
    else fcal_lpfd_adj = 3;
  }

  void dirty_reg(int rno) {
    dirty.set(rno);
  }

public:
  LMX2820(double OSC_IN);

  double get_fVCO();
  double get_fOUTA();
  double get_fPD();

  void set_fOUTA(double);

  output_mux_t get_OUTAMux() { return outAmux; };
  output_mux_t get_OUTBMux() { return outBmux; };

  double get_fA();
  double get_fB();
  int get_powerA();
  int get_powerB();

  double set_fOUTA();

  void reprogram();
  void program();

  void setup();

  void dump();

  void set_reg(int reg, uint16_t val);
  uint16_t get_reg(int reg);

  int read_reg(int reg, uint16_t *v);
  int write_reg(int reg, uint16_t v);

  int locked();
};

};

#endif

#endif /* __LMX_H__ */
