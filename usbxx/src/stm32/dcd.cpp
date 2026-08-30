
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE


/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/ux_device_stack.h>

UINT  _ux_dcd_stm32_initialize(ULONG dcd_io, ULONG parameter)
{

UX_SLAVE_DCD            *dcd;
UX_DCD_STM32            *dcd_stm32;


    UX_PARAMETER_NOT_USED(dcd_io);

    /* Get the pointer to the DCD.  */
    dcd =  &_ux_system_slave -> ux_system_slave_dcd;

    /* The controller initialized here is of STM32 type.  */
    dcd -> ux_slave_dcd_controller_type =  UX_DCD_STM32_SLAVE_CONTROLLER;

    /* Allocate memory for this STM32 DCD instance.  */
    dcd_stm32 = (UX_DCD_STM32 *)::malloc(sizeof(UX_DCD_STM32));

    /* Check if memory was properly allocated.  */
    if(dcd_stm32 == UX_NULL)
        return(UX_MEMORY_INSUFFICIENT);

    ::memset(dcd_stm32, 0, sizeof(UX_DCD_STM32));

    /* Set the pointer to the STM32 DCD.  */
    dcd -> ux_slave_dcd_controller_hardware =  (VOID *) dcd_stm32;

    /* Set the generic DCD owner for the STM32 DCD.  */
    dcd_stm32 -> ux_dcd_stm32_dcd_owner =  dcd;

    /* Initialize the function collector for this DCD.  */
    dcd -> ux_slave_dcd_function =  _ux_dcd_stm32_function;

    dcd_stm32 -> pcd_handle = (PCD_HandleTypeDef *)parameter;

    /* Set the state of the controller to OPERATIONAL now.  */
    dcd -> ux_slave_dcd_status =  UX_DCD_STATUS_OPERATIONAL;

    /* Return successful completion.  */
    return(UX_SUCCESS);
}
