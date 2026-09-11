/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** USBX Component                                                        */
/**                                                                       */
/**   STM32 Controller Driver                                             */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

/* Include necessary system files.  */

#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>

#include <usbxx/endian.hpp>
#include <usbxx/device.hpp>

using namespace USBXX;

void STM32::DCD::reset()
{
  disconnect();

  current_speed = desired_speed;

  /* Complete the device initialization.  */
  complete_initialization();

  /* Mark the device as attached now.  */
  device->set_state(DeviceState::ATTACHED);
}

void STM32::DCD::connect()
{
  device->on_connected();
}

void STM32::DCD::disconnect()
{
  if (device->get_state() == DeviceState::RESET)
    return;

  std::vector<uint8_t> eps;

  for (auto t : endpoints) {
    auto a = std::get<0>(t);

    if (a & 0x7F) {
      eps.push_back(a);
    }
  }

  for (auto a : eps) {
    endpoints.erase(a);
  }

  device->on_disconnected();

  device->disconnect();
}

void STM32::DCD::suspend()
{
  device->on_suspended();
}

void STM32::DCD::resume()
{
  device->on_resumed();
}

void STM32::DCD::on_sof()
{
  device->on_sof();
}



void STM32::DCD::on_state_change(DeviceState state)
{
  if (state == DeviceState::FORCE_DISCONNECT)
  {
    stop();
  }
}
