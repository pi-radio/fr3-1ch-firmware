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

struct field_base
{
  LMX2820 *_lmx;
  uint32_t nreg;
  uint32_t sbit;
  uint32_t ebit;
  uint32_t mask;

  field_base(LMX2820 *_pll, uint32_t _nreg, uint32_t _sbit, uint32_t _ebit);

  operator uint16_t() const;

  uint16_t set(uint16_t v);

  //field_base &operator =(const uint16_t &v);
  //field_base &operator =(const int &v) { return (*this = (const uint16_t)v); }
};

struct bit_base
{
  LMX2820 *_lmx;
  uint32_t nreg;
  uint32_t sbit;

  bit_base(LMX2820 *_pll, uint32_t _nreg, uint32_t _sbit);

  bool set(bool b);
  operator bool() const;
};

template <uint32_t _nreg, uint32_t _sbit, uint32_t _ebit>
struct field : public field_base
{
  field(LMX2820 *_pll) : field_base(_pll, _nreg, _sbit, _ebit) {};
};

template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
struct rbfield
{
  LMX2820 *_lmx;
  const static uint16_t mask = ((1 << (ebit - sbit + 1)) - 1);

  rbfield(LMX2820 *_pll);

  operator uint16_t() const;
};


template <uint32_t _nreg, uint32_t _sbit>
struct bit : public bit_base
{
  LMX2820 *_lmx;

  bit(LMX2820 *_pll) : bit_base(_pll, _nreg, _sbit) {}
};

template <uint32_t nreg>
struct regname
{
  LMX2820 *_lmx;

  explicit regname(LMX2820 *_pll) : _lmx(_pll) {}

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

  friend class field_base;
  friend class bit_base;


  template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
  friend class rbfield;
  template <uint32_t nreg>
  friend class regname;

  //uint16_t regs[N_REGS];
  std::array<reg, N_REGS> regs;
  std::bitset<N_REGS> dirty;

  output_mux_t outAmux;
  output_mux_t outBmux;


  double _fOSC;
  double _fVCO;
  
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

  void update_PLL(double);
  void update_fVCO(double _fVCO);

  void update_fcal() {
    if (get_fPD() <= 100e6) fcal_hpfd_adj.set(0);
    else if (get_fPD() <= 150e6) fcal_hpfd_adj.set(1);
    else if (get_fPD() <= 200e6) fcal_hpfd_adj.set(2);
    else fcal_hpfd_adj.set(3);

    if (get_fPD() >= 10e6) fcal_lpfd_adj.set(0);
    else if (get_fPD() >= 5e6) fcal_lpfd_adj.set(1);
    else if (get_fPD() >= 2.5e6) fcal_lpfd_adj.set(2);
    else fcal_lpfd_adj.set(3);
  }

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

  void dump(std::ostream &os);

  void tune(double f);
  void set_drive(uint8_t d);

  void set_reg(int reg, uint16_t val);
  uint16_t get_reg(int reg);

  int read_reg(int reg, uint16_t *v);
  void write_reg(int reg, uint16_t v);

  int power_up();
  int power_down();

  int set_powerdown(bool b);

  int locked();
};

};

#endif

#endif /* __LMX_H__ */
