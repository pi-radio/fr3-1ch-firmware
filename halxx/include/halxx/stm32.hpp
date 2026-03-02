#pragma once

#include <cstdint>

namespace halxx
{
  struct uniqid {
    union {
      struct {
	uint32_t dw[3];
	
	uint32_t wafer_x, wafer_y;
	char lot_id[8];
	uint32_t wafer_no;
      } __attribute__((packed));
      uint8_t data[96];
    };
  };
};
