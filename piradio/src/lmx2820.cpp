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
#include <piradio/hardware.hpp>
#include <piradio/lmx2820.hpp>

using namespace piradio::hardware;



namespace LMX {
#if 0
    // Register 0
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
#endif

  template <uint32_t nreg, uint32_t sbit, uint32_t ebit>
  struct field
  {
    LMX2820 *_lmx;
    static constexpr uint32_t mask = ((1 << (ebit - sbit + 1)) - 1);

    field(LMX2820 *_pll) : _lmx(_pll) {};

    operator uint16_t() const
    {
      return (_lmx->regs[nreg] >> sbit) & mask;
    }


    uint16_t set(uint16_t v)
    {
      _lmx->regs[nreg] &= (uint16_t)~(mask << sbit);
      _lmx->regs[nreg] |= (v & mask) << sbit;

      _lmx->dirty.set(nreg);

      return v;
    }
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

    bool set(bool b)
    {
      if (b) {
        _lmx->regs[nreg] |= 1 << sbit;
      } else {
        _lmx->regs[nreg] &= (uint16_t)~(1 << sbit);
      }

      _lmx->dirty.set(nreg);

      return b;
    }


    operator bool() const
    {
      return (_lmx->regs[nreg] & (1 << sbit)) ? true : false;
    }
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

  using instcal_skip_acal = bit<0,13>;
  using fcal_hpfd_adj = field<0,9,10>;
  using fcal_lpfd_adj = field<0,7,8>;
  using dblr_cal_en = bit<0,6>;
  using fcal_en = bit<0,4>;
  using reset = bit<0,1>;
  using powerdown = bit<0,0>;

  // Register 1
  using phase_sync_en = bit<1,15>;
  using ld_vtune_en = bit<1,5> ;
  using instcal_dblr_en = bit<1,1> ;
  using instcal_en = bit<1,0>;

  // Register 2
  using cal_clk_div = field<2, 12, 14>;
  using instcal_dly =field<2, 1, 11>;
  using quick_recal_en = bit<2, 0>;

  // Register 6
  using acal_cmp_dly = field<6, 8, 15>;

  // Register 10
  using pfd_dly_manual = bit<10, 12>;
  using vco_daciset_force = bit<10, 11>;
  using vco_capctl_force = bit<10, 7>;

  // Register 11
  using osc_2x = bit<11, 4>;

  // Register 12
  using mult = field<12, 10, 12>;

  // Register 13
  using pll_r = field<13, 5, 12>;

  // Register 14
  using pll_r_pre = field<14, 0, 11>;

  using cpg = field<16, 1, 4>;

  using ld_type = bit<17, 6>;

  using ld_dly = regname<18>;

  using tempsense_en = field<19, 3, 4>;

  using vco_daciset = field<20, 0, 8>;

  using vco_sel = field<22, 13, 15>;
  using vco_capctl = field<22, 0, 7>;

  using vco_sel_force = bit<23, 0>;

  using chdivB = field<32, 9, 11>;
  using chdivA = field<32, 6, 8>;

  using loopback_en = bit<34, 11>;
  using extvco_div = bit<34, 4>;
  using extvco_en = bit<34, 0>;

  using mash_reset_n = bit<35, 12>;
  using mash_order = field<35, 7, 8>;
  using mash_seed_en = bit<35, 6>;

  using pll_n = field<36, 0, 14>;

  using pfd_delay = field<37, 0, 14>;

  using pll_den = dreg<38, 39>;

  using mash_seed = dreg<40, 41>;

  using pll_num = dreg<42, 43>;

  using instcal_pll_num = dreg <44, 45>;

  using extpfd_div = field<56, 0, 5>;

  using pfd_sel = bit <57, 0>;

  using mash_rst_count = dreg <62, 63>;

  using sysref_inp_fmt = field <64, 8, 9>;
  using sysref_div_pre = field <64, 5, 7>;

  using sysref_repeat_ns = bit<64, 4>;
  using sysref_pulse = bit<64, 3>;
  using sysref_en = bit<64, 2> ;
  using sysref_repeat = bit <64, 1>;

  using sysref_div = field<65, 0, 10>;

  using jesd_dac2_ctrl = field<66, 6, 11>;
  using jesd_dac1_ctrl = field<66, 0, 5>;

