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

