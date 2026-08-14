/*
 * lmx2820.c
 *
 *  Created on: Jan 28, 2026
 *      Author: zapman
 */
#include <stdint.h>
#include <fr3_1ch_hw.h>

#include <initializer_list>
#include <array>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <numeric>

#include <format>

#include <threadxx/dbgstream.hpp>

#include <piradio/rational.hpp>
#include <piradio/lmx2820.hpp>

namespace LMX {

  field_base::field_base(LMX2820 *_pll, uint32_t _nreg, uint32_t _sbit, uint32_t _ebit) :
   _lmx(_pll), nreg(_nreg), sbit(_sbit), ebit(_ebit), mask(((1 << (ebit - sbit + 1)) - 1))
   {
    assert(nreg < LMX2820::N_REGS);
    assert(sbit < ebit);
    assert(ebit < 16);
   }

  field_base::operator uint16_t() const
  {
    return (_lmx->regs[nreg] >> sbit) & mask;
  }

  uint16_t field_base::set(uint16_t v)
  {
    _lmx->regs[nreg] &= (uint16_t)~(mask << sbit);
    _lmx->regs[nreg] |= (v & mask) << sbit;

    _lmx->dirty.set(nreg);

    return v;
  }

  bit_base::bit_base(LMX2820 *_pll, const uint32_t _nreg, const uint32_t _sbit) :
       _lmx(_pll), nreg(_nreg), sbit(_sbit)
  {
    assert(_nreg < LMX2820::N_REGS);
    assert(_sbit < 16);
  }

  bool bit_base::set(bool b)
  {
    if (b) {
      _lmx->regs[nreg] |= 1 << sbit;
    } else {
      _lmx->regs[nreg] &= (uint16_t)~(1 << sbit);
    }

    _lmx->dirty.set(nreg);

    return b;
  }


