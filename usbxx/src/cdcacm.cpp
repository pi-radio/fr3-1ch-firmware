/*
 * usbxxcdcacm.cpp
 *
 *  Created on: Feb 21, 2026
 *      Author: zapman
 */

#include <threadxx/dbgstream.hpp>

#include <ux_api.h>

#include <usbxx/descriptor.hpp>

using namespace USBXX;

Class_CDCACM::Class_CDCACM(DeviceInfo *_dev) : ClassInfo(dev, Descriptors::CDC_ACM, 2, 0)
{
}
