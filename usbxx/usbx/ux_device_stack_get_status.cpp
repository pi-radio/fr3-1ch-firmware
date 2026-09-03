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

#include <usbxx/ux_api.h>
#include <usbxx/ux_device_stack.h>

#include <usbxx/stm32/dcd.hpp>

#include <usbxx/device.hpp>

using namespace USBXX;

UINT  _ux_device_stack_get_status(ULONG request_type, ULONG request_index, ULONG request_length)
{

USBXX::DCD            *dcd;
Transfer       *xfer;
UINT                    status;
ULONG                   data_length;

    UX_PARAMETER_NOT_USED(request_length);

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_GET_STATUS, request_type, request_index, request_length, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)

    /* Get the pointer to the DCD.  */
    dcd = STM32::gDCD;

    /* Get the pointer to the device.  */
    auto device = _ux_system_slave->device;

    /* Get the control endpoint for the device.  */
    auto endpoint =  device -> get_control_endpoint();

    /* Get the pointer to the transfer request associated with the endpoint.  */
    xfer = device->get_control_transfer();

    /* Reset the status buffer.  */
    *xfer -> data =  0;
    *(xfer -> data + 1) =  0;
    
    /* The default length for GET_STATUS is 2, except for OTG get Status.  */
    data_length = 2;
    
    /* The status can be for either the device or the endpoint.  */
    switch (request_type & UX_REQUEST_TARGET)
    {
    
    case UX_REQUEST_TARGET_DEVICE:

        /* When the device is probed, it is either for the power/remote capabilities or OTG role swap.  
           We differentiate with the Windex, 0 or OTG status Selector.  */
        if (request_index == UX_OTG_STATUS_SELECTOR)
        {

            /* Set the data length to 1.  */
            data_length = 1;

        }
        else
        {

            /* Store the current power state in the status buffer. */
            if (_ux_system_slave -> ux_system_slave_power_state == UX_DEVICE_SELF_POWERED)
                *xfer -> data =  1;

            /* Store the remote wakeup capability state in the status buffer.  */

            if (_ux_system_slave -> ux_system_slave_remote_wakeup_enabled)
                *xfer -> data |=  2;
        }
        
        break;
            
    case UX_REQUEST_TARGET_ENDPOINT:
    {
      auto tgt = dcd->get_endpoint(request_index);

      if (tgt->is_stalled()) {
        *xfer -> data = 1;
      }

      break;
    }

    default:
        endpoint->stall();
    
        /* No more work to do here.  The command failed but the upper layer does not depend on it.  */
        return 0;            
    }
    
    /* Set the phase of the transfer to data out.  */
    xfer -> phase =  TransferPhase::DATA_OUT;

    /* Send the descriptor with the appropriate length to the host.  */
    status = device->transfer_request(xfer, data_length, data_length);

    /* Return the function status.  */
    return(status);
}