  bit_base::operator bool() const
  {
    return (_lmx->regs[nreg] & (1 << sbit)) ? true : false;
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

  uint16_t reg_reserved_data[][2] = {
    { 0xd82c, 0x4020 },
    { 0x7fdc, 0x5780 },
    { 0x8000, 0x8000 },
    { 0xffff, 0x0041 },
    { 0xffff, 0x4204 },
    { 0xffff, 0x0032 },
    { 0x00ff, 0x0043 },
    { 0xffff, 0x0000 },
    { 0xffff, 0xc802 },
    { 0xffff, 0x0005 },
    { 0xe77f, 0x0000 },
    { 0xffef, 0x0602 },
    { 0xe3ff, 0x0008 },
    { 0xe01f, 0x0018 },
    { 0xf000, 0x3000 },
    { 0xf1ff, 0x2001 },
    { 0xffe1, 0x1700 },
    { 0xffbf, 0x1580 },
    { 0x0000, 0x0000 },
    { 0xffe7, 0x2120 },
    { 0xfe00, 0x2600 },
    { 0xffff, 0x1c64 },
    { 0x1f00, 0x0200 },
    { 0xfffe, 0x1102 },
    { 0xffff, 0x0e34 },
    { 0xffff, 0x0624 },
    { 0xffff, 0x0db0 },
    { 0xffff, 0x8001 },
    { 0xffff, 0x0639 },
    { 0xffff, 0x318c },
    { 0xffff, 0xb18c },
    { 0xffff, 0x0401 },
    { 0xffff, 0x1001 },
    { 0x0000, 0x0000 },
    { 0xf7ee, 0x0000 },
    { 0xee3f, 0x2000 },
    { 0x8000, 0x0000 },
    { 0x81ff, 0x0100 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0xffff, 0x0300 },
    { 0xffff, 0x0300 },
    { 0xffff, 0x4180 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0080 },
    { 0xffff, 0x203f },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0002 },
    { 0xfffe, 0x0000 },
    { 0xfffe, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x1388 },
    { 0xffff, 0x01f4 },
    { 0xffff, 0x03e8 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0xfc01, 0x4000 },
    { 0xf800, 0x0000 },
    { 0xf000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0xffde, 0x0000 },
    { 0xffef, 0x0001 },
    { 0xff0f, 0x000e },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0x2003, 0x0000 },
    { 0xfe00, 0x0000 },
    { 0xf800, 0x0000 },
    { 0xfeff, 0x0608 },
    { 0xffec, 0x0000 },
    { 0xfec1, 0x0000 },
    { 0xfe3f, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0f00 },
    { 0xffff, 0x0040 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0040 },
    { 0xffff, 0xff00 },
    { 0xffff, 0x03ff },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x1000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x17f8 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x1c80 },
    { 0xffff, 0x19b9 },
    { 0xffff, 0x0533 },
    { 0xffff, 0x03e8 },
    { 0xffff, 0x0028 },
    { 0xffff, 0x0014 },
    { 0xffff, 0x0014 },
    { 0xffff, 0x000a },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x001f },
    { 0xffff, 0x0000 },
    { 0xffff, 0xffff },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 },
    { 0xffff, 0x0000 }
  };

  LMX2820::LMX2820(double fOSC) :
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
    _fOSC = fOSC;

    for (int i = 0; i < N_REGS; i++) {
      regs[i].rnum = i;
      regs[i].lmx = this;
      regs[i].rsrvd.mask = reg_reserved_data[i][0];
      regs[i].value = regs[i].rsrvd.value =
        reg_reserved_data[i][1];
    }
  }

  void LMX2820::setup()
  {
    dblr_cal_en.set(1);
    fcal_en.set(1);
    reset = 0;
    powerdown = 0;

    instcal_skip_acal.set(1);
    phase_sync_en = 0;
    ld_vtune_en.set(1);
    instcal_dblr_en = 0;
    instcal_en = 0;
    cal_clk_div.set(0);
    instcal_dly.set(0xfa);

    acal_cmp_dly.set(0xa);
    quick_recal_en = 0;
    pfd_dly_manual = 0;

    vco_daciset.set(0x12C);

    vco_daciset_force = 0;
    vco_capctl_force = 0;
    cpg.set(0xe);
    ld_type.set(1);
    ld_dly = 0;

    tempsense_en.set(0);

    dblbuf_outmux_en = 0;
    dblbuf_outbuf_en = 0;
    dblbuf_chdiv_en = 0;
    dblbuf_pll_en = 0;

    sysref_en = 0;
    srout_pd.set(1);
    sysref_inp_fmt.set(0);
    sysref_div_pre.set(0);
    sysref_div.set(0);
    sysref_pulse = 0;
    sysref_pulse_cnt.set(1);
    sysref_repeat = 0;
    sysref_repeat_ns = 0;
    jesd_dac1_ctrl.set(0x3f);
    jesd_dac2_ctrl.set(0x0);
    jesd_dac3_ctrl.set(0x0);
    jesd_dac4_ctrl.set(0x0);
    inpin_ignore.set(1);
    psync_inp_fmt = 0;
    pinmute_pol = 0;

    /* Setup PFD */
    osc_2x.set(1);
    pll_r_pre.set(1);
    pll_r.set(1);
    mult.set(1);
    pfd_delay.set(0x500);
    pfd_sel.set(1);
    extpfd_div.set(1);

    update_fcal();

    /* Setup internal VCO */
    loopback_en = 0;
    extvco_div.set(1);
    extvco_en = 0;

    mash_reset_n.set(1);
    mash_order.set(2);
    mash_seed_en = 0;
    mash_rst_count = 50000;
    mash_seed = 0;

    instcal_pll_num = 0;

    update_fVCO(10e9);

    chdivA.set(0);
    outa_mux.set(1);
    outa_pwr.set(7);
    outa_pd.set(0);

    chdivB.set(0);
    outb_mux.set(1);
    outb_pwr.set(7);
    outb_pd.set(1);
  }

  void LMX2820::set_OSCIN(double f)
  {
    _fOSC = f;

    update_PLL(_fVCO);
  }

  void LMX2820::update_PLL(double f)
  {
    double fPD = get_fPD();
    double max_denom = 65535;

    double m = f / fPD;
    double intp;

    double frac = std::modf(m, &intp);

    pll_n.set(intp);

    dbg::dbgout << "Setting PLL N to: " << pll_n << " frac: " << frac << std::endl;

    if (std::fabs(frac) < 1.0 / max_denom) {
      pll_num = 0;
      pll_den = 1000;
    } else {
      auto result = rational<uint32_t>::approximate(frac, max_denom);

      pll_num = result.num;
      pll_den = result.den;

      dbg::dbgout << "Setting fractional-N to " << (uint32_t)pll_num << "/" << (uint32_t)pll_den << std::endl;
    }

    _fVCO = fPD * (pll_n + (double)pll_num / pll_den);
    
    dbg::dbgout << "Computed fVCO: " << _fVCO << std::endl;
  }
  
  void LMX2820::update_fVCO(double f)
  {
    int vco = 0;
    double t;
    double kVCO;

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
    vco_sel.set(vco);

    dbg::dbgout << "kVCO: " << kVCO << std::endl;

    vco_capctl.set(191 * (1 - t));
    
    update_PLL(f);

    dirty.set(0);
  }


  double LMX2820::get_fPD()
  {
    double fPD = _fOSC;

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

  void LMX2820::write_reg(int reg, uint16_t val)
  {
    regs[reg] = val;

    program_reg(reg);
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


  void LMX2820::dump(std::ostream &os)
  {
    os << "LMX2820 dump" << std::endl;

    os << "Control: " << std::endl;
    os << " dblr_cal_en: " << dblr_cal_en << " fcal_en: " << fcal_en << " reset: " << reset << " powerdown: " << powerdown << std::endl;
    os << " instcal_skip_acal: " << instcal_skip_acal << " phase_sync_en: " << phase_sync_en << " ld_vtune_en:" << ld_vtune_en << std::endl;
    os << " instcal_dblr_en: " << instcal_dblr_en << " instcal_en: " << instcal_en << " cal_clk_div: " << cal_clk_div << " instcal_dly: " << instcal_dly << std::endl;
    os << " acal_cmp_dly: " << acal_cmp_dly << " quick_recal_en: " << quick_recal_en << " pfd_dly_manual: " << pfd_dly_manual << " vco_daciset_force: " << vco_daciset_force << std::endl;
    os << " vco_capctl_force: " << vco_capctl_force << " cpg: " << cpg << " ld_type: " << ld_type << " ld_dly: " << ld_dly << std::endl;
    os << " tempsense_en: " << tempsense_en << std::endl;
    os << " dblbuf_outmux_en: " << dblbuf_outmux_en << " dblbuf_outbuf_en: " << dblbuf_outbuf_en << " dblbuf_chdiv_en: " << dblbuf_chdiv_en << " dblbuf_pll_en: " << dblbuf_pll_en << std::endl;

    os << std::endl;
    os << "Input Path:" << std::endl;
    os << " Osc 2x: " << osc_2x << std::endl;
    os << " fcal hpfd adj: " << fcal_hpfd_adj << " lpfd adj: " << fcal_lpfd_adj << std::endl;
    os << " Pre R Divider: " << (uint16_t)pll_r_pre << " Post R divider: " << (uint16_t)pll_r << std::endl;
    os << " PFD delay: " << pfd_delay << " pfd sel: " << pfd_sel << " ext PFD div: " << extpfd_div << std::endl;

    os << std::endl;
    os << "fPD: " << get_fPD() / 1.0e6 << " MHz" << std::endl;

    os << std::endl;
    os << "PLL Path:" << std::endl;
    os << " loopback_en: " << loopback_en << " extvcd_div: " << extvco_div << " extvco_en: " << extvco_en << std::endl;
    os << " pll_n: " << (uint16_t)pll_n << std::endl;
    os << " pll_den: " << pll_den << std::endl;
    os << " pll_num: " << pll_num << std::endl;
    os << " vco_sel: " << vco_sel << " vco_sel_force: " << vco_sel_force << std::endl;
    os << " vco_capctl: " << vco_capctl << std::endl;
    os << " vco_daciset: " << vco_daciset << std::endl;

    os << " mash_reset_n: " << mash_reset_n << " mash_order: " << mash_order << " mash_seed_en: " << mash_seed_en << " mash_rst_count: " << mash_rst_count << " mash_seed: " << mash_seed << std::endl;

    os << " instcal_pll_num: " << instcal_pll_num << std::endl;

    double frac = pll_n + (double)pll_num/pll_den;

    os << std::endl;
    os << "VCO freq: " << frac * get_fPD() / 1e9 << " GHz" << std::endl;

    os << std::endl;
    os << "Output: " << std::endl;
    os << " chdivA: " << chdivA << " outa_mux: " << outa_mux << " outa_pwr: "  << outa_pwr << " outa_pd: " << outa_pd << std::endl;
    os << " chdivB: " << chdivB << " outb_mux: " << outb_mux << " outb_pwr: "  << outb_pwr << " outb_pd: " << outb_pd << std::endl;

    os << std::endl;
    os << "JESD:" << std::endl;
    os << " sysref_en: " << sysref_en << " srout_pd: " << srout_pd << " sysref_inp_fmt: " << sysref_inp_fmt << std::endl;
    os << " sysref_div_pre: " << sysref_div_pre << " sysref_div: " << sysref_div  << std::endl;
    os << " sysref_pulse: " << sysref_pulse << " sysref_pulse_cnt: " << sysref_pulse_cnt << std::endl;
    os << " sysref_repeat: " << sysref_repeat << " sysref_repeat_ns: " << sysref_repeat_ns <<  std::endl;

    os << " jesd_dac1_ctrl: " << jesd_dac1_ctrl << " jesd_dac2_ctrl: " <<  jesd_dac2_ctrl << " jesd_dac3_ctrl: " << jesd_dac3_ctrl << " jesd_dac4_ctrl: " << jesd_dac4_ctrl << std::endl;

    os << " inpin_ignore: " << inpin_ignore << " psync_inp_fmt: " << psync_inp_fmt << " pinmute_pol: " << pinmute_pol << std::endl;

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

    spi_transfer(SPI_DEVICE_LMX, 3, &v);

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

  void LMX2820::tune(double f)
  {
    if (f > f_VCO_max) {
      chdivA.set(1);
      f /= 2;
    }

    chdivA.set(0);
    update_fVCO(f);

    program();
  }
};
