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
/**   Device Stack                                                        */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include <cstdint>

#include <usbxx/ux_api.h>
#include <usbxx/ux_device_stack.h>

#include <usbxx/device.hpp>


UCHAR _ux_system_slave_class_storage_name[] =                               "ux_slave_class_storage";
UCHAR _ux_system_slave_class_cdc_acm_name[] =                               "ux_slave_class_cdc_acm";
UCHAR _ux_system_slave_class_dpump_name[] =                                 "ux_slave_class_dpump";
UCHAR _ux_system_slave_class_pima_name[] =                                  "ux_slave_class_pima";
UCHAR _ux_system_slave_class_hid_name[] =                                   "ux_slave_class_hid";
UCHAR _ux_system_slave_class_rndis_name[] =                                 "ux_slave_class_rndis";
UCHAR _ux_system_slave_class_cdc_ecm_name[] =                               "ux_slave_class_cdc_ecm";
UCHAR _ux_system_slave_class_dfu_name[] =                                   "ux_slave_class_dfu";
UCHAR _ux_system_slave_class_audio_name[] =                                 "ux_slave_class_audio";

UCHAR _ux_system_device_class_printer_name[] =                              "ux_device_class_printer";
UCHAR _ux_system_device_class_ccid_name[] =                                 "ux_device_class_ccid";
UCHAR _ux_system_device_class_video_name[] =                                "ux_device_class_video";

#if 0

UINT  _ux_device_stack_initialize(UCHAR * device_framework_high_speed, ULONG device_framework_length_high_speed,
                                  UCHAR * device_framework_full_speed, ULONG device_framework_length_full_speed,
                                  UCHAR * string_framework, ULONG string_framework_length,
                                  UCHAR * language_id_framework, ULONG language_id_framework_length,
                                  UINT (*ux_system_slave_change_function)(ULONG))
{
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_device_stack_initialize                        PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in device stack initialization          */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    class_name                            Name of class                 */
/*    class_function_entry                  Class entry function          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_device_stack_initialize           Device Stack Initialize       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2023     Chaoqiong Xiao           Initial Version 6.3.0         */
/*                                                                        */
/**************************************************************************/
UINT  _uxe_device_stack_initialize(UCHAR * device_framework_high_speed, ULONG device_framework_length_high_speed,
                                  UCHAR * device_framework_full_speed, ULONG device_framework_length_full_speed,
                                  UCHAR * string_framework, ULONG string_framework_length,
                                  UCHAR * language_id_framework, ULONG language_id_framework_length,
                                  UINT (*ux_system_slave_change_function)(ULONG))
{

    /* Sanity checks.  */
    if (((device_framework_high_speed == nullptr) && (device_framework_length_high_speed != 0)) ||
        (device_framework_full_speed == nullptr) || (device_framework_length_full_speed == 0) ||
        ((string_framework == nullptr) && (string_framework_length != 0)) ||
        (language_id_framework == nullptr) || (language_id_framework_length == 0))
        return(UX_INVALID_PARAMETER);

    /* Invoke stack initialize function.  */
    return(_ux_device_stack_initialize(device_framework_high_speed, device_framework_length_high_speed,
                                       device_framework_full_speed, device_framework_length_full_speed,
                                       string_framework, string_framework_length,
                                       language_id_framework, language_id_framework_length,
                                       ux_system_slave_change_function));
}
#endif
