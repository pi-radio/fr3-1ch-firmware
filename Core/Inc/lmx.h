#ifndef __LMX_H__
#define __LMX_H__

#include <stdint.h>


#if defined(__cplusplus)
extern "C" void lmx_program(void);

#include <vector>


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

  typedef enum {
    DIVIDER = 0,
    DIRECT = 1,
    DOUBLER = 2
  } output_mux_t;

protected:
  uint16_t regs[N_REGS];
  bitmap<N_REGS> dirty;

  output_mux_t outAmux;
  output_mux_t outBmux;


  double fOSC;
  double kVCO;

  struct field_base;

  std::vector<field_base *> fields;

  struct field_base
  {
    LMX2820 *lmx;
    bool dirty;

    field_base(LMX2820 *_lmx) : lmx(_lmx), dirty(false) {
      lmx->fields.push_back(this);
    }
  };

  template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
  struct field
  {
    LMX2820 *lmx;
    const static uint16_t mask = ((1 << (ebit - sbit + 1)) - 1);

    field(LMX2820 *_lmx) : lmx(_lmx) {}
    field(LMX2820 *_lmx, uint16_t val) : lmx(_lmx) { *this = val; }

    operator uint16_t() const {
      return (lmx->regs[nreg] >> sbit) & mask;
    }

    field &operator =(uint16_t v) {
      lmx->regs[nreg] &= ~(mask << sbit);
      lmx->regs[nreg] |= (v & mask) << sbit;

      lmx->dirty.set(nreg);

      return *this;
    }
  };

  template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
  struct rbfield
  {
    LMX2820 *lmx;
    const static uint16_t mask = ((1 << (ebit - sbit + 1)) - 1);

    rbfield(LMX2820 *_lmx) : lmx(_lmx) {}

    operator uint16_t() const {
      return (lmx->regs[nreg] >> sbit) & mask;
    }
  };


  template <uint32_t nreg, uint32_t sbit>
  struct bit : field<nreg, sbit, sbit>
  {
    bit(LMX2820 *_lmx) : field<nreg, sbit, sbit>(_lmx) {}
  };

  template <uint32_t nreg>
  struct regname : field<nreg, 0, 15>
  {
    regname(LMX2820 *_lmx) : field<nreg, 0, 15>(_lmx) {}
  };

  template <uint32_t hreg, uint32_t lreg>
  struct dreg
  {
    regname<hreg> h;
    regname<lreg> l;

    dreg(LMX2820 *_lmx) : h(_lmx), l(_lmx) {}

    operator uint16_t() const {
      return ((uint32_t)h << 16) | (uint16_t)l;
    }

    dreg &operator =(uint32_t v) {
      h = v >> 16;
      l = v & 0xFFFF;

      return *this;
    }
  };


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

  dreg <42, 42> pll_num;

  dreg <44, 45> instcal_pll_num;

  field <56, 0, 5> extpfd_div;

  bit <56, 0> pfd_sel;

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

  void update_kVCO();

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

public:
  LMX2820(double OSC_IN);

  double get_fVCO();

  double get_fPD();

  output_mux_t get_OUTAMux() { return outAmux; };
  output_mux_t get_OUTBMux() { return outBmux; };

  double get_fA();
  double get_fB();
  int get_powerA();
  int get_powerB();

  double set_fOUTA();

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
