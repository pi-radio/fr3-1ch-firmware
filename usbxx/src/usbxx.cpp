/*
 * usbxx.cpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */
#include <stdexcept>

#include <usbxx/usbxx.hpp>

#include <ux_api.h>

#include <usb.h>

USBXX::SystemBase::SystemBase()
{
}

void USBXX::SystemBase::start_system(uint8_t *stack, size_t n)
{
    if (ux_system_initialize(stack, n, UX_NULL, 0) != UX_SUCCESS)
    {
      throw std::runtime_error("Unable to start USBX system");
    }
}

void USBXX::DeviceBase::thread_entry()
{
  dbg::dbgout << "Initializing USB Hardware" << std::endl;

  MX_USB_PCD_Init();
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x00 , PCD_SNG_BUF, 0x40);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x80 , PCD_SNG_BUF, 0x80);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x01, PCD_SNG_BUF, 0xC0);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x81, PCD_SNG_BUF, 0x100);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x82, PCD_SNG_BUF, 0x140);
  ux_dcd_stm32_initialize((ULONG)USB_DRD_FS, (ULONG)&hpcd_USB_DRD_FS);
  HAL_PCD_Start(&hpcd_USB_DRD_FS);

  // Call app thread, if desired

  dbg::dbgout << "Hardware initialized" << std::endl;
}


USBXX::DeviceBase *_devbase = NULL;

USBXX::DeviceBase::DeviceBase()
{
  _devbase = this;
}

UINT USBXX::_usbxx_change_notification(ULONG new_state)
{
  return _devbase->on_change(new_state);
}

uint32_t USBXX::DeviceBase::on_change(uint32_t new_state)
{
  switch (new_state)
  {
    case UX_DEVICE_ATTACHED:
      return on_attached();
    case UX_DEVICE_REMOVED:
      return on_removed();
    case UX_DCD_STM32_DEVICE_CONNECTED:
      return on_connected();
    case UX_DCD_STM32_DEVICE_DISCONNECTED:
      return on_disconnected();
    case UX_DCD_STM32_DEVICE_SUSPENDED:
      return on_suspended();
    case UX_DCD_STM32_DEVICE_RESUMED:
      return on_resumed();
    case UX_DCD_STM32_SOF_RECEIVED:
      return on_sof();
    default:
      dbg::dbgout << "Unknown state: " << new_state << std::endl;
      return UX_SUCCESS;
  }

}

void USBXX::DeviceBase::setup_device()
{
  UCHAR *device_framework_high_speed;
  UCHAR *device_framework_full_speed;
  ULONG device_framework_hs_length;
  ULONG device_framework_fs_length;
  ULONG string_framework_length;
  ULONG language_id_framework_length;
  UCHAR *string_framework;
  UCHAR *language_id_framework;

  /* Get Device Framework High Speed and get the length */
  device_framework_high_speed = USBD_Get_Device_Framework_Speed(USBD_HIGH_SPEED,
                                                                &device_framework_hs_length);

  /* Get Device Framework Full Speed and get the length */
  device_framework_full_speed = USBD_Get_Device_Framework_Speed(USBD_FULL_SPEED,
                                                                &device_framework_fs_length);

  /* Get String Framework and get the length */
  string_framework = USBD_Get_String_Framework(&string_framework_length);

  /* Get Language Id Framework and get the length */
  language_id_framework = USBD_Get_Language_Id_Framework(&language_id_framework_length);

  /* Install the device portion of USBX */
  if (ux_device_stack_initialize(device_framework_high_speed,
                                 device_framework_hs_length,
                                 device_framework_full_speed,
                                 device_framework_fs_length,
                                 string_framework,
                                 string_framework_length,
                                 language_id_framework,
                                 language_id_framework_length,
                                 _usbxx_change_notification) != UX_SUCCESS)
  {
    throw std::runtime_error("Failed to initialize USB device stack\n");
  }

}


void USBXX::DeviceBase::start()
{
  try {
    start_system();
    setup_device();
    register_class();
    start_app();
  } catch(std::runtime_error &e) {
    dbg::dbgout << "Exception in starting USB device: " << e.what() << std::endl;
  } catch (...) {
    dbg::dbgout << "Unknown exception in starting USB device!" << std::endl;
  }

}

void USBXX::CDCACM::register_class()
{
  cdc_acm_parameter.ux_slave_class_cdc_acm_instance_activate   = USBD_CDC_ACM_Activate;
  cdc_acm_parameter.ux_slave_class_cdc_acm_instance_deactivate = USBD_CDC_ACM_Deactivate;
  cdc_acm_parameter.ux_slave_class_cdc_acm_parameter_change    = USBD_CDC_ACM_ParameterChange;

  cdc_acm_configuration_number = USBD_Get_Configuration_Number(CLASS_TYPE_CDC_ACM, 0);

  cdc_acm_interface_number = USBD_Get_Interface_Number(CLASS_TYPE_CDC_ACM, 0);

  /* Initialize the device cdc acm class */
  if (ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name,
                                     ux_device_class_cdc_acm_entry,
                                     cdc_acm_configuration_number,
                                     cdc_acm_interface_number,
                                     &cdc_acm_parameter) != UX_SUCCESS)
  {
    throw std::runtime_error("Failed to register CDC ACM class");
  }

}

#if 0
  class System
  {
    uint8_t _system_stack[system_stack_size];

  public:

  template <size_t system_stack_size, size_t app_stack_size>
  class Device
  {
    class AppThread : TXX::Thread<app_stack_size> {
      AppThread() : TXX::Thread<app_stack_size>("USB Device Thread") {}
      void main() override;
    };

    System<system_stack_size> system;
    AppThread app_thread;


    friend void _usbxx_change_notification();
  public:
    Device();

    void start();
  };

void TXX::USBXXBase::start_system(uint8_t *stack, int stack_size)
{
}
#endif
