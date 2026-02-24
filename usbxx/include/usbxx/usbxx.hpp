/*
 * usbxx.hpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */

#ifndef USBXX_HPP_
#define USBXX_HPP_

#include <app_usbx_device.h>

#include <usbxx/device.hpp>

namespace USBXX
{
  // Make app stack paramaterizable
  class CDCACM : public Device<8192, 2048> {
    ULONG cdc_acm_interface_number;
    ULONG cdc_acm_configuration_number;
    UX_SLAVE_CLASS_CDC_ACM_PARAMETER cdc_acm_parameter;

    void register_class() override;
  };
};


#endif /* USBXX_HPP_ */
