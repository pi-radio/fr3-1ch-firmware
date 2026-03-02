#include <halxx/stm32h563.hpp>

extern "C" {
#include <stm32h5xx_hal.h>
}

static void *ro_base = (void *)0x08FFF800;

using namespace halxx;

STM32H563::STM32H563()
{
  int i;

  uint32_t *pu32 = (uint32_t *)ro_base;

  uint8_t *p8;

  for (i = 0; i < 3; i++) {
    id.dw[i] = *pu32++;
  }

  p8 = (uint8_t *)&id.dw;

  id.wafer_x = (*p8++) * 10;
  id.wafer_x += *p8++;

  id.wafer_y = (*p8++) * 10;
  id.wafer_y += *p8++;

  id.wafer_no = *p8++;

  for (i = 0; i < 7; i++) {
    id.lot_id[i] = *p8++;
  }

  id.lot_id[7] = 0;

  pu32 = (uint32_t *)p8;
  
  flash_code = (*pu32) & 0xFFFF;
  package_code = (*pu32) >> 16;
  
  HAL_Init();
}
