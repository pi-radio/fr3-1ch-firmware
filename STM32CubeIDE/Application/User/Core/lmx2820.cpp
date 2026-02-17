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

extern uint16_t lmx_default_regs[LMX2820::N_REGS];

extern "C" int dbgprint(const char *fmt, ...);



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

  reg &operator =(uint16_t v) {
    if ((v & rsrvd.mask) != rsrvd.value) {
      dbgprint("ERROR: incorrect register assignment\r\n");
    } else {
      value = v;
    }

    return *this;
  }
};

reg template_regs[] {
    { 0, { rsrvd(15, 1), rsrvd(11, 12, 0), rsrvd(5, 1), rsrvd(2, 3, 0) } },
    { 1, { rsrvd(6, 14, 0x15E), rsrvd(2, 4, 0) } },
    { 2, { rsrvd(15, 1) } },
    { 3, rsrvd(0x41) },
    { 4, rsrvd(0x4204) },
    { 5, rsrvd(0x3832) },
    { 6, rsrvd(0, 7, 0x43) },
    { 7, rsrvd(0) },
    { 8, rsrvd(0xC802) },
    { 9, rsrvd(5) },
    { 10, { rsrvd(13, 15, 0), rsrvd(8, 10, 0), rsrvd(0, 6, 0) } },
    { 11, { rsrvd(5, 15, 0x30), rsrvd(0, 3, 0x2) } },
    { 12, { rsrvd(13, 15, 0), rsrvd(0, 9, 0x8) } },
    { 13, { rsrvd(13, 15, 0), rsrvd(0, 4, 0x18) } },
    { 14, rsrvd(12, 15, 0x3) },
    { 15, { rsrvd(12, 15, 0x2), rsrvd(0, 8, 0x1) } },
    { 16, { rsrvd(5, 15, 0x138), rsrvd(0, 0) } },
    { 17, { rsrvd(0, 5, 0), rsrvd(7, 15, 0x28) } },
    { 18 },
    { 19, { rsrvd(5, 15, 0x109), rsrvd(0, 2, 0) } },
    { 20, rsrvd(9, 15, 0x13) },
    { 21, rsrvd(0x1C64) },
    { 22, rsrvd(8, 12, 0x2) },
    { 23, rsrvd(1, 15, 0x881) },
    { 24, rsrvd(0xE34) },
    { 25, rsrvd(0x624) },
    { 26, rsrvd(0xDB0) },
    { 27, rsrvd(0x8001) },
    { 28, rsrvd(0x639) },
    { 29, rsrvd(0x318C) },
    { 30, rsrvd(0xB18C) },
    { 31, rsrvd(0x0401) },
    { 32, { rsrvd(12, 15, 0x01), rsrvd(0, 5, 1), rsrvd(0, 15, 0) } },
    { 33 },
    { 34, { rsrvd(12, 15, 0), rsrvd(5, 10, 0), rsrvd(1, 3, 0) } },
    { 35, { rsrvd(13, 15, 0x1), rsrvd(9, 11, 0), rsrvd(0, 5, 0) } },
    { 36, rsrvd(15, 0) },
    { 37, { rsrvd(15, 0), rsrvd(0, 8, 0x100) } },
    { 38 },
    { 39 },
    { 40 },
    { 41 },
    { 42 },
    { 43 },
    { 44 },
    { 45 },
    { 46, rsrvd(0x300) },
    { 47, rsrvd(0x300) },
    { 48, rsrvd(0x4180) },
    { 49, rsrvd(0) },
    { 50, rsrvd(0X80) },
    { 51, rsrvd(0X203F) },
    { 52, rsrvd(0) },
    { 53, rsrvd(0) },
    { 54, rsrvd(0) },
    { 55, rsrvd(0x2) },
    { 56, rsrvd(1, 15, 0) },
    { 57, rsrvd(1, 15, 0) },
    { 58, rsrvd(0) },
    { 59, rsrvd(0x1388) },
    { 60, rsrvd(0x01F4) },
    { 61, rsrvd(0x03E8) },
    { 62 },
    { 63 },
    { 64, { rsrvd(10, 15, 0x10), rsrvd(0, 0) } },
    { 65, rsrvd(11, 15, 0) },
    { 66, rsrvd(12, 15, 0) },
    { 67 },
    { 68, { rsrvd(6, 15, 0), rsrvd(1, 4, 0) } },
    { 69, { rsrvd(5, 15, 0), rsrvd(0, 3, 0x1) } },
    { 70, rsrvd(8, 15, 0) },
    { 71, rsrvd(0) },
    { 72, rsrvd(0) },
    { 73, rsrvd(0) },
    { 74, { rsrvd(13, 0), rsrvd(0, 1, 0) } },
    { 75, rsrvd(9, 15, 0) },
    { 76, rsrvd(11, 15, 0) },
    { 77, { rsrvd(9, 15, 0x3), rsrvd(0, 7, 0x8) } },
    { 78, { rsrvd(5, 15, 0), rsrvd(2, 3, 0) } },
    { 79, { rsrvd(9, 15, 0), rsrvd(6, 7, 0), rsrvd(0, 0) } },
    { 80, { rsrvd(9, 15, 0), rsrvd(0, 5, 0) } },
    { 81, rsrvd(0) },
    { 82, rsrvd(0) },
    { 83, rsrvd(0xF00) },
    { 84, rsrvd(0x40) },
    { 85, rsrvd(0) },
    { 86, rsrvd(0x40) },
    { 87, rsrvd(0xFF00) },
    { 88, rsrvd(0x3FF) },
    { 89, rsrvd(0) },
    { 90, rsrvd(0) },
    { 91, rsrvd(0) },
    { 92, rsrvd(0) },
    { 92, rsrvd(0) },
    { 93, rsrvd(0x1000) },
    { 94, rsrvd(0) },
    { 95, rsrvd(0) },
    { 96, rsrvd(0x17F8) },
    { 97, rsrvd(0) },
    { 98, rsrvd(0x1C80) },
    { 99, rsrvd(0x19B9) },
    { 100, rsrvd(0x0533) },
    { 101, rsrvd(0x3E8) },
    { 102, rsrvd(0x28) },
    { 103, rsrvd(0x14) },
    { 104, rsrvd(0x14) },
    { 105, rsrvd(0xA) },
    { 106, rsrvd(0) },
    { 107, rsrvd(0) },
    { 108, rsrvd(0) },
    { 109, rsrvd(0) },
    { 110, rsrvd(0x1F) },
    { 111, rsrvd(0) },
    { 112, rsrvd(0xFFFF) },
    { 113, rsrvd(0) },
    { 114, rsrvd(0) },
    { 115, rsrvd(0) },
    { 116, rsrvd(0) },
    { 117, rsrvd(0) },
    { 118, rsrvd(0) },
    { 119, rsrvd(0) },
    { 120, rsrvd(0) },
    { 121, rsrvd(0) },
    { 122, rsrvd(0) },
};


