
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

#include <stdexcept>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/ux_device_stack.h>

#include <usbxx/device.hpp>

using namespace USBXX;

USBXX::STM32::DCD *USBXX::STM32::gDCD;

STM32::DCD::DCD(PCD_TypeDef *_pcd) : pcd(_pcd)
{
  STM32::gDCD = this;
};

void STM32::DCD::low_level_init()
{

}

uint32_t STM32::DCD::initialize()
{
  pcd_handle = &hpcd;

  hpcd.Instance = pcd;
  hpcd.Init.dev_endpoints = 8;
  hpcd.Init.speed = USBD_FS_SPEED;
  hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd.Init.Sof_enable = DISABLE;
  hpcd.Init.low_power_enable = DISABLE;
  hpcd.Init.lpm_enable = DISABLE;
  hpcd.Init.battery_charging_enable = DISABLE;
  hpcd.Init.vbus_sensing_enable = DISABLE;
  hpcd.Init.bulk_doublebuffer_enable = DISABLE;
  hpcd.Init.iso_singlebuffer_enable = DISABLE;

  if (HAL_PCD_Init(&hpcd) != HAL_OK)
  {
    throw std::runtime_error("Unable to initialize USB stack");
  }

  // MOVE ME
  HAL_PCDEx_PMAConfig(&hpcd, 0x00 , PCD_SNG_BUF, 0x40);
  HAL_PCDEx_PMAConfig(&hpcd, 0x80 , PCD_SNG_BUF, 0x80);
  HAL_PCDEx_PMAConfig(&hpcd, 0x01, PCD_SNG_BUF, 0xC0);
  HAL_PCDEx_PMAConfig(&hpcd, 0x81, PCD_SNG_BUF, 0x100);
  HAL_PCDEx_PMAConfig(&hpcd, 0x82, PCD_SNG_BUF, 0x140);

  ux_slave_dcd_status =  UX_DCD_STATUS_OPERATIONAL;

  HAL_PCD_Start(&hpcd);

  /* Return successful completion.  */
  return(UX_SUCCESS);
}

