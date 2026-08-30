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
/**   System                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Define UX_SYSTEM_INIT to bring in the USBX version ID string.  */

#define UX_SYSTEM_INIT


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include <usbxx/ux_api.h>
#include <usbxx/ux_system.h>

/* Define the USBX system data structure.  */
/* Define names of all the packed descriptors in USBX.  */

UCHAR _ux_system_endpoint_descriptor_structure[] =                          {1,1,1,1,2,1 };
UCHAR _ux_system_device_descriptor_structure[] =                            {1,1,2,1,1,1,1,2,2,2,1,1,1,1};
UCHAR _ux_system_configuration_descriptor_structure[] =                     {1,1,2,1,1,1,1,1};
UCHAR _ux_system_interface_descriptor_structure[] =                         {1,1,1,1,1,1,1,1,1};
UCHAR _ux_system_interface_association_descriptor_structure[] =             {1,1,1,1,1,1,1,1};
UCHAR _ux_system_string_descriptor_structure[] =                            {1,1,2};
UCHAR _ux_system_dfu_functional_descriptor_structure[] =                    {1,1,1,2,2,2};
UCHAR _ux_system_class_audio_interface_descriptor_structure[] =             {1,1,1,1,1,1,1,1};
UCHAR _ux_system_class_audio_input_terminal_descriptor_structure[] =        {1,1,1,1,2,1,1,2,1,1};
UCHAR _ux_system_class_audio_output_terminal_descriptor_structure[] =       {1,1,1,1,2,1,1,1};
UCHAR _ux_system_class_audio_feature_unit_descriptor_structure[] =          {1,1,1,1,1,1,1};
UCHAR _ux_system_class_audio_streaming_interface_descriptor_structure[] =   {1,1,1,1,1,1};
UCHAR _ux_system_class_audio_streaming_endpoint_descriptor_structure[] =    {1,1,1,1,1,1};
UCHAR _ux_system_hub_descriptor_structure[] =                               {1,1,1,2,1,1,1,1};
UCHAR _ux_system_hid_descriptor_structure[] =                               {1,1,2,1,1,1,2};
UCHAR _ux_system_class_pima_storage_structure[] =                           {2,2,2,4,4,4,4,4};
UCHAR _ux_system_class_pima_object_structure[] =                            {4,2,2,4,2,4,4,4,4,4,4,4,2,4,4};
UCHAR _ux_system_ecm_interface_descriptor_structure[] =                     {1,1,1,1,4,2,2,1};

UCHAR _ux_system_bos_descriptor_structure[] =                               {1,1,2,1};
UCHAR _ux_system_usb_2_0_extension_descriptor_structure[] =                 {1,1,1,4};
UCHAR _ux_system_container_id_descriptor_structure[] =                      {1,1,1,1,4,4,4,4};


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_system_initialize                               PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the various control data structures for   */
/*    the USBX system.                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    regular_memory_pool_start        Start of non cached memory pool    */
/*    regular_memory_size              Size of non cached memory pool     */
/*    cache_safe_memory_pool_start     Start of cached memory pool        */
/*    cache_safe_memory_size           Size of cached memory pool         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_utility_memory_allocate           Allocate memory               */
/*    _ux_utility_memory_set                Set memory                    */
/*    _ux_utility_mutex_create              Create mutex                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            verified memset and memcpy  */
/*                                            cases,                      */
/*                                            resulting in version 6.1    */
/*  12-31-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added BOS support,          */
/*                                            resulting in version 6.1.3  */
/*  01-31-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added standalone support,   */
/*                                            resulting in version 6.1.10 */
/*  10-31-2023     Chaoqiong Xiao           Modified comment(s),          */
/*                                            refined memory management,  */
/*                                            added UX_ASSERT check for   */
/*                                            STD descriptor parse size,  */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
UINT  _ux_system_initialize(VOID *regular_memory_pool_start, ULONG regular_memory_size,
                            VOID *cache_safe_memory_pool_start, ULONG cache_safe_memory_size)
{
ALIGN_TYPE          int_memory_pool_start;
VOID                *regular_memory_pool_end;
VOID                *cache_safe_memory_pool_end;
ULONG               memory_pool_offset;
#if !defined(UX_STANDALONE)
UINT                status;
#endif
ULONG               pool_size;

    /* Create the Mutex object used by USBX to control critical sections.  */
    status =  _ux_system_mutex_create(&_ux_system -> ux_system_mutex, "ux_system_mutex");
    if(status != UX_SUCCESS)
        return(UX_MUTEX_ERROR);

    return(UX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_system_initialize                              PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in system initialization function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    regular_memory_pool_start        Start of non cached memory pool    */
/*    regular_memory_size              Size of non cached memory pool     */
/*    cache_safe_memory_pool_start     Start of cached memory pool        */
/*    cache_safe_memory_size           Size of cached memory pool         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_system_initialize                 Get encoded feedback          */
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
UINT  _uxe_system_initialize(VOID *regular_memory_pool_start, ULONG regular_memory_size,
                            VOID *cache_safe_memory_pool_start, ULONG cache_safe_memory_size)
{

    /* Compiling option check of descriptors structs.  */
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_endpoint_descriptor_structure, UX_ENDPOINT_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_ENDPOINT_DESCRIPTOR));
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_device_descriptor_structure, UX_DEVICE_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_DEVICE_DESCRIPTOR));
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_configuration_descriptor_structure, UX_CONFIGURATION_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_CONFIGURATION_DESCRIPTOR));
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_interface_descriptor_structure, UX_INTERFACE_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_INTERFACE_DESCRIPTOR));
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_interface_association_descriptor_structure, UX_INTERFACE_ASSOCIATION_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_INTERFACE_ASSOCIATION_DESCRIPTOR));
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_string_descriptor_structure, UX_STRING_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_STRING_DESCRIPTOR));
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_dfu_functional_descriptor_structure, UX_DFU_FUNCTIONAL_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_DFU_FUNCTIONAL_DESCRIPTOR));
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_bos_descriptor_structure, UX_BOS_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_BOS_DESCRIPTOR));
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_usb_2_0_extension_descriptor_structure, UX_USB_2_0_EXTENSION_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_USB_2_0_EXTENSION_DESCRIPTOR));
    UX_ASSERT((_ux_utility_descriptor_parse_size(_ux_system_container_id_descriptor_structure, UX_CONTAINER_ID_DESCRIPTOR_ENTRIES, 0x3u)) == sizeof(UX_CONTAINER_ID_DESCRIPTOR));


    /* Sanity check.  */
    if ((regular_memory_pool_start == nullptr) || (regular_memory_size == 0))
            return(UX_INVALID_PARAMETER);

    /* Invoke system initialization function.  */
    return(_ux_system_initialize(regular_memory_pool_start, regular_memory_size,
                                 cache_safe_memory_pool_start, cache_safe_memory_size));
}
