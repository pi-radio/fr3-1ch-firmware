#include <usbxx/ux_api.h>
#include <usbxx/ux_device_stack.h>

#include <usbxx/device.hpp>

using namespace USBXX;

#if 0
UCHAR _ux_system_slave_class_storage_name[] =                               "ux_slave_class_storage";
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
#endif


uint32_t DeviceBase::process_control_event(Transfer *xfer)
{

USBXX::DCD                *dcd;
UX_SLAVE_CLASS              *class_ptr;
UX_SLAVE_CLASS_COMMAND      class_command;
ULONG                       request_type;
ULONG                       request;
ULONG                       request_value;
ULONG                       request_index;
ULONG                       request_length;
ULONG                       class_index;
UINT                        status =  UX_ERROR;
ULONG                       application_data_length;


    /* Get the pointer to the device.  */
    auto device = _ux_system_slave->device;

    /* Get the pointer to the DCD.  */
    dcd = device->get_dcd();


    /* Ensure that the Setup request has been received correctly.  */
    if (xfer->is_valid())
    {

        /* Seems so far, the Setup request is valid. Extract all fields of
           the request.  */
        request_type   =   *xfer -> setup;
        request        =   *(xfer -> setup + UX_SETUP_REQUEST);
        request_value  =   usb_get_short(xfer -> setup + UX_SETUP_VALUE);
        request_index  =   usb_get_short(xfer -> setup + UX_SETUP_INDEX);
        request_length =   usb_get_short(xfer -> setup + UX_SETUP_LENGTH);

        /* Filter for GET_DESCRIPTOR/SET_DESCRIPTOR commands. If the descriptor to be returned is not a standard descriptor,
           treat the command as a CLASS command.  */
        if ((request == UX_GET_DESCRIPTOR || request == UX_SET_DESCRIPTOR) && (((request_value >> 8) & UX_REQUEST_TYPE) != UX_REQUEST_TYPE_STANDARD))
        {

            /* This request is to be handled by the class layer.  */
            request_type &=  (UINT)~UX_REQUEST_TYPE;
            request_type |= UX_REQUEST_TYPE_CLASS;
        }

        /* Check if there is a vendor registered function at the application layer.  If the request
           is VENDOR and the request match, pass the request to the application.  */
        if ((request_type & UX_REQUEST_TYPE) == UX_REQUEST_TYPE_VENDOR)
        {                /* This is a Microsoft extended function. It happens before the device is configured.
                   The request is passed to the application directly.  */
                application_data_length = UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH;
                status = device->on_vendor_request(request, request_value,
                                                                                            request_index, request_length,
                                                                                            xfer -> data,
                                                                                            &application_data_length);

                /* Check the status from the application.  */
                if (status == UX_SUCCESS)
                {
                    /* Get the pointer to the transfer request associated with the control endpoint.  */
                    auto xfer2 = device->get_control_transfer();

                    /* Set the direction to OUT.  */
                    xfer2 -> phase =  TransferPhase::DATA_OUT;

                    /* Perform the data transfer.  */
                    transfer_request(xfer2, application_data_length, request_length);

                    /* We are done here.  */
                    return 0;
                }
                else
                {

                    /* The application did not like the vendor command format, stall the control endpoint.  */
                    device -> get_control_endpoint()->stall();

                    /* We are done here.  */
                    return 0;
                }
        }

        /* Check the destination of the request. If the request is of type CLASS or VENDOR_SPECIFIC,
           the function has to be passed to the class layer.  */
        if (((request_type & UX_REQUEST_TYPE) == UX_REQUEST_TYPE_CLASS) ||
            ((request_type & UX_REQUEST_TYPE) == UX_REQUEST_TYPE_VENDOR))
        {

            /* Build all the fields of the Class Command.  */
            class_command.ux_slave_class_command_request =  UX_SLAVE_CLASS_COMMAND_REQUEST;

            /* We need to find which class this request is for.  */
            for (class_index = 0; class_index < UX_MAX_SLAVE_INTERFACES; class_index ++)
            {

                /* Get the class for the interface.  */
                class_ptr =  _ux_system_slave -> ux_system_slave_interface_class_array[class_index];

                /* If class is not ready, try next.  */
                if (class_ptr == nullptr)
                    continue;

                /* Is the request target to an interface?  */
                if ((request_type & UX_REQUEST_TARGET) == UX_REQUEST_TARGET_INTERFACE)
                {

                    /* Yes, so the request index contains the index of the interface
                       the request is for. So if the current index does not match
                       the request index, we should go to the next one.  */
                    /* For printer class (0x07) GET_DEVICE_ID (0x00) the high byte of
                       wIndex is interface index (for recommended index sequence the interface
                       number is same as interface index inside configuration).
                     */
                    if ((request_type == 0xA1) && (request == 0x00) &&
                        (class_ptr -> ux_slave_class_interface -> descriptor.bInterfaceClass == 0x07))
                    {

                        /* Check wIndex high byte.  */
                        if(*(xfer -> setup + UX_SETUP_INDEX + 1) != class_index)
                            continue;
                    }
                    else
                    {

                        /* Check wIndex low.  */
                        if ((request_index & 0xFF) != class_index)
                            continue;
                    }
                }

                /* Memorize the class in the command.  */
                class_command.ux_slave_class_command_class_ptr = class_ptr;

                /* We have found a potential candidate. Call this registered class entry function.  */
                status = class_ptr -> ux_slave_class_entry_function(&class_command);

                /* The status simply tells us if the registered class handled the
                   command - if there was an issue processing the command, it would've
                   stalled the control endpoint, notifying the host (and not us).  */
                if (status == UX_SUCCESS)

                    /* We are done, break the loop!  */
                    break;

                /* Not handled, try next.  */
            }

            /* If no class handled the command, then we have an error here.  */
            if (status != UX_SUCCESS)

                /* We stall the command (request not supported).  */
                device->get_control_endpoint()->stall();

            /* We are done for class/vendor request.  */
            return(status);
        }

        /* At this point, the request must be a standard request that the device stack should handle.  */
        switch (request)
        {

        case UX_GET_STATUS:

            status = get_entity_status(request_type, request_index, request_length);
            break;

        case UX_CLEAR_FEATURE:

            status =  clear_feature(request_type, request_value, request_index);
            break;

        case UX_SET_FEATURE:

            status =  set_feature(request_type, request_value, request_index);
            break;

        case UX_SET_ADDRESS:

            /* Memorize the address. Some controllers memorize the address here. Some don't.  */


            /* Force the new address.  */
            dcd->set_device_address(request_value);

            status = 0;

            break;

        case UX_GET_DESCRIPTOR:
        {
          ControlRequest req(xfer->setup);

          status = send_descriptor(req); //request_value, request_index, request_length);
          break;
        }

        case UX_SET_DESCRIPTOR:

            status = UX_FUNCTION_NOT_SUPPORTED;
            break;

        case UX_GET_CONFIGURATION:

            status = device->on_get_configuration();
            break;

        case UX_SET_CONFIGURATION:

            status =  device->on_set_configuration(request_value);
            break;

        case UX_GET_INTERFACE:

            status = device->on_get_alternate_setting(request_index);
            break;

        case UX_SET_INTERFACE:

            status =  device->on_set_alternate_setting(request_index,request_value);
            break;


        case UX_SYNCH_FRAME:

            status = UX_SUCCESS;
            break;

        default :

            status = UX_FUNCTION_NOT_SUPPORTED;
            break;
        }

        if (status != UX_SUCCESS)

            /* Stall the control endpoint to issue protocol error. */
           device->get_control_endpoint()->stall();
    }

    /* Return the function status.  */
    return(status);
}

uint32_t DeviceBase::get_entity_status(uint32_t request_type, uint32_t request_index, uint32_t request_length)
{
Transfer       *xfer;
UINT                    status;
ULONG                   data_length;

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
      auto tgt = device->dcd->get_endpoint(request_index);

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