  using sysref_pulse_cnt = field<67, 12, 15>;
  using jesd_dac4_ctrl = field <67, 6, 11>;
  using jesd_dac3_ctrl = field <67, 0, 5>;

  using inpin_ignore = bit<68, 5>;
  using psync_inp_fmt = bit<68, 0>;

  using srout_pd = bit<69, 4>;

  using dblbuf_outmux_en = bit<70, 7>;
  using dblbuf_outbuf_en = bit<70, 6>;
  using dblbuf_chdiv_en = bit<70, 5>;
  using dblbuf_pll_en = bit<70, 4>;

  using rb_lock_detect = rbfield<74, 14, 15>;
  using rb_vco_capctrl = rbfield<74, 5, 12>;
  using rb_vco_sel = rbfield<74, 2, 4>;

  using rb_vco_daciset = rbfield<75, 0, 8>;

  using rb_temp_sense = rbfield<76, 0, 10>;

  using pinmute_pol = bit<77, 8>;

  using outa_pd = bit<78, 4>;
  using outa_mux = field<78, 0, 1>;

  using outb_pd = bit<79, 8>;
  using outb_mux = field<79, 4, 5>;
  using outa_pwr = field<79, 1, 3>;

  using outb_pwr = field<80, 6, 8>;



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

  LMX2820::LMX2820(double fOSC)
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
    dblr_cal_en(this).set(1);
    fcal_en(this).set(1);
    reset(this) = 0;
    powerdown(this) = 0;

    instcal_skip_acal(this).set(1);
    phase_sync_en(this) = 0;
    ld_vtune_en(this).set(1);
    instcal_dblr_en(this) = 0;
    instcal_en(this) = 0;
    cal_clk_div(this).set(0);
    instcal_dly(this).set(0xfa);

    acal_cmp_dly(this).set(0xa);
    quick_recal_en(this) = 0;
    pfd_dly_manual(this) = 0;

    vco_daciset(this).set(0x12C);

    vco_daciset_force(this) = 0;
    vco_capctl_force(this) = 0;
    cpg(this).set(0xe);
    ld_type(this).set(1);
    ld_dly(this) = 0;

    tempsense_en(this).set(1);

    // First section

    dblbuf_outmux_en(this) = 0;
    dblbuf_outbuf_en(this) = 0;
    dblbuf_chdiv_en(this) = 0;
    dblbuf_pll_en(this) = 0;

    // third section

    sysref_en(this) = 0;
    srout_pd(this).set(1);
    sysref_inp_fmt(this).set(0);
    sysref_div_pre(this).set(0);
    //sysref_div.set(0);
    sysref_pulse(this) = 0;
    sysref_pulse_cnt(this).set(1);

    // fourth section

    sysref_repeat(this) = 0;
    sysref_repeat_ns(this) = 0;
    jesd_dac1_ctrl(this).set(0x3f);
    jesd_dac2_ctrl(this).set(0x0);
    jesd_dac3_ctrl(this).set(0x0);
    jesd_dac4_ctrl(this).set(0x0);
    inpin_ignore(this).set(1);
    psync_inp_fmt(this) = 0;
    pinmute_pol(this) = 0;

    // second section

    /* Setup PFD */
    osc_2x(this).set(1);
    pll_r_pre(this).set(1);
    pll_r(this).set(1);
    mult(this).set(1);
    pfd_delay(this).set(0x500);
    pfd_sel(this).set(1);
    extpfd_div(this).set(1);

    update_fcal();

    /* Setup internal VCO */
    loopback_en(this) = 0;
    extvco_div(this).set(1);
    extvco_en(this) = 0;

    mash_reset_n(this).set(1);
    mash_order(this).set(2);
    mash_seed_en(this) = 0;
    mash_rst_count(this) = 50000;
    mash_seed(this) = 0;

    instcal_pll_num(this) = 0;

    update_fVCO(10e9);

    chdivA(this).set(0);
    outa_mux(this).set(1);
    outa_pwr(this).set(7);
    outa_pd(this).set(0);

    chdivB(this).set(0);
    outb_mux(this).set(1);
    outb_pwr(this).set(7);
    outb_pd(this).set(1);
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

    pll_n(this).set(intp);

