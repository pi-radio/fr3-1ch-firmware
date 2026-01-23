#include <config_data.h>

struct _uid U_ID;

uint16_t flash_code = -1;
uint16_t package_code = -1;


extern char _system_ro_data[];
extern const char _board_config_data[96 * 1024];

void init_config_data(void) {
  int i;

  uint32_t *pu32 = (uint32_t *)&_system_ro_data;

  uint8_t *p8;

  for (i = 0; i < 3; i++) {
    U_ID.dw[i] = *pu32++;
  }

  p8 = (uint8_t *)&U_ID.dw;

  U_ID.wafer_x = (*p8++) * 10;
  U_ID.wafer_x += *p8++;

  U_ID.wafer_y = (*p8++) * 10;
  U_ID.wafer_y += *p8++;

  U_ID.wafer_no = *p8++;

  for (i = 0; i < 7; i++) {
    U_ID.lot_id[i] = *p8++;
  }

  U_ID.lot_id[7] = 0;

  flash_code = (*pu32) & 0xFFFF;
  package_code = (*pu32) >> 16;

  uint16_t *pbcd = (uint16_t *)&_board_config_data;

  uint16_t a = *pbcd;
  a++;
}

uint16_t config_get_flash_code(void) {
  return flash_code;
}

uint16_t config_get_package_code(void) {
  return package_code;
}

int is_fr3_1ch_board(void)
{
  return package_code == 0x10;
}
