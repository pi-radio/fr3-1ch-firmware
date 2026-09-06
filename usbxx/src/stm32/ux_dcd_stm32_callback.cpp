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

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>
#include <usbxx/ux_device_stack.h>
#include <usbxx/ux_utility.h>

#include <usbxx/endian.hpp>
#include <usbxx/device.hpp>

using namespace USBXX;

void STM32::DCD::reset()
{

    /* If the device is attached or configured, we need to disconnect it.  */
    if (device->state !=  UX_DEVICE_RESET)
    {
      device->disconnect();
    }

    /* Set USB Current Speed */
    switch(hpcd.Init.speed)
    {
    case PCD_SPEED_HIGH:
        _ux_system_slave -> ux_system_slave_speed =  UX_HIGH_SPEED_DEVICE;
        break;
    case PCD_SPEED_FULL:
        _ux_system_slave -> ux_system_slave_speed =  UX_FULL_SPEED_DEVICE;
        break;
    default:
        _ux_system_slave -> ux_system_slave_speed =  UX_FULL_SPEED_DEVICE;
        break;
    }

    /* Complete the device initialization.  */
    complete_initialization();

    /* Mark the device as attached now.  */
    device->state =  UX_DEVICE_ATTACHED;
}

void STM32::DCD::connect()
{
  device->on_connected();
}

void STM32::DCD::disconnect()
{
  device->on_disconnected();

  /* Check if the device is attached or configured.  */
  if (device->state != UX_DEVICE_RESET)
  {
    device->disconnect();
  }
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

void STM32::DCD::on_state_change(uint32_t state)
{
  if ((ULONG) state == UX_DEVICE_FORCE_DISCONNECT)
    HAL_PCD_Stop(pcd_handle);
}
