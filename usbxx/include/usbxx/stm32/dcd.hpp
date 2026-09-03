#pragma once

#include <usbxx/dcd.hpp>

#include <usbxx/ux_stm32_config.h>

#include <usbxx/stm32/endpoint.hpp>

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



/* Define USB STM32 physical endpoint state machine definition.  */


/* Define USB STM32 device callback notification state definition.  */

#define UX_DCD_STM32_SOF_RECEIVED                                0xF0U
#define UX_DCD_STM32_DEVICE_CONNECTED                            0xF1U
#define UX_DCD_STM32_DEVICE_DISCONNECTED                         0xF2U
#define UX_DCD_STM32_DEVICE_RESUMED                              0xF3U
#define UX_DCD_STM32_DEVICE_SUSPENDED                            0xF4U

/* Define USB STM32 endpoint transfer status definition.  */

#define TRANSFER_STATUS_IDLE                     0
#define TRANSFER_STATUS_SETUP                    1
#define TRANSFER_STATUS_IN_COMPLETION            2
#define TRANSFER_STATUS_OUT_COMPLETION           3

/* Define USB STM32 physical endpoint structure.  */



namespace USBXX
{
  namespace STM32
  {

    class DCD : public USBXX::DCD
    {
      PCD_TypeDef *pcd;
      PCD_HandleTypeDef hpcd;
      STM32::Endpoint ep_out[UX_DCD_STM32_MAX_ED];
      STM32::Endpoint ep_in[UX_DCD_STM32_MAX_ED];
      PCD_HandleTypeDef   *pcd_handle;

      void control_IRQ();
      void endpoint_IRQ();

      HAL_StatusTypeDef transmit(PCD_EPTypeDef *ep, uint16_t wEPVal);
      uint16_t receive(PCD_EPTypeDef *ep, uint16_t wEPVal);

    public:
      DCD(PCD_TypeDef *_pcd);

      void low_level_init();

      uint32_t initialize() override;

      void handle_IRQ();

      PCD_HandleTypeDef *get_pcd_handle() { return pcd_handle; }

      inline struct STM32::Endpoint *__get_endpoint(ULONG ep_addr)
      {
        ULONG ep_dir = ep_addr & 0x80;
        ULONG ep_num = ep_addr & 0x7F;

        if (ep_num == 0) {
          return &ep_out[0];
        }

        if (ep_num >= UX_DCD_STM32_MAX_ED ||
            ep_num >= pcd_handle->Init.dev_endpoints)
            return nullptr;

        if (ep_dir)
            return &ep_in[ep_num];

        return &ep_out[ep_num];
      }

      Endpoint *get_endpoint(uint8_t epaddr) { return __get_endpoint(epaddr); }
      Endpoint *get_control_endpoint() override { return &ep_out[0]; };
      Transfer *get_control_transfer() override { return ep_out[0].get_transfer(); };

      USBXX::Endpoint *allocate_endpoint(std::shared_ptr<Interface>, const EndpointDescriptor &) override;
      uint32_t get_frame_number() override;
      UINT complete_initialization() override;
      UINT transfer_request(USBXX::Transfer *transfer_request) override;
      UINT uninitialize() override;

      UINT transfer_out(Transfer *xfer);
      UINT transfer_in(Transfer *xfer);

      void on_control_in();

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
