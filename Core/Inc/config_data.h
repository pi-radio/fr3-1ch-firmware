#ifndef __CONFIG_DATA_H__
#define __CONFIG_DATA_H__

#include <stdint.h>

#define CUR_CONFIG_VERSION 0x0001

#define TAG_CONFIG_HEADER  0xDEAF

typedef struct tlv {
  short t;
  short l;
} tlv_t;

struct config_header {
  tlv_t tlv;
  int version;
};

struct config_serial {
  tlv_t tlv;
  char serno[16];
};

struct _uid {
  uint32_t dw[3];

  uint32_t wafer_x, wafer_y;
  char lot_id[8];
  uint32_t wafer_no;
};

extern struct _uid U_ID;


extern const char _board_config_data[96 * 1024];


extern void init_config_data(void);
extern uint16_t config_get_flash_code(void);
extern uint16_t config_get_package_code(void);

static inline uint32_t config_get_flash_size(void) {
  return config_get_flash_code() * 1024UL;
}


extern int is_fr3_1ch_board(void);

#endif /* __CONFIG_DATA_H__ */