UINT  STM32::DCD::complete_initialization()
{
UX_SLAVE_DEVICE         *device;
UCHAR                     *device_framework;
UX_SLAVE_TRANSFER       *transfer_request;


    /* Get the pointer to the device.  */
    device =  &_ux_system_slave -> ux_system_slave_device;

    /* Are we in DFU mode ? If so, check if we are in a Reset mode.  */
    if (_ux_system_slave -> ux_system_slave_device_dfu_state_machine == UX_SYSTEM_DFU_STATE_APP_DETACH)
    {

        /* The device is now in DFU reset mode. Switch to the DFU device framework.  */
        _ux_system_slave -> ux_system_slave_device_framework =  _ux_system_slave -> ux_system_slave_dfu_framework;
        _ux_system_slave -> ux_system_slave_device_framework_length =  _ux_system_slave -> ux_system_slave_dfu_framework_length;

    }
    else
    {

        /* Set State to App Idle. */
        _ux_system_slave -> ux_system_slave_device_dfu_state_machine = UX_SYSTEM_DFU_STATE_APP_IDLE;

        /* Check the speed and set the correct descriptor.  */
        if (_ux_system_slave -> ux_system_slave_speed ==  UX_FULL_SPEED_DEVICE)
        {

            /* The device is operating at full speed.  */
            _ux_system_slave -> ux_system_slave_device_framework =  _ux_system_slave -> ux_system_slave_device_framework_full_speed;
            _ux_system_slave -> ux_system_slave_device_framework_length =  _ux_system_slave -> ux_system_slave_device_framework_length_full_speed;
        }
        else
        {

            /* The device is operating at high speed.  */
            _ux_system_slave -> ux_system_slave_device_framework =  _ux_system_slave -> ux_system_slave_device_framework_high_speed;
            _ux_system_slave -> ux_system_slave_device_framework_length =  _ux_system_slave -> ux_system_slave_device_framework_length_high_speed;
        }
    }

    /* Get the device framework pointer.  */
    device_framework =  _ux_system_slave -> ux_system_slave_device_framework;

    /* And create the decompressed device descriptor structure.  */
    _ux_utility_descriptor_parse(device_framework,
                                _ux_system_device_descriptor_structure,
                                UX_DEVICE_DESCRIPTOR_ENTRIES,
                                (UCHAR *) &device -> ux_slave_device_descriptor);

    /* Now we create a transfer request to accept the first SETUP packet
       and get the ball running. First get the address of the endpoint
       transfer request container.  */
    transfer_request =  &device -> ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;

    /* Set the timeout to be for Control Endpoint.  */
    transfer_request -> ux_slave_transfer_request_timeout =  UX_MS_TO_TICK(UX_CONTROL_TRANSFER_TIMEOUT);

    /* Adjust the current data pointer as well.  */
    transfer_request -> ux_slave_transfer_request_current_data_pointer =
                            transfer_request -> ux_slave_transfer_request_data_pointer;

    /* Update the transfer request endpoint pointer with the default endpoint.  */
    transfer_request -> ux_slave_transfer_request_endpoint =  &device -> ux_slave_device_control_endpoint;

    /* The control endpoint max packet size needs to be filled manually in its descriptor.  */
    transfer_request -> ux_slave_transfer_request_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize =
                                device -> ux_slave_device_descriptor.bMaxPacketSize0;

    /* On the control endpoint, always expect the maximum.  */
    transfer_request -> ux_slave_transfer_request_requested_length =
                                device -> ux_slave_device_descriptor.bMaxPacketSize0;

    /* Attach the control endpoint to the transfer request.  */
    transfer_request -> ux_slave_transfer_request_endpoint =  &device -> ux_slave_device_control_endpoint;

    /* Create the default control endpoint attached to the device.
       Once this endpoint is enabled, the host can then send a setup packet
       The device controller will receive it and will call the setup function
       module.  */
    create_endpoint(&device -> ux_slave_device_control_endpoint);

    /* Open Control OUT endpoint.  */
    HAL_PCD_EP_Flush(pcd_handle, 0x00U);
    HAL_PCD_EP_Open(pcd_handle, 0x00U, device -> ux_slave_device_descriptor.bMaxPacketSize0, UX_CONTROL_ENDPOINT);

    /* Open Control IN endpoint.  */
    HAL_PCD_EP_Flush(pcd_handle, 0x80U);
    HAL_PCD_EP_Open(pcd_handle, 0x80U, device -> ux_slave_device_descriptor.bMaxPacketSize0, UX_CONTROL_ENDPOINT);

    /* Ensure the control endpoint is properly reset.  */
    device -> ux_slave_device_control_endpoint.ux_slave_endpoint_state = UX_ENDPOINT_RESET;

    /* Mark the phase as SETUP.  */
    transfer_request -> ux_slave_transfer_request_type =  UX_TRANSFER_PHASE_SETUP;

    /* Mark this transfer request as pending.  */
    transfer_request -> ux_slave_transfer_request_status =  UX_TRANSFER_STATUS_PENDING;

    /* Ask for 8 bytes of the SETUP packet.  */
    transfer_request -> ux_slave_transfer_request_requested_length =    UX_SETUP_SIZE;
    transfer_request -> ux_slave_transfer_request_in_transfer_length =  UX_SETUP_SIZE;

    /* Reset the number of bytes sent/received.  */
    transfer_request -> ux_slave_transfer_request_actual_length =  0;

    /* Check the status change callback.  */
    if(_ux_system_slave -> ux_system_slave_change_function != UX_NULL)
    {
      _ux_system_slave -> ux_system_slave_change_function(UX_DEVICE_ATTACHED);
    }

    return(UX_SUCCESS);
}

UINT  STM32::DCD::uninitialize()
{
    /* Set the state of the controller to HALTED now.  */
  ux_slave_dcd_status =  UX_DCD_STATUS_HALTED;

  return(UX_SUCCESS);
}



void STM32::DCD::set_device_address(uint8_t addr)
{
  HAL_PCD_SetAddress(pcd_handle, addr);
}

void STM32::DCD::handle_IRQ()
{
  HAL_PCD_IRQHandler(&hpcd);
}

uint32_t STM32::DCD::get_frame_number()
{

    /* This function never fails. */
    return(UX_SUCCESS);
}


extern "C" void USB_DRD_FS_IRQHandler(void)
{
  STM32::gDCD->handle_IRQ();
}
