#pragma once

#include <usbxx/dcd.hpp>

#include <usbxx/ux_stm32_config.h>

/* Define STM32 generic equivalences.  */

#define UX_DCD_STM32_SLAVE_CONTROLLER                           0x80
#ifndef UX_DCD_STM32_MAX_ED
#define UX_DCD_STM32_MAX_ED                                     4
#endif /* UX_DCD_STM32_MAX_ED */
#define UX_DCD_STM32_IN_FIFO                                    3


#define UX_DCD_STM32_FLUSH_RX_FIFO                              0x00000010
#define UX_DCD_STM32_FLUSH_TX_FIFO                              0x00000020
#define UX_DCD_STM32_FLUSH_FIFO_ALL                             0x00000010
#define UX_DCD_STM32_ENDPOINT_SPACE_SIZE                        0x00000020
#define UX_DCD_STM32_ENDPOINT_CHANNEL_SIZE                      0x00000020


/* Define USB STM32 physical endpoint status definition.  */

#define UX_DCD_STM32_ED_STATUS_UNUSED                            0u
#define UX_DCD_STM32_ED_STATUS_USED                              1u
#define UX_DCD_STM32_ED_STATUS_TRANSFER                          2u
#define UX_DCD_STM32_ED_STATUS_STALLED                           4u
#define UX_DCD_STM32_ED_STATUS_DONE                              8u
#define UX_DCD_STM32_ED_STATUS_SETUP_IN                          (1u<<8)
#define UX_DCD_STM32_ED_STATUS_SETUP_STATUS                      (2u<<8)
#define UX_DCD_STM32_ED_STATUS_SETUP_OUT                         (3u<<8)
#define UX_DCD_STM32_ED_STATUS_SETUP                             (3u<<8)
#define UX_DCD_STM32_ED_STATUS_TASK_PENDING                      (1u<<10)

/* Define USB STM32 physical endpoint state machine definition.  */

#define UX_DCD_STM32_ED_STATE_IDLE                               0
#define UX_DCD_STM32_ED_STATE_DATA_TX                            1
#define UX_DCD_STM32_ED_STATE_DATA_RX                            2
#define UX_DCD_STM32_ED_STATE_STATUS_TX                          3
#define UX_DCD_STM32_ED_STATE_STATUS_RX                          4

/* Define USB STM32 device callback notification state definition.  */

#define UX_DCD_STM32_SOF_RECEIVED                                0xF0U
#define UX_DCD_STM32_DEVICE_CONNECTED                            0xF1U
#define UX_DCD_STM32_DEVICE_DISCONNECTED                         0xF2U
#define UX_DCD_STM32_DEVICE_RESUMED                              0xF3U
#define UX_DCD_STM32_DEVICE_SUSPENDED                            0xF4U

/* Define USB STM32 endpoint transfer status definition.  */

#define UX_DCD_STM32_ED_TRANSFER_STATUS_IDLE                     0
#define UX_DCD_STM32_ED_TRANSFER_STATUS_SETUP                    1
#define UX_DCD_STM32_ED_TRANSFER_STATUS_IN_COMPLETION            2
#define UX_DCD_STM32_ED_TRANSFER_STATUS_OUT_COMPLETION           3

/* Define USB STM32 physical endpoint structure.  */

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
    class DCD : public USBXX::DCD
    {
      PCD_TypeDef *pcd;
      PCD_HandleTypeDef hpcd;
      struct UX_SLAVE_DCD_STRUCT
                          *ux_dcd_stm32_dcd_owner;
      UX_DCD_STM32_ED ux_dcd_stm32_ed[UX_DCD_STM32_MAX_ED];
  #if defined(UX_DEVICE_BIDIRECTIONAL_ENDPOINT_SUPPORT)
      UX_DCD_STM32_ED ux_dcd_stm32_ed_in[UX_DCD_STM32_MAX_ED];
  #endif /* defined(UX_DEVICE_BIDIRECTIONAL_ENDPOINT_SUPPORT) */
      PCD_HandleTypeDef   *pcd_handle;



    public:
      DCD(PCD_TypeDef *_pcd);

      void low_level_init();

      uint32_t initialize() override;

      void handle_IRQ();



      inline struct UX_DCD_STM32_ED *_stm32_ed_get(ULONG ep_addr)
      {
        ULONG ep_dir = ep_addr & 0x80u;
        ULONG ep_num = ep_addr & 0x7Fu;

        if (ep_num >= UX_DCD_STM32_MAX_ED ||
            ep_num >= pcd_handle->Init.dev_endpoints)
            return (struct UX_DCD_STM32_ED *)(UX_NULL);

        if (ep_dir)
            return &ux_dcd_stm32_ed_in[ep_num];

        return &ux_dcd_stm32_ed[ep_num];
      }

      UINT create_endpoint(UX_SLAVE_ENDPOINT *endpoint) override;
      UINT destroy_endpoint(UX_SLAVE_ENDPOINT *endpoint) override;
      UINT reset_endpoint(UX_SLAVE_ENDPOINT *endpoint) override;
      UINT stall(UX_SLAVE_ENDPOINT *endpoint) override;
      UINT get_endpoint_status(ULONG endpoint_index) override;
      uint32_t get_frame_number() override;
      UINT complete_initialization() override;
      UINT abort_transfer(UX_SLAVE_TRANSFER *transfer_request) override;
      UINT transfer_request(UX_SLAVE_TRANSFER *transfer_request) override;
      UINT uninitialize() override;

      void setup() override;
      void on_data_in(uint8_t epnum) override;
      void on_data_out(uint8_t epnum) override;
      void reset() override;
      void connect() override;
      void disconnect() override;
      void suspend() override;
      void resume() override;
      void on_sof() override;
      void on_state_change(uint32_t) override;
      void set_device_address(uint8_t) override;
    };

    extern DCD *gDCD;
  }
}
