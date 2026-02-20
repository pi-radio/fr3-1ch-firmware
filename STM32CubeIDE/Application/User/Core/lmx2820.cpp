/*
 * lmx2820.c
 *
 *  Created on: Jan 28, 2026
 *      Author: zapman
 */
#include <stdint.h>
#include <fr3_1ch_hw.h>
#include <lmx.h>

#include <initializer_list>
#include <array>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <numeric>

#include <format>

#include <dbgstream.hpp>

extern std::array<uint16_t, LMX::LMX2820::N_REGS> lmx_default_regs;






namespace LMX {

std::array<reg, LMX2820::N_REGS> template_regs {
    reg({ 0, { rsrvd(14, 15, 1), rsrvd(11, 12, 0), rsrvd(5, 1), rsrvd(2, 3, 0) } }),
    reg({ 1, { rsrvd(6, 14, 0x15E), rsrvd(2, 4, 0) } }),
    reg({ 2, { rsrvd(15, 1) } }),
    reg({ 3, rsrvd(0x41) }),
    reg({ 4, rsrvd(0x4204) }),
    reg({ 5, rsrvd(0x0032) }),
    reg({ 6, rsrvd(0, 7, 0x43) }),
    reg({ 7, rsrvd(0) }),
    reg({ 8, rsrvd(0xC802) }),
    reg({ 9, rsrvd(5) }),

    reg({ 10, { rsrvd(13, 15, 0), rsrvd(8, 10, 0), rsrvd(0, 6, 0) } }),
    reg({ 11, { rsrvd(5, 15, 0x30), rsrvd(0, 3, 0x2) } }),
    reg({ 12, { rsrvd(13, 15, 0), rsrvd(0, 9, 0x8) } }),
    reg({ 13, { rsrvd(13, 15, 0), rsrvd(0, 4, 0x18) } }),
    reg({ 14, rsrvd(12, 15, 0x3) }),
    reg({ 15, { rsrvd(12, 15, 0x2), rsrvd(0, 8, 0x1) } }),
    reg({ 16, { rsrvd(5, 15, 0xB8), rsrvd(0, 0) } }),
    reg({ 17, { rsrvd(0, 5, 0), rsrvd(7, 15, 0x2B) } }),
    reg({ 18 }),
    reg({ 19, { rsrvd(5, 15, 0x109), rsrvd(0, 2, 0) } }),
    reg({ 20, rsrvd(9, 15, 0x13) }),

    reg({ 21, rsrvd(0x1C64) }),
    reg({ 22, rsrvd(8, 12, 0x2) }),
    reg({ 23, rsrvd(1, 15, 0x881) }),
    reg({ 24, rsrvd(0xE34) }),
    reg({ 25, rsrvd(0x624) }),
    reg({ 26, rsrvd(0xDB0) }),
    reg({ 27, rsrvd(0x8001) }),
    reg({ 28, rsrvd(0x639) }),
    reg({ 29, rsrvd(0x318C) }),

    reg({ 30, rsrvd(0xB18C) }),
    reg({ 31, rsrvd(0x0401) }),
    reg({ 32, { rsrvd(12, 15, 0x01), rsrvd(0, 5, 1), rsrvd(0, 15, 0) } }),
    reg({ 33 }),
    reg({ 34, { rsrvd(12, 15, 0), rsrvd(5, 10, 0), rsrvd(1, 3, 0) } }),
    reg({ 35, { rsrvd(13, 15, 0x1), rsrvd(9, 11, 0), rsrvd(0, 5, 0) } }),
    reg({ 36, rsrvd(15, 0) }),
    reg({ 37, { rsrvd(15, 0), rsrvd(0, 8, 0x100) } }),
    reg({ 38 }),
    reg({ 39 }),

    reg({ 40 }),
    reg({ 41 }),
    reg({ 42 }),
    reg({ 43 }),
    reg({ 44 }),
    reg({ 45 }),
    reg({ 46, rsrvd(0x300) }),
    reg({ 47, rsrvd(0x300) }),
    reg({ 48, rsrvd(0x4180) }),
    reg({ 49, rsrvd(0) }),

    reg({ 50, rsrvd(0X80) }),
    reg({ 51, rsrvd(0X203F) }),
    reg({ 52, rsrvd(0) }),
    reg({ 53, rsrvd(0) }),
    reg({ 54, rsrvd(0) }),
    reg({ 55, rsrvd(0x2) }),
    reg({ 56, rsrvd(1, 15, 0) }),
    reg({ 57, rsrvd(1, 15, 0) }),
    reg({ 58, rsrvd(0) }),
    reg({ 59, rsrvd(0x1388) }),

    reg({ 60, rsrvd(0x01F4) }),
    reg({ 61, rsrvd(0x03E8) }),
    reg({ 62 }),
    reg({ 63 }),
    reg({ 64, { rsrvd(10, 15, 0x10), rsrvd(0, 0) } }),
    reg({ 65, rsrvd(11, 15, 0) }),
    reg({ 66, rsrvd(12, 15, 0) }),
    reg({ 67 }),
    reg({ 68, { rsrvd(6, 15, 0), rsrvd(1, 4, 0) } }),
    reg({ 69, { rsrvd(5, 15, 0), rsrvd(0, 3, 0x1) } }),

    reg({ 70, { rsrvd(8, 15, 0), rsrvd(0, 3, 0xE) }}),
    reg({ 71, rsrvd(0) }),
    reg({ 72, rsrvd(0) }),
    reg({ 73, rsrvd(0) }),
    reg({ 74, { rsrvd(13, 0), rsrvd(0, 1, 0) } }),
    reg({ 75, rsrvd(9, 15, 0) }),
    reg({ 76, rsrvd(11, 15, 0) }),
    reg({ 77, { rsrvd(9, 15, 0x3), rsrvd(0, 7, 0x8) } }),
    reg({ 78, { rsrvd(5, 15, 0), rsrvd(2, 3, 0) } }),
    reg({ 79, { rsrvd(9, 15, 0), rsrvd(6, 7, 0), rsrvd(0, 0) } }),

    reg({ 80, { rsrvd(9, 15, 0), rsrvd(0, 5, 0) } }),
    reg({ 81, rsrvd(0) }),
    reg({ 82, rsrvd(0) }),
    reg({ 83, rsrvd(0xF00) }),
    reg({ 84, rsrvd(0x40) }),
    reg({ 85, rsrvd(0) }),
    reg({ 86, rsrvd(0x40) }),
    reg({ 87, rsrvd(0xFF00) }),
    reg({ 88, rsrvd(0x3FF) }),
    reg({ 89, rsrvd(0) }),

    reg({ 90, rsrvd(0) }),
    reg({ 91, rsrvd(0) }),
    reg({ 92, rsrvd(0) }),
    reg({ 93, rsrvd(0x1000) }),
    reg({ 94, rsrvd(0) }),
    reg({ 95, rsrvd(0) }),
    reg({ 96, rsrvd(0x17F8) }),
    reg({ 97, rsrvd(0) }),
    reg({ 98, rsrvd(0x1C80) }),
    reg({ 99, rsrvd(0x19B9) }),
    reg({ 100, rsrvd(0x0533) }),
    reg({ 101, rsrvd(0x3E8) }),
    reg({ 102, rsrvd(0x28) }),
    reg({ 103, rsrvd(0x14) }),
    reg({ 104, rsrvd(0x14) }),
    reg({ 105, rsrvd(0xA) }),
    reg({ 106, rsrvd(0) }),
    reg({ 107, rsrvd(0) }),
    reg({ 108, rsrvd(0) }),
    reg({ 109, rsrvd(0) }),
    reg({ 110, rsrvd(0x1F) }),
    reg({ 111, rsrvd(0) }),
    reg({ 112, rsrvd(0xFFFF) }),
    reg({ 113, rsrvd(0) }),
    reg({ 114, rsrvd(0) }),
    reg({ 115, rsrvd(0) }),
    reg({ 116, rsrvd(0) }),
    reg({ 117, rsrvd(0) }),
    reg({ 118, rsrvd(0) }),
    reg({ 119, rsrvd(0) }),
    reg({ 120, rsrvd(0) }),
    reg({ 121, rsrvd(0) }),
    reg({ 122, rsrvd(0) }),
};


reg &reg::operator =(uint16_t v) {
  if ((v & rsrvd.mask) != rsrvd.value) {
    dbgout << std::format("WARNING: Incorrect register assignment: register: {} val: {:04x} mask: {:04x} value: {:04x} masked assignment: {:04x}\n", rnum, v, rsrvd.mask, rsrvd.value, v & rsrvd.mask);
  } else {
    value = v;
  }

  if (lmx) lmx->dirty_reg(rnum);

  return *this;
}

reg &reg::operator |=(uint16_t v) {

  value |= v;

  if ((value & rsrvd.mask) != rsrvd.value) {
    dbgout << std::format("WARNING: Incorrect register assignment: register: {} val: {:04x} mask: {:04x} value: {:04x} masked assignment: {:04x}\n", rnum, v, rsrvd.mask, rsrvd.value, v & rsrvd.mask);
  }

  if (lmx) lmx->dirty_reg(rnum);

  return *this;
}

reg &reg::operator &=(uint16_t v) {
  value &= v;

  if ((value & rsrvd.mask) != rsrvd.value) {
    dbgout << std::format("WARNING: Incorrect register assignment: register: {} val: {:04x} mask: {:04x} value: {:04x} masked assignment: {:04x}\n", rnum, v, rsrvd.mask, rsrvd.value, v & rsrvd.mask);
  }

  if (lmx) lmx->dirty_reg(rnum);

  return *this;
}

template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
field<nreg, sbit, ebit>::field(LMX2820 *_pll) : _lmx(_pll)
{
}


template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
field<nreg, sbit, ebit>::field(LMX2820 *_pll, uint16_t val) : _lmx(_pll)
{
  *this = val;
}

template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
field<nreg, sbit, ebit>::operator uint16_t() const {
  return (_lmx->regs[nreg] >> sbit) & mask;
}

template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
field<nreg, sbit, ebit> &field<nreg, sbit, ebit>::operator =(uint16_t v) {
  _lmx->regs[nreg] &= (uint16_t)~(mask << sbit);
  _lmx->regs[nreg] |= (v & mask) << sbit;

  _lmx->dirty.set(nreg);

  return *this;
}

template <uint32_t nreg, uint32_t sbit>
bit<nreg, sbit>::operator bool() const
{
  return (_lmx->regs[nreg] & (1 << sbit)) ? true : false;
}


template <uint32_t nreg, uint32_t sbit>
bit<nreg, sbit> &bit<nreg, sbit>::operator =(bool b)
{
  if (b) {
    _lmx->regs[nreg] |= 1 << sbit;
  } else {
    _lmx->regs[nreg] &= (uint16_t)~(1 << sbit);
  }

  _lmx->dirty.set(nreg);

  return *this;
}

template <uint32_t nreg>
regname<nreg>::operator uint16_t() const
{
  return _lmx->regs[nreg];
}

template <uint32_t nreg>
regname<nreg> &regname<nreg>::operator =(uint16_t v)
{
  _lmx->regs[nreg] = v;
  return *this;
}



template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
rbfield<nreg, sbit, ebit>::rbfield(LMX2820 *_pll) : _lmx(_pll) {}

template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
rbfield<nreg, sbit, ebit>::operator uint16_t() const {
  return (_lmx->regs[nreg] >> sbit) & mask;
}

const std::array<drange, 7> LMX2820::fVCO {
   drange {f_VCO_min, 6.35e9},
   drange {6.35e9, 7.3e9},
   drange {7.3e9, 8.1e9},
   drange {8.1e9, 9.0e9},
   drange {9.0e9, 9.8e9},
   drange {9.8e9, 10.6e9},
   drange {10.6e9, f_VCO_max}
 };

const std::array<drange, 7> LMX2820::VCO_gain_range {
   drange {79.798, 110.202},
   drange {90.4624, 127.4863},
   drange {118.4778, 151.8703},
   drange {127.5931, 164.8139},
   drange {132.9341, 165.0659},
   drange {139.2359, 172.7641},
   drange {121.2635, 141.9561}
 };


LMX2820::LMX2820(double _fOSC) :
    regs(template_regs),
    outAmux(DIRECT),
    outBmux(DIRECT),
    instcal_skip_acal(this),
    fcal_hpfd_adj(this),
    fcal_lpfd_adj(this),
    dblr_cal_en(this),
    fcal_en(this),
    reset(this),
    powerdown(this),
    phase_sync_en(this),
    ld_vtune_en(this),
    instcal_dblr_en(this),
    instcal_en(this),
    cal_clk_div(this),
    instcal_dly(this),
    quick_recal_en(this),
    acal_cmp_dly(this),
    pfd_dly_manual(this),
    vco_daciset_force(this),
    vco_capctl_force(this),
    osc_2x(this),
    mult(this),
    pll_r(this),
    pll_r_pre(this),
    cpg(this),
    ld_type(this),
    ld_dly(this),
    tempsense_en(this),
    vco_daciset(this),
    vco_sel(this),
    vco_capctl(this),
    vco_sel_force(this),
    chdivB(this),
    chdivA(this),
    loopback_en(this),
    extvco_div(this),
    extvco_en(this),
    mash_reset_n(this),
    mash_order(this),
    mash_seed_en(this),
    pll_n(this),
    pfd_delay(this),
    pll_den(this),
    mash_seed(this),
    pll_num(this),
    instcal_pll_num(this),
    extpfd_div(this),
    pfd_sel(this),
    mash_rst_count(this),
    sysref_inp_fmt(this),
    sysref_div_pre(this),
    sysref_repeat_ns(this),
    sysref_pulse(this),
    sysref_en(this),
    sysref_repeat(this),
    sysref_div(this),
    jesd_dac2_ctrl(this),
    jesd_dac1_ctrl(this),
    sysref_pulse_cnt(this),
    jesd_dac4_ctrl(this),
    jesd_dac3_ctrl(this),
    inpin_ignore(this),
    psync_inp_fmt(this),
    srout_pd(this),
    dblbuf_outmux_en(this),
    dblbuf_outbuf_en(this),
    dblbuf_chdiv_en(this),
    dblbuf_pll_en(this),
    rb_lock_detect(this),
    rb_vco_capctrl(this),
    rb_vco_sel(this),
    rb_vco_daciset(this),
    rb_temp_sense(this),
    pinmute_pol(this),
    outa_pd(this),
    outa_mux(this),
    outb_pd(this),
    outb_mux(this),
    outa_pwr(this),
    outb_pwr(this)
{
  fOSC = _fOSC;

  for (int i = 0; i < N_REGS; i++) {
    regs[i].lmx = this;
    //regs[i] = lmx_default_regs[i];
  }

  /***

Input Path:
 Osc 2x: 1
 fcal hpfd adj: 0x0 lpfd adj: 0x0
 Pre R Divider: 1 Post R divider: 1
 PFD delay: 0x500 pfd sel: 1 ext PFD div: 0x1

fPD: 20 MHz

PLL Path:
 loopback_en: 0 extvcd_div: 1 extvco_en: 0
 pll_n: 500
 pll_den: 1000
 pll_num: 0
 vco_sel: 0x7 vco_sel_force: 0
 vco_capctl: 0xbf
 vco_daciset: 0x12c

VCO freq: 10 GHz

JESD:

   */

}

void LMX2820::setup()
{
  dblr_cal_en = 1;
  fcal_en = 1;
  reset = 0;
  powerdown = 0;

  instcal_skip_acal = 1;
  phase_sync_en = 0;
  ld_vtune_en = 1;
  instcal_dblr_en = 0;
  instcal_en = 0;
  cal_clk_div = 0;
  instcal_dly = 0xfa;

  acal_cmp_dly = 0xa;
  quick_recal_en = 0;
  pfd_dly_manual = 0;

  vco_daciset = 0x12C;

  vco_daciset_force = 0;
  vco_capctl_force = 0;
  cpg = 0xe;
  ld_type = 1;
  ld_dly = 0;

  tempsense_en = 0;

  dblbuf_outmux_en = 0;
  dblbuf_outbuf_en = 0;
  dblbuf_chdiv_en = 0;
  dblbuf_pll_en = 0;

  sysref_en = 0;
  srout_pd = 1;
  sysref_inp_fmt = 0;
  sysref_div_pre = 0;
  sysref_div = 0;
  sysref_pulse = 0;
  sysref_pulse_cnt = 1;
  sysref_repeat = 0;
  sysref_repeat_ns = 0;
  jesd_dac1_ctrl = 0x3f;
  jesd_dac2_ctrl = 0x0;
  jesd_dac3_ctrl = 0x0;
  jesd_dac4_ctrl = 0x0;
  inpin_ignore = 1;
  psync_inp_fmt = 0;
  pinmute_pol = 0;



  /* Setup PFD */

  osc_2x = 1;
  pll_r_pre = 1;
  pll_r = 1;
  mult = 1;
  pfd_delay = 0x500;
  pfd_sel = 1;
  extpfd_div = 1;

  update_fcal();

  /* Setup internal VCO */

  loopback_en = 0;
  extvco_div = 1;
  extvco_en = 0;

  mash_reset_n = 1;
  mash_order = 2;
  mash_seed_en = 0;
  mash_rst_count = 50000;
  mash_seed = 0;

  instcal_pll_num = 0;


  update_fVCO(10e9);
  //update_fVCO(10e9 * 2 / 3);

  chdivA = 0;
  outa_mux = 1;
  outa_pwr =7;
  outa_pd = 0;

  chdivB = 0;
  outb_mux = 1;
  outb_pwr =7;
  outb_pd = 1;

  for (int i = 0; i < N_REGS; i++) {
    if (regs[i] != lmx_default_regs[i]) {
      dbgout << std::format("Error: default regs difference: register {}: {:04x} default: {:04x}\n", i, (uint16_t)regs[i], (uint16_t)lmx_default_regs[i]);
    }
  }
}

template <typename T>
struct rational
{
  T num;
  T den;

