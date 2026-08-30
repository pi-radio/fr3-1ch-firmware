
#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE

#include <stdexcept>

/* Include necessary system files.  */

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>
#include <usbxx/ux_device_stack.h>

using namespace USBXX;

UINT STM32::DCD::create_endpoint(UX_SLAVE_ENDPOINT *endpoint)
{
ULONG               stm32_endpoint_index;


    /* The endpoint index in the array of the STM32 must match the endpoint number.  */
    stm32_endpoint_index =  endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & ~UX_ENDPOINT_DIRECTION;


    /* Get STM32 ED.  */
    auto ed = _stm32_ed_get(endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress);

    if (ed == UX_NULL)
        return(UX_NO_ED_AVAILABLE);

    /* Check the endpoint status, if it is free, reserve it. If not reject this endpoint.  */
    if ((ed -> ux_dcd_stm32_ed_status & UX_DCD_STM32_ED_STATUS_USED) == 0)
    {

        /* We can use this endpoint.  */
        ed -> ux_dcd_stm32_ed_status |=  UX_DCD_STM32_ED_STATUS_USED;

        /* Keep the physical endpoint address in the endpoint container.  */
        endpoint -> ux_slave_endpoint_ed =  (VOID *) ed;

        /* Save the endpoint pointer.  */
        ed -> ux_dcd_stm32_ed_endpoint =  endpoint;

        /* And its index.  */
        ed -> ux_dcd_stm32_ed_index =  stm32_endpoint_index;

        /* And its direction.  */
        ed -> ux_dcd_stm32_ed_direction =  endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION;

        /* Check if it is non-control endpoint.  */
        if (stm32_endpoint_index != 0)
        {

            /* Open the endpoint.  */
            HAL_PCD_EP_Open(pcd_handle, endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress,
                            endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize,
                            endpoint -> ux_slave_endpoint_descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE);
        }

        /* Return successful completion.  */
        return(UX_SUCCESS);
    }

    /* Return an error.  */
    return(UX_NO_ED_AVAILABLE);
}

UINT STM32::DCD::destroy_endpoint(UX_SLAVE_ENDPOINT *endpoint)
{
 auto ed =  (UX_DCD_STM32_ED *) endpoint -> ux_slave_endpoint_ed;

 ed -> ux_dcd_stm32_ed_status =  UX_DCD_STM32_ED_STATUS_UNUSED;

  /* Deactivate the endpoint.  */
  HAL_PCD_EP_Close(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);

  /* This function never fails.  */
  return(UX_SUCCESS);
}

UINT STM32::DCD::get_endpoint_status(ULONG endpoint_index)
{
  UX_DCD_STM32_ED      *ed;

  /* Fetch the address of the physical endpoint.  */
  ed = _stm32_ed_get(endpoint_index);

  /* Check the endpoint status, if it is free, we have a illegal endpoint.  */
  if ((ed -> ux_dcd_stm32_ed_status & UX_DCD_STM32_ED_STATUS_USED) == 0)
      return(UX_ERROR);

  /* Check if the endpoint is stalled.  */
  if ((ed -> ux_dcd_stm32_ed_status & UX_DCD_STM32_ED_STATUS_STALLED) == 0)
      return(UX_FALSE);
  else
      return(UX_TRUE);
}

UINT  STM32::DCD::reset_endpoint(UX_SLAVE_ENDPOINT *endpoint)
{
  auto ed =  (UX_DCD_STM32_ED *) endpoint -> ux_slave_endpoint_ed;
  UX_INTERRUPT_SAVE_AREA

  UX_DISABLE

  /* Set the status of the endpoint to not stalled.  */
  ed->ux_dcd_stm32_ed_status &= ~(UX_DCD_STM32_ED_STATUS_STALLED |
                                    UX_DCD_STM32_ED_STATUS_DONE |
                                    UX_DCD_STM32_ED_STATUS_SETUP);

  /* Set the state of the endpoint to IDLE.  */
  ed->ux_dcd_stm32_ed_state =  UX_DCD_STM32_ED_STATE_IDLE;

  /* Clear STALL condition.  */
  HAL_PCD_EP_ClrStall(pcd_handle, endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress);

  /* Flush buffer.  */
  HAL_PCD_EP_Flush(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);

#ifndef UX_DEVICE_STANDALONE

  /* Wakeup pending thread.  */
  if (endpoint -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_semaphore.tx_semaphore_suspended_count)
      _ux_utility_semaphore_put(&endpoint -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_semaphore);
#endif

  UX_RESTORE

  /* This function never fails.  */
  return(UX_SUCCESS);
}


UINT  STM32::DCD::stall(UX_SLAVE_ENDPOINT *endpoint)
{
  auto ed =  (UX_DCD_STM32_ED *) endpoint -> ux_slave_endpoint_ed;

  ed->ux_dcd_stm32_ed_status |=  UX_DCD_STM32_ED_STATUS_STALLED;

    /* Stall the endpoint.  */
  HAL_PCD_EP_SetStall(pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress | ed -> ux_dcd_stm32_ed_direction);

  /* This function never fails.  */
  return(UX_SUCCESS);
}
