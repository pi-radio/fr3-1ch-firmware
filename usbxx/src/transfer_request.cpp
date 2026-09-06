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

uint32_t DeviceBase::transfer_request(Transfer *xfer,
    ULONG slave_length,
    ULONG host_length)
{
  UX_INTERRUPT_SAVE_AREA

  UINT                    status;


    /* Do we have to skip this transfer?  */
    if (xfer -> status_phase_ignore == UX_TRUE)
        return 0;

    /* Disable interrupts to prevent the disconnection ISR from preempting us
       while we check the device state and set the transfer status.  */
    UX_DISABLE

    /* We can only transfer when the device is ATTACHED, ADDRESSED OR CONFIGURED.  */
    if ((state == UX_DEVICE_ATTACHED) ||
        (state == UX_DEVICE_ADDRESSED) ||
        (state == UX_DEVICE_CONFIGURED))

        /* Set the transfer to pending.  */
        xfer->set_pending();

    else
    {

        /* The device is in an invalid state. Restore interrupts and return error.  */
        UX_RESTORE
        return(UX_TRANSFER_NOT_READY);
    }

    /* Restore interrupts.  */
    UX_RESTORE
                    
    /* Get the endpoint associated with this transaction.  */
    auto endpoint =  xfer -> endpoint;
    
    /* If the endpoint is non Control, check the endpoint direction and set the data phase direction.  */
    if ((endpoint -> descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE) != UX_CONTROL_ENDPOINT)
    {

        /* Check if the endpoint is STALLED. In this case, we must refuse the transaction until the endpoint
           has been reset by the host.  */
        while (endpoint -> state == UX_ENDPOINT_HALTED)

            /* Wait for 100ms for endpoint to be reset by a CLEAR_FEATURE command.  */
        {
          auto ms_wait = 100;
          /* translate ms into ticks. */
          auto ticks = (ULONG)(ms_wait * UX_PERIODIC_RATE) / 1000;

          /* For safety add 1 to ticks.  */
          ticks++;

          /* Call ThreadX sleep function.  */
          tx_thread_sleep(ticks);
        }

        /* Isolate the direction from the endpoint address.  */
        if ((endpoint -> descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) == UX_ENDPOINT_IN)
            xfer -> phase =  TransferPhase::DATA_OUT;
        else    
            xfer -> phase =  TransferPhase::DATA_IN;
    }    

    /* See if we need to force a zero length packet at the end of the transfer. 
       This happens on a DATA IN and when the host requested length is not met
       and the last packet is on a boundary. If slave_length is zero, then it is 
       a explicit ZLP request, no need to force ZLP.  */
    if ((xfer -> phase ==  TransferPhase::DATA_OUT) &&
        (slave_length != 0) && (host_length != slave_length) && 
        (slave_length % endpoint -> descriptor.wMaxPacketSize) == 0)
    {

        /* If so force Zero Length Packet.  */
        xfer -> force_zlp =  UX_TRUE;
    }
    else
    {

        /* Condition is not met, do not force a Zero Length Packet.  */
        xfer -> force_zlp =  UX_FALSE;
    }

    /* Reset the number of bytes sent/received.  */
    xfer -> actual_length =  0;

    /* Determine how many bytes to send in this transaction.  We keep track of the original
        length and have a working length.  */
    xfer -> requested_length =    slave_length;
    xfer -> in_transfer_length =  slave_length;

    /* Save the buffer pointer.  */
    xfer -> current_data_pointer =
                            xfer -> data;

    /* Call the DCD driver transfer function.   */
    status = xfer->transfer();

    /* And return the status.  */
    return(status);
}

