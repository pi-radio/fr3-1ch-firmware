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

static inline void _ux_dcd_stm32_setup_in(STM32::Endpoint * ed, Transfer *transfer_request)
{
    ed -> direction = UX_ENDPOINT_IN;
    ed -> state = STM32::EndpointState::DATA_TX;
    ed->ux_slave_endpoint_device->process_control_event(transfer_request);
}

static inline void _ux_dcd_stm32_setup_out(STM32::Endpoint * ed, Transfer *transfer_request,
                                           PCD_HandleTypeDef *hpcd)
{
  transfer_request->complete(UX_SUCCESS);
  ed -> direction = UX_ENDPOINT_IN;
  if (ed->ux_slave_endpoint_device->process_control_event(transfer_request) == UX_SUCCESS)
  {
    ed -> state = STM32::EndpointState::STATUS_TX;
    HAL_PCD_EP_Transmit(hpcd, 0x00U, nullptr, 0U);
  }
}

static inline void _ux_dcd_stm32_setup_status(STM32::Endpoint * ed, Transfer *transfer_request,
                                              PCD_HandleTypeDef *hpcd)
{
  ed -> direction = UX_ENDPOINT_IN;

  if (ed->ux_slave_endpoint_device->process_control_event(transfer_request) == UX_SUCCESS)
  {

    /* Set the state to STATUS RX.  */
    ed -> state = STM32::EndpointState::STATUS_RX;
    HAL_PCD_EP_Transmit(hpcd, 0x00U, nullptr, 0U);
  }
}

void STM32::DCD::setup()
{
  auto endpoint = get_control_endpoint();
  Transfer *transfer_request;

  /* Get the pointer to the transfer request.  */
  transfer_request = get_control_transfer();

  /* Copy setup data to transfer request.  */
  ::memcpy(transfer_request->setup, hpcd.Setup, UX_SETUP_SIZE);

  /* Clear the length of the data received.  */
  transfer_request->actual_length =  0;

  /* Mark the phase as SETUP.  */
  transfer_request->type =  TransferType::SETUP;

  /* Mark the transfer as successful.  */
  transfer_request->complete(UX_SUCCESS);

  endpoint->in_transfer = false;
  endpoint->stalled = false;
  endpoint->done = false;


    /* Check if the transaction is IN.  */
    if (*transfer_request -> setup & UX_REQUEST_IN)
    {
      _ux_dcd_stm32_setup_in(endpoint, transfer_request);
    }
    else
    {

        /* The endpoint is OUT.  This is important to memorize the direction for the control endpoint
           in case of a STALL. */
        endpoint -> direction  = UX_ENDPOINT_OUT;

        /* We are in a OUT transaction. Check if there is a data payload. If so, wait for the payload
           to be delivered.  */
        if (*(transfer_request -> setup + 6) == 0 &&
            *(transfer_request -> setup + 7) == 0)
        {
          _ux_dcd_stm32_setup_status(endpoint, transfer_request, &hpcd);
        }
        else
        {
          endpoint = (STM32::Endpoint *)transfer_request -> endpoint;

          /* Get the length we expect from the SETUP packet.  */
          transfer_request -> requested_length = usb_get_short(transfer_request -> setup + 6);

            /* Check if we have enough space for the request.  */
            if (transfer_request -> requested_length > UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH)
            {

                /* No space available, stall the endpoint.  */
              endpoint->stall();

                /* Next phase is a SETUP.  */
                endpoint->state =  EndpointState::IDLE;

                /* We are done.  */
                return;
            }
            else
            {

                /* Reset what we have received so far.  */
                transfer_request -> actual_length =  0;

                /* And reprogram the current buffer address to the beginning of the buffer.  */
                transfer_request -> current_data_pointer =  transfer_request -> data;

                /* Receive data.  */
                HAL_PCD_EP_Receive(&hpcd,
                            endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress,
                            transfer_request -> current_data_pointer,
                            transfer_request -> requested_length);

                /* Set the state to RX.  */
                endpoint->state =  EndpointState::DATA_RX;
            }
        }
    }
}

