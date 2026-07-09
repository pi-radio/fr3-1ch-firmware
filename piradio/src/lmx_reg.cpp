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
  reg &reg::operator =(uint16_t v) {
    if ((v & rsrvd.mask) != rsrvd.value) {
      dbg::dbgout << std::format("WARNING: Incorrect register assignment: register: {} val: {:04x} mask: {:04x} value: {:04x} masked assignment: {:04x}\n", rnum, v, rsrvd.mask, rsrvd.value, v & rsrvd.mask);
    } else {
      value = v;
    }

    if (lmx) lmx->dirty_reg(rnum);

    return *this;
  }

  reg &reg::operator |=(uint16_t v) {

    value |= v;

    if ((value & rsrvd.mask) != rsrvd.value) {
      dbg::dbgout << std::format("WARNING: Incorrect register assignment: register: {} val: {:04x} mask: {:04x} value: {:04x} masked assignment: {:04x}\n", rnum, v, rsrvd.mask, rsrvd.value, v & rsrvd.mask);
    }

    if (lmx) lmx->dirty_reg(rnum);

    return *this;
  }

  reg &reg::operator &=(uint16_t v) {
    value &= v;

    if ((value & rsrvd.mask) != rsrvd.value) {
      dbg::dbgout << std::format("WARNING: Incorrect register assignment: register: {} val: {:04x} mask: {:04x} value: {:04x} masked assignment: {:04x}\n", rnum, v, rsrvd.mask, rsrvd.value, v & rsrvd.mask);
    }

    if (lmx) lmx->dirty_reg(rnum);

    return *this;
  }
};
