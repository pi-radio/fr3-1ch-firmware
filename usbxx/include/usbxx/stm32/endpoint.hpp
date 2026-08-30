#pragma once

#include <usbxx/endpoint.hpp>

#include <usbxx/ux_stm32_config.h>

struct UX_DCD_STM32_ED
{
    UX_SLAVE_ENDPOINT
                    *ux_dcd_stm32_ed_endpoint;
    ULONG           ux_dcd_stm32_ed_status;
    UCHAR           ux_dcd_stm32_ed_state;
    UCHAR           ux_dcd_stm32_ed_index;
    UCHAR           ux_dcd_stm32_ed_direction;
    UCHAR           reserved;
};

namespace USBXX
{
  namespace STM32
  {

  }
}
