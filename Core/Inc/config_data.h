#ifndef __CONFIG_DATA_H__
#define __CONFIG_DATA_H__

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


extern const char _board_config_data[96 * 1024];

#endif /* __CONFIG_DATA_H__ */