LMX2820::LMX2820(double _fOSC) :
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

  auto rl = std::to_array(template_regs);

  for (auto bits : rl) {
  }

  memcpy(regs, lmx_default_regs, sizeof(regs));

  dirty.set_all();
}

void LMX2820::update_kVCO()
{
  double fVCO = get_fVCO();

  if (fVCO > 11300) {
    kVCO = 141.9561;
  } else if (fVCO > 10600) {
    kVCO = 121.2635+(141.9561-121.2635)*(fVCO-10600)/(11300-10600);
  } else if (fVCO > 9800) {
    kVCO = 139.2359+(172.7641-139.2359)*(fVCO-9800)/(10600-9800);
  } else if (fVCO > 9000) {
    kVCO = 132.9341+(165.0659-132.9341)*(fVCO-9000)/(9800-9000);
  } else if (fVCO > 8100) {
    kVCO = 127.5931+(164.8139-127.5931)*(fVCO-8100)/(9000-8100);
  } else if (fVCO > 7300) {
    kVCO = 118.4778+(151.8703-118.4778)*(fVCO-7300)/(8100-7300);
  } else if (fVCO > 6350) {
    kVCO = 90.4624+(127.4863-90.4624)*(fVCO-6350)/(7300-6350);
  } else if (fVCO > 5650) {
    kVCO = 79.798+(110.202-79.798)*(fVCO-5650)/(6350-5650);
  } else {
    kVCO = 79.798;
  }
}

double LMX2820::get_fPD()
{
  double fPD = fOSC;

  if (osc_2x) {
    fPD *= 2;
  }

  fPD /= pll_r_pre;

  fPD *= mult;

  return fPD;
}


void LMX2820::reprogram()
{
  dirty.set_all();

  program();
}

void LMX2820::program()
{
  int i;

  for (i = N_REGS - 1; i >= 0; i--) {
    if (dirty.get(i)) {
      lmx.program_reg(i);
    }
  }
}

void LMX2820::program_reg(int reg)
{
  uint32_t v = (reg << 16) | regs[reg];

#if 0
  spi_transmit(SPI_DEVICE_LMX, 3, v);
#else
  spi_transfer(SPI_DEVICE_LMX, 3, &v);
#endif

  dirty.clear(reg);
}

int LMX2820::read_reg(int reg, uint16_t *val) {
  int retval;

  uint32_t v = (reg << 16) | regs[reg];

  retval = spi_transfer(SPI_DEVICE_LMX, 3, &v);

  *val = (v & 0xFFFF);

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

extern "C" void lmx_program(void)
{
  lmx.program();
}



#ifdef OCTOLO
LMX2820 lmx(100e6);
#else
LMX2820 lmx(10e6);
#endif



#ifndef OCTOLO

// Default 10MHz registers
uint16_t lmx_default_regs[LMX2820::N_REGS] = {
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

uint16_t lmx_default_regs[LMX2820::N_REGS] = {
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

