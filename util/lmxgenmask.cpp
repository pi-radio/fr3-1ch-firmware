#include <cstdint>
#include <cassert>
#include <array>
#include <format>
#include <iostream>

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
  reg_reserved rsrvd;

  reg(int _rn) : rnum(_rn) {}
  reg(int _rn, reg_reserved _rsrvd) : rnum(_rn), rsrvd(_rsrvd) {}
  
  reg(int _rn, const std::initializer_list<reg_reserved> &_rsrvd) : rnum(_rn), rsrvd() {
    for (auto r : _rsrvd) {
      rsrvd |= r;
    }
  }
};


int main()
{
std::array<reg, 123> template_regs {
    reg(0, { rsrvd(14, 15, 1), rsrvd(11, 12, 0), rsrvd(5, 1), rsrvd(2, 3, 0) }),
    reg(1, { rsrvd(6, 14, 0x15E), rsrvd(2, 4, 0) }),
    reg(2, { rsrvd(15, 1) }),
    reg(3, rsrvd(0x41)),
    reg(4, rsrvd(0x4204)),
    reg(5, rsrvd(0x0032)),
    reg(6, rsrvd(0, 7, 0x43)),
    reg(7, rsrvd(0)),
    reg(8, rsrvd(0xC802)),
    reg(9, rsrvd(5)),

    reg(10, { rsrvd(13, 15, 0), rsrvd(8, 10, 0), rsrvd(0, 6, 0) }),
    reg(11, { rsrvd(5, 15, 0x30), rsrvd(0, 3, 0x2) }),
    reg(12, { rsrvd(13, 15, 0), rsrvd(0, 9, 0x8) }),
    reg(13, { rsrvd(13, 15, 0), rsrvd(0, 4, 0x18) }),
    reg(14, rsrvd(12, 15, 0x3)),
    reg(15, { rsrvd(12, 15, 0x2), rsrvd(0, 8, 0x1) }),
    reg(16, { rsrvd(5, 15, 0xB8), rsrvd(0, 0) }),
    reg(17, { rsrvd(0, 5, 0), rsrvd(7, 15, 0x2B) }),
    reg(18),
    reg(19, { rsrvd(5, 15, 0x109), rsrvd(0, 2, 0) }),
    reg(20, rsrvd(9, 15, 0x13)),

    reg(21, rsrvd(0x1C64)),
    reg(22, rsrvd(8, 12, 0x2)),
    reg(23, rsrvd(1, 15, 0x881)),
    reg(24, rsrvd(0xE34)),
    reg(25, rsrvd(0x624)),
    reg(26, rsrvd(0xDB0)),
    reg(27, rsrvd(0x8001)),
    reg(28, rsrvd(0x639)),
    reg(29, rsrvd(0x318C)),

    reg(30, rsrvd(0xB18C)),
    reg(31, rsrvd(0x0401)),
    reg(32, { rsrvd(12, 15, 0x01), rsrvd(0, 5, 1), rsrvd(0, 15, 0) }),
    reg(33),
    reg(34, { rsrvd(12, 15, 0), rsrvd(5, 10, 0), rsrvd(1, 3, 0) }),
    reg(35, { rsrvd(13, 15, 0x1), rsrvd(9, 11, 0), rsrvd(0, 5, 0) }),
    reg(36, rsrvd(15, 0)),
    reg(37, { rsrvd(15, 0), rsrvd(0, 8, 0x100) }),
    reg(38),
    reg(39),

    reg(40),
    reg(41),
    reg(42),
    reg(43),
    reg(44),
    reg(45),
    reg(46, rsrvd(0x300)),
    reg(47, rsrvd(0x300)),
    reg(48, rsrvd(0x4180)),
    reg(49, rsrvd(0)),

    reg(50, rsrvd(0X80)),
    reg(51, rsrvd(0X203F)),
    reg(52, rsrvd(0)),
    reg(53, rsrvd(0)),
    reg(54, rsrvd(0)),
    reg(55, rsrvd(0x2)),
    reg(56, rsrvd(1, 15, 0)),
    reg(57, rsrvd(1, 15, 0)),
    reg(58, rsrvd(0)),
    reg(59, rsrvd(0x1388)),

    reg(60, rsrvd(0x01F4)),
    reg(61, rsrvd(0x03E8)),
    reg(62),
    reg(63),
    reg(64, { rsrvd(10, 15, 0x10), rsrvd(0, 0) }),
    reg(65, rsrvd(11, 15, 0)),
    reg(66, rsrvd(12, 15, 0)),
    reg(67),
    reg(68, { rsrvd(6, 15, 0), rsrvd(1, 4, 0) }),
    reg(69, { rsrvd(5, 15, 0), rsrvd(0, 3, 0x1) }),

    reg(70, { rsrvd(8, 15, 0), rsrvd(0, 3, 0xE) }),
    reg(71, rsrvd(0)),
    reg(72, rsrvd(0)),
    reg(73, rsrvd(0)),
    reg(74, { rsrvd(13, 0), rsrvd(0, 1, 0) }),
    reg(75, rsrvd(9, 15, 0)),
    reg(76, rsrvd(11, 15, 0)),
    reg(77, { rsrvd(9, 15, 0x3), rsrvd(0, 7, 0x8) }),
    reg(78, { rsrvd(5, 15, 0), rsrvd(2, 3, 0) }),
    reg(79, { rsrvd(9, 15, 0), rsrvd(6, 7, 0), rsrvd(0, 0) }),

    reg(80, { rsrvd(9, 15, 0), rsrvd(0, 5, 0) }),
    reg(81, rsrvd(0)),
    reg(82, rsrvd(0)),
    reg(83, rsrvd(0xF00)),
    reg(84, rsrvd(0x40)),
    reg(85, rsrvd(0)),
    reg(86, rsrvd(0x40)),
    reg(87, rsrvd(0xFF00)),
    reg(88, rsrvd(0x3FF)),
    reg(89, rsrvd(0)),

    reg(90, rsrvd(0)),
    reg(91, rsrvd(0)),
    reg(92, rsrvd(0)),
    reg(93, rsrvd(0x1000)),
    reg(94, rsrvd(0)),
    reg(95, rsrvd(0)),
    reg(96, rsrvd(0x17F8)),
    reg(97, rsrvd(0)),
    reg(98, rsrvd(0x1C80)),
    reg(99, rsrvd(0x19B9)),
    reg(100, rsrvd(0x0533)),
    reg(101, rsrvd(0x3E8)),
    reg(102, rsrvd(0x28)),
    reg(103, rsrvd(0x14)),
    reg(104, rsrvd(0x14)),
    reg(105, rsrvd(0xA)),
    reg(106, rsrvd(0)),
    reg(107, rsrvd(0)),
    reg(108, rsrvd(0)),
    reg(109, rsrvd(0)),
    reg(110, rsrvd(0x1F)),
    reg(111, rsrvd(0)),
    reg(112, rsrvd(0xFFFF)),
    reg(113, rsrvd(0)),
    reg(114, rsrvd(0)),
    reg(115, rsrvd(0)),
    reg(116, rsrvd(0)),
    reg(117, rsrvd(0)),
    reg(118, rsrvd(0)),
    reg(119, rsrvd(0)),
    reg(120, rsrvd(0)),
    reg(121, rsrvd(0)),
    reg(122, rsrvd(0)),
};

 for (auto r : template_regs) {
   std::cout << std::format("{{ 0x{:04x}, 0x{:04x} }}", r.rsrvd.mask, r.rsrvd.value) << std::endl;
  }
}