void STM32::DCD::on_control_in()
{
  Transfer *transfer_request;
  ULONG             transfer_length;
  STM32::Endpoint     *endpoint;

  endpoint = (STM32::Endpoint *)get_endpoint(0);
  transfer_request =  endpoint->get_transfer();

  /* Check if we need to send data again on control endpoint. */
  if (endpoint->state == EndpointState::DATA_TX)
  {
    HAL_PCD_EP_Receive(&hpcd, 0, 0, 0);

    /* Are we done with this transfer ? */
    if (transfer_request -> in_transfer_length <=
        endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize)
    {

        /* There is no data to send but we may need to send a Zero Length Packet.  */
        if (transfer_request -> force_zlp ==  UX_TRUE)
        {

            /* Arm a ZLP packet on IN.  */
            HAL_PCD_EP_Transmit(&hpcd,
                    endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0);

            /* Reset the ZLP condition.  */
            transfer_request -> force_zlp =  UX_FALSE;

        }
        else
        {

            /* Set the completion code to no error.  */
            transfer_request -> completion_code =  UX_SUCCESS;

            /* The transfer is completed.  */
            transfer_request -> status =  TransferStatus::COMPLETED;
            transfer_request -> actual_length =
                transfer_request -> requested_length;

#if defined(UX_DEVICE_STANDALONE)

            /* Control status phase done.  */
            ed -> status |= STM32::Endpoint_STATUS_DONE;
#endif

            /* We are using a Control endpoint, if there is a callback, invoke it. We are still under ISR.  */
            if (transfer_request -> completion_function)
                transfer_request -> completion_function (transfer_request) ;

            /* State is now STATUS RX.  */
            endpoint -> state = EndpointState::STATUS_RX;
        }
    }
    else
    {

        /* Get the size of the transfer.  */
        transfer_length = transfer_request -> in_transfer_length - endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;

        /* Check if the endpoint size is bigger that data requested. */
        if (transfer_length > endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize)
        {

            /* Adjust the transfer size.  */
            transfer_length =  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
        }

        /* Adjust the data pointer.  */
        transfer_request -> current_data_pointer += endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;

        /* Adjust the transfer length remaining.  */
        transfer_request -> in_transfer_length -= transfer_length;

        /* Transmit data.  */
        HAL_PCD_EP_Transmit(&hpcd,
                    endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                    transfer_request->current_data_pointer,
                    transfer_length);
    }
  }
}


void STM32::DCD::on_data_in(uint8_t epnum)
{
  if (epnum == 0) {
    on_control_in();
    return;
  }

  Transfer *xfer;
  STM32::Endpoint     *endpoint;

  endpoint = (STM32::Endpoint *)get_endpoint(epnum | 0x80);

    /* Get the pointer to the transfer request.  */
  xfer = endpoint->get_transfer();

  /* Check if a ZLP should be armed.  */
  if (xfer->force_zlp &&
      xfer->requested_length)
  {
    xfer->force_zlp = UX_FALSE;
    xfer->in_transfer_length = 0;

    /* Arm a ZLP packet on IN.  */
    HAL_PCD_EP_Transmit(&hpcd, epnum, 0, 0);
  }
  else
  {
    xfer -> actual_length =
        xfer -> requested_length;

  /* Non control endpoint operation, use semaphore.  */
    xfer->complete(UX_SUCCESS);
  }
}

void STM32::DCD::on_data_out(uint8_t epnum)
{

STM32::Endpoint         *ed;
Transfer       *xfer;
ULONG                   transfer_length;
Endpoint       *endpoint;


    /* Fetch the address of the physical endpoint.  */
    ed = &ep_out[epnum & 0xF];

    /* Get the pointer to the transfer request.  */
    xfer = ed->get_transfer();

    /* Endpoint 0 is different.  */
    if (epnum == 0U)
    {

        /* Check if we have received something on endpoint 0 during data phase .  */
        if (ed -> state == EndpointState::DATA_RX)
        {

            /* Get the pointer to the logical endpoint from the transfer request.  */
            endpoint = (STM32::Endpoint *)xfer -> endpoint;

            /* Read the received data length for the Control endpoint.  */
            transfer_length = HAL_PCD_EP_GetRxCount(&hpcd, epnum);

            /* Update the length of the data received.  */
            xfer -> actual_length += transfer_length;

            /* Can we accept this much?  */
            if (xfer -> actual_length <=
                xfer -> requested_length)
            {

                /* Are we done with this transfer ? */
                if ((xfer -> actual_length ==
                     xfer -> requested_length) ||
                    (transfer_length != endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize))
                {
                    _ux_dcd_stm32_setup_out(ed, xfer, &hpcd);
                }
                else
                {

                    /* Rearm the OUT control endpoint for one packet. */
                    xfer -> current_data_pointer += endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
                    HAL_PCD_EP_Receive(&hpcd,
                                endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress,
                                xfer -> current_data_pointer,
                                endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize);
                }
            }
            else
            {

                /*  We have an overflow situation. Set the completion code to overflow.  */
                xfer -> completion_code =  UX_TRANSFER_BUFFER_OVERFLOW;

                /* If trace is enabled, insert this event into the trace buffer.  */
                UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_TRANSFER_BUFFER_OVERFLOW, xfer, 0, 0, UX_TRACE_ERRORS, 0, 0)

#if defined(UX_DEVICE_STANDALONE)

                /* Control status phase done.  */
                ed -> status |= STM32::Endpoint_STATUS_DONE;
#endif

                /* We are using a Control endpoint, if there is a callback, invoke it. We are still under ISR.  */
                if (xfer -> completion_function)
                    xfer -> completion_function (xfer) ;
            }
        }
    }
    else
    {


        /* Update the length of the data sent in previous transaction.  */
        xfer -> actual_length =  HAL_PCD_EP_GetRxCount(&hpcd, epnum);

        xfer->complete(UX_SUCCESS);
    }

}

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