    dbg::dbgout << "Setting PLL N to: " << pll_n(this) << " frac: " << frac << std::endl;

    if (std::fabs(frac) < 1.0 / max_denom) {
      pll_num(this) = 0;
      pll_den(this) = 1000;
    } else {
      auto result = rational<uint32_t>::approximate(frac, max_denom);

      pll_num(this) = result.num;
      pll_den(this) = result.den;

      dbg::dbgout << "Setting fractional-N to " << (uint32_t)pll_num(this) << "/" << (uint32_t)pll_den(this) << std::endl;
    }

    _fVCO = fPD * (pll_n(this) + (double)pll_num(this) / pll_den(this));
    
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
    vco_sel(this).set(vco);

    dbg::dbgout << "kVCO: " << kVCO << std::endl;

    vco_capctl(this).set(191 * (1 - t));
    
    update_PLL(f);

    dirty.set(0);
  }


  double LMX2820::get_fPD()
  {
    double fPD = _fOSC;

    if (osc_2x(this)) {
      fPD *= 2;
    }

    fPD /= pll_r_pre(this);

    fPD *= mult(this);

    fPD /= pll_r(this);

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

  void LMX2820::set_drive(uint8_t v )
  {
    outa_pwr(this).set(v);

    program();
  }

  void LMX2820::set_powerdown(bool b)
  {
    powerdown(this).set(b);

    program();
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

  void LMX2820::update_fcal() {
    if (get_fPD() <= 100e6) fcal_hpfd_adj(this).set(0);
    else if (get_fPD() <= 150e6) fcal_hpfd_adj(this).set(1);
    else if (get_fPD() <= 200e6) fcal_hpfd_adj(this).set(2);
    else fcal_hpfd_adj(this).set(3);

    if (get_fPD() >= 10e6) fcal_lpfd_adj(this).set(0);
    else if (get_fPD() >= 5e6) fcal_lpfd_adj(this).set(1);
    else if (get_fPD() >= 2.5e6) fcal_lpfd_adj(this).set(2);
    else fcal_lpfd_adj(this).set(3);
  }


  void LMX2820::dump(std::ostream &os)
  {
    os << "LMX2820 dump" << std::endl;

    os << "Control: " << std::endl;
    os << " dblr_cal_en: " << dblr_cal_en(this) << " fcal_en: " << fcal_en(this) << " reset: " << reset(this) << " powerdown: " << powerdown(this) << std::endl;
    os << " instcal_skip_acal: " << instcal_skip_acal(this) << " phase_sync_en: " << phase_sync_en(this) << " ld_vtune_en:" << ld_vtune_en(this) << std::endl;
    os << " instcal_dblr_en: " << instcal_dblr_en(this) << " instcal_en: " << instcal_en(this)
        << " cal_clk_div: " << cal_clk_div(this) << " instcal_dly: " << instcal_dly(this) << std::endl;
    os << " acal_cmp_dly: " << acal_cmp_dly(this) << " quick_recal_en: " << quick_recal_en(this)
        << " pfd_dly_manual: " << pfd_dly_manual(this) << " vco_daciset_force: " << vco_daciset_force(this) << std::endl;
    os << " vco_capctl_force: " << vco_capctl_force(this) << " cpg: " << cpg(this) << " ld_type: " << ld_type(this) << " ld_dly: " << ld_dly(this) << std::endl;
    os << " tempsense_en: " << tempsense_en(this) << std::endl;
    os << " dblbuf_outmux_en: " << dblbuf_outmux_en(this) << " dblbuf_outbuf_en: " << dblbuf_outbuf_en(this)
        << " dblbuf_chdiv_en: " << dblbuf_chdiv_en(this) << " dblbuf_pll_en: " << dblbuf_pll_en(this) << std::endl;

    os << std::endl;
    os << "Input Path:" << std::endl;
    os << " Osc 2x: " << osc_2x(this) << std::endl;
    os << " fcal hpfd adj: " << fcal_hpfd_adj(this) << " lpfd adj: " << fcal_lpfd_adj(this) << std::endl;
    os << " Pre R Divider: " << (uint16_t)pll_r_pre(this) << " Post R divider: " << (uint16_t)pll_r(this) << std::endl;
    os << " PFD delay: " << pfd_delay(this) << " pfd sel: " << pfd_sel(this) << " ext PFD div: " << extpfd_div(this) << std::endl;

    os << std::endl;
    os << "fPD: " << get_fPD() / 1.0e6 << " MHz" << std::endl;

    os << std::endl;
    os << "PLL Path:" << std::endl;
    os << " loopback_en: " << loopback_en(this) << " extvcd_div: " << extvco_div(this) << " extvco_en: " << extvco_en(this) << std::endl;
    os << " pll_n: " << (uint16_t)pll_n(this) << std::endl;
    os << " pll_den: " << pll_den(this) << std::endl;
    os << " pll_num: " << pll_num(this) << std::endl;
    os << " vco_sel: " << vco_sel(this) << " vco_sel_force: " << vco_sel_force(this) << std::endl;
    os << " vco_capctl: " << vco_capctl(this) << std::endl;
    os << " vco_daciset: " << vco_daciset(this) << std::endl;

    os << " mash_reset_n: " << mash_reset_n(this) << " mash_order: " << mash_order(this)
        << " mash_seed_en: " << mash_seed_en(this) << " mash_rst_count: " << mash_rst_count(this) << " mash_seed: " << mash_seed(this) << std::endl;

    os << " instcal_pll_num: " << instcal_pll_num(this) << std::endl;

    double frac = pll_n(this) + (double)pll_num(this)/pll_den(this);

    os << std::endl;
    os << "VCO freq: " << frac * get_fPD() / 1e9 << " GHz" << std::endl;

    os << std::endl;
    os << "Output: " << std::endl;
    os << " chdivA: " << chdivA(this) << " outa_mux: " << outa_mux(this) << " outa_pwr: "  << outa_pwr(this) << " outa_pd: " << outa_pd(this) << std::endl;
    os << " chdivB: " << chdivB(this) << " outb_mux: " << outb_mux(this) << " outb_pwr: "  << outb_pwr(this) << " outb_pd: " << outb_pd(this) << std::endl;

    os << std::endl;
    os << "JESD:" << std::endl;
    os << " sysref_en: " << sysref_en(this) << " srout_pd: " << srout_pd(this) << " sysref_inp_fmt: " << sysref_inp_fmt(this) << std::endl;
    os << " sysref_div_pre: " << sysref_div_pre(this) << " sysref_div: " << sysref_div(this)  << std::endl;
    os << " sysref_pulse: " << sysref_pulse(this) << " sysref_pulse_cnt: " << sysref_pulse_cnt(this) << std::endl;
    os << " sysref_repeat: " << sysref_repeat(this) << " sysref_repeat_ns: " << sysref_repeat_ns(this) <<  std::endl;

    os << " jesd_dac1_ctrl: " << jesd_dac1_ctrl(this) << " jesd_dac2_ctrl: " <<  jesd_dac2_ctrl(this)
        << " jesd_dac3_ctrl: " << jesd_dac3_ctrl(this) << " jesd_dac4_ctrl: " << jesd_dac4_ctrl(this) << std::endl;

    os << " inpin_ignore: " << inpin_ignore(this) << " psync_inp_fmt: " << psync_inp_fmt(this) << " pinmute_pol: " << pinmute_pol(this) << std::endl;

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
    //SPIRequest::transfer(SPITarget::LMX, 3, v);

    dirty.reset(reg);
  }

  int LMX2820::read_reg(int reg, uint16_t *val) {
    int retval;

    uint32_t v = (reg << 16) | regs[reg];

    retval = spi_transfer(SPI_DEVICE_LMX, 3, &v);
    //retval = SPIRequest::transfer(SPITarget::LMX, 3, v);

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

  double LMX2820::get_fOUTA()
  {
    if (outa_mux(this) == 2) {
      return _fVCO * 2;
    } else if (outa_mux(this) == 1) {
      return _fVCO;
    } else {
      return -1;
    }
  }


  bool LMX2820::get_instcal_dblr_en() {
    return (outa_mux(this) == 0x02 || outb_mux(this) == 0x02);
  }

  void LMX2820::tune(double f)
  {
    if (f > f_VCO_max) {
      outa_mux(this).set(2);
      f /= 2;
    } else {
      outa_mux(this).set(1);
    }

    update_fVCO(f);

    program();
  }
};