  rational(T n = 0, T d = 0) : num(n), den(d) {}

  rational mediant(const rational &o)
  {
    return rational(num + o.num, den + o.den);
  }

  operator double() const { return (double)num/den; }
};


void LMX2820::update_fVCO(double f)
{
  int vco = 0;
  double t;
  double kVCO;
  double max_denom = 65536;

  for (auto i = 0; i < N_VCOS; i++) {
    if (fVCO[i].contains(f)) {
      vco = i + 1;
      t = fVCO[i].to_parametric(f);
      break;
    }
  }

  if (vco == 0) {
    if (f < fVCO[0]) {
      vco = 1;
      t = 0;
    } else {
      vco = N_VCOS;
      t = 1;
    }
  }

  kVCO = VCO_gain_range[vco - 1].from_parametric(t);
  vco_sel = (uint16_t)vco;

  dbgout << "kVCO: " << kVCO << std::endl;

  vco_capctl = (uint16_t)(191 * (1 - t));

  double fPD = get_fPD();

  double m = f / fPD;
  double intp;

  double frac = std::modf(m, &intp);

  pll_n = (int)intp;

  dbgout << "Setting PLL N to: " << pll_n << " frac: " << frac << std::endl;

  if (std::fabs(frac) < 1.0 / max_denom) {
    pll_num = 0;
    pll_den = 1000;
  } else {
    rational<uint32_t> low(0,1), high(1,1), result;

    while (true) {
      auto mediant = low.mediant(high);

      if (mediant.den > max_denom) {
        if (frac - low < high - frac) {
          result = low;
        } else {
          result = high;
        }

        break;
      }
      // We're already close enough, so we take the simplest fraction
      if (std::fabs(frac - mediant) < 1.0/max_denom) {
        result = mediant;
        break;
      }

      double n1 = frac * low.den - low.num;
      double n2 = high.num - frac * high.den;

      if (frac < (double)mediant) {
        int K = std::floor(n2 / n1);
        high.num = low.num + K * high.num;
        high.den = low.den + K * high.den;
      } else {
        int K = std::floor(n1 / n2);
        low.num = low.num + K * high.num;
        low.den = low.den + K * high.den;
      }
    }

    pll_num = result.num;
    pll_den = result.den;

    dbgout << "Setting fractional-N to " << (uint32_t)pll_num << "/" << (uint32_t)pll_den << std::endl;
  }

  dbgout << "Computed fVCO: " << (fPD * (pll_n + (double)pll_num / pll_den)) << std::endl;
}


double LMX2820::get_fPD()
{
  double fPD = fOSC;

  if (osc_2x) {
    fPD *= 2;
  }

  fPD /= pll_r_pre;

  fPD *= mult;

  fPD /= pll_r;

  return fPD;
}


void LMX2820::reprogram()
{
  dirty.set();

  program();
}

void LMX2820::program()
{
  int i;

  for (i = N_REGS - 1; i >= 0; i--) {
    if (dirty[i]) {
      program_reg(i);
    }
  }
}

template <uint32_t A, uint32_t B>
std::ostream &operator<<(std::ostream &os, dreg<A, B> &f)
{
  os << (uint32_t)f;

  return os;
}


template <uint32_t r, uint32_t b, uint32_t b2>
std::ostream &operator<<(std::ostream &os, field<r, b, b2> &f)
{
  std::ios save(NULL);
  save.copyfmt(os);

  os << std::hex << "0x" << (uint16_t)f;

  os.copyfmt(save);

  return os;
}

template <uint32_t r, uint32_t b>
std::ostream &operator<<(std::ostream &os, bit<r, b> &f)
{
  os << (bool)f;

  return os;
}


void LMX2820::dump()
{
  dbgout << "LMX2820 dump" << std::endl;

  dbgout << "Control: " << std::endl;
  dbgout << " dblr_cal_en: " << dblr_cal_en << " fcal_en: " << fcal_en << " reset: " << reset << " powerdown: " << powerdown << std::endl;
  dbgout << " instcal_skip_acal: " << instcal_skip_acal << " phase_sync_en: " << phase_sync_en << " ld_vtune_en:" << ld_vtune_en << std::endl;
  dbgout << " instcal_dblr_en: " << instcal_dblr_en << " instcal_en: " << instcal_en << " cal_clk_div: " << cal_clk_div << " instcal_dly: " << instcal_dly << std::endl;
  dbgout << " acal_cmp_dly: " << acal_cmp_dly << " quick_recal_en: " << quick_recal_en << " pfd_dly_manual: " << pfd_dly_manual << " vco_daciset_force: " << vco_daciset_force << std::endl;
  dbgout << " vco_capctl_force: " << vco_capctl_force << " cpg: " << cpg << " ld_type: " << ld_type << " ld_dly: " << ld_dly << std::endl;
  dbgout << " tempsense_en: " << tempsense_en << std::endl;
  dbgout << " dblbuf_outmux_en: " << dblbuf_outmux_en << " dblbuf_outbuf_en: " << dblbuf_outbuf_en << " dblbuf_chdiv_en: " << dblbuf_chdiv_en << " dblbuf_pll_en: " << dblbuf_pll_en << std::endl;

  dbgout << std::endl;
  dbgout << "Input Path:" << std::endl;
  dbgout << " Osc 2x: " << osc_2x << std::endl;
  dbgout << " fcal hpfd adj: " << fcal_hpfd_adj << " lpfd adj: " << fcal_lpfd_adj << std::endl;
  dbgout << " Pre R Divider: " << (uint16_t)pll_r_pre << " Post R divider: " << (uint16_t)pll_r << std::endl;
  dbgout << " PFD delay: " << pfd_delay << " pfd sel: " << pfd_sel << " ext PFD div: " << extpfd_div << std::endl;

  dbgout << std::endl;
  dbgout << "fPD: " << get_fPD() / 1.0e6 << " MHz" << std::endl;

  dbgout << std::endl;
  dbgout << "PLL Path:" << std::endl;
  dbgout << " loopback_en: " << loopback_en << " extvcd_div: " << extvco_div << " extvco_en: " << extvco_en << std::endl;
  dbgout << " pll_n: " << (uint16_t)pll_n << std::endl;
  dbgout << " pll_den: " << pll_den << std::endl;
  dbgout << " pll_num: " << pll_num << std::endl;
  dbgout << " vco_sel: " << vco_sel << " vco_sel_force: " << vco_sel_force << std::endl;
  dbgout << " vco_capctl: " << vco_capctl << std::endl;
  dbgout << " vco_daciset: " << vco_daciset << std::endl;

  dbgout << " mash_reset_n: " << mash_reset_n << " mash_order: " << mash_order << " mash_seed_en: " << mash_seed_en << " mash_rst_count: " << mash_rst_count << " mash_seed: " << mash_seed << std::endl;

  dbgout << " instcal_pll_num: " << instcal_pll_num << std::endl;

  double frac = pll_n + (double)pll_num/pll_den;

  dbgout << std::endl;
  dbgout << "VCO freq: " << frac * get_fPD() / 1e9 << " GHz" << std::endl;

  dbgout << std::endl;
  dbgout << "Output: " << std::endl;
  dbgout << " chdivA: " << chdivA << " outa_mux: " << outa_mux << " outa_pwr: "  << outa_pwr << " outa_pd: " << outa_pd << std::endl;
  dbgout << " chdivB: " << chdivB << " outb_mux: " << outb_mux << " outb_pwr: "  << outb_pwr << " outb_pd: " << outb_pd << std::endl;

  dbgout << std::endl;
  dbgout << "JESD:" << std::endl;
  dbgout << " sysref_en: " << sysref_en << " srout_pd: " << srout_pd << " sysref_inp_fmt: " << sysref_inp_fmt << std::endl;
  dbgout << " sysref_div_pre: " << sysref_div_pre << " sysref_div: " << sysref_div  << std::endl;
  dbgout << " sysref_pulse: " << sysref_pulse << " sysref_pulse_cnt: " << sysref_pulse_cnt << std::endl;
  dbgout << " sysref_repeat: " << sysref_repeat << " sysref_repeat_ns: " << sysref_repeat_ns <<  std::endl;

  dbgout << " jesd_dac1_ctrl: " << jesd_dac1_ctrl << " jesd_dac2_ctrl: " <<  jesd_dac2_ctrl << " jesd_dac3_ctrl: " << jesd_dac3_ctrl << " jesd_dac4_ctrl: " << jesd_dac4_ctrl << std::endl;

  dbgout << " inpin_ignore: " << inpin_ignore << " psync_inp_fmt: " << psync_inp_fmt << " pinmute_pol: " << pinmute_pol << std::endl;

#if 0

  rbfield<74, 14, 15> rb_lock_detect;
  rbfield<74, 5, 12> rb_vco_capctrl;
  rbfield<74, 2, 4> rb_vco_sel;

  rbfield<75, 0, 8> rb_vco_daciset;

  rbfield<76, 0, 10> rb_temp_sense;


#endif
}


void LMX2820::program_reg(int reg)
{
  uint32_t v = (reg << 16) | regs[reg];

#if 0
  spi_transmit(SPI_DEVICE_LMX, 3, v);
#else
  spi_transfer(SPI_DEVICE_LMX, 3, &v);
#endif

  dirty.reset(reg);
}

int LMX2820::read_reg(int reg, uint16_t *val) {
  int retval;

  uint32_t v = (reg << 16) | regs[reg];

  retval = spi_transfer(SPI_DEVICE_LMX, 3, &v);

  *val = (v & 0xFFFF);

  if (retval == 0 && !dirty[reg])
    regs[reg] = *val;

  return retval;
}

int LMX2820::locked()
{
  uint16_t v;
  int retval = read_reg(74, &v);

  if (retval < 0) {
    return false;
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

};



#ifndef OCTOLO

// Default 10MHz registers
std::array<uint16_t, LMX::LMX2820::N_REGS> lmx_default_regs{
    0x6070,
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
    0x01f4,
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
    0x0000,
};

#else

std::array<uint16_t, LMX::LMX2820::N_REGS> lmx_default_regs{
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



#endif

