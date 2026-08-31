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

#include <usbxx/endian.hpp>

#include <usbxx/device.hpp>

using namespace USBXX;


#if (UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH < UX_DEVICE_DESCRIPTOR_LENGTH) || \
    (UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH < UX_DEVICE_QUALIFIER_DESCRIPTOR_LENGTH) || \
    (UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH < UX_OTG_DESCRIPTOR_LENGTH)
/* #error UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH too small, please check  */
/* Build option checked runtime by UX_ASSERT  */
#endif

#include <usbxx/stm32/dcd.hpp>

using namespace USBXX;

UINT  _ux_device_stack_descriptor_send(ULONG descriptor_type, ULONG request_index, ULONG host_length)
{
  USBXX::DCD                    *dcd;
  ULONG                           descriptor_index;
  ULONG                           parsed_descriptor_index;
  UX_SLAVE_TRANSFER               *transfer_request;
  ConfigurationDescriptor     configuration_descriptor;
  BOSDescriptor               bos_descriptor;
  Endpoint               *endpoint;
  UCHAR                           *device_framework;
  UCHAR                           *device_framework_end;
  ULONG                           device_framework_length;
  ULONG                           descriptor_length;
  ULONG                           target_descriptor_length = 0;
  UINT                            status =  UX_ERROR;
  ULONG                           length;
  UCHAR                           *string_memory;
  UCHAR                           *string_framework;
  ULONG                           string_framework_length;
  ULONG                           string_length;

    /* Get the pointer to the DCD.  */
    dcd = STM32::gDCD;

    /* Get the pointer to the device.  */
    auto device = _ux_system_slave->device;

    /* Get the control endpoint associated with the device.  */
    endpoint = device->get_control_endpoint();

    /* Get the pointer to the transfer request associated with the endpoint.  */
    transfer_request =  &endpoint -> ux_slave_endpoint_transfer_request;

    /* Set the direction to OUT.  */
    transfer_request -> ux_slave_transfer_request_phase =  UX_TRANSFER_PHASE_DATA_OUT;

    /* Isolate the descriptor index.  */
    descriptor_index =  descriptor_type & 0xff;

    /* Reset the parsed index.  */
    parsed_descriptor_index =  0;

    /* Shift the descriptor type in the low byte field.  */
    descriptor_type =  (UCHAR) ((descriptor_type >> 8) & 0xff);

    /* Default descriptor length is host length.  */
    length =  host_length;

    /* What type of descriptor do we need to return?  */
    switch (descriptor_type)
    {

    case DeviceDescriptor::desc_type:

		/* Setup device descriptor length.  */
        if (host_length > UX_DEVICE_DESCRIPTOR_LENGTH)
            length =  UX_DEVICE_DESCRIPTOR_LENGTH;

        /* Fall through.  */
    case UX_DEVICE_QUALIFIER_DESCRIPTOR_ITEM:

        /* Setup qualifier descriptor length.  */
        if (descriptor_type == UX_DEVICE_QUALIFIER_DESCRIPTOR_ITEM &&
            host_length > UX_DEVICE_QUALIFIER_DESCRIPTOR_LENGTH)
            length =  UX_DEVICE_QUALIFIER_DESCRIPTOR_LENGTH;

        /* Fall through.  */
    case UX_OTG_DESCRIPTOR_ITEM:

        /* Setup OTG descriptor length.  */
        if (descriptor_type == UX_OTG_DESCRIPTOR_ITEM &&
            host_length > UX_OTG_DESCRIPTOR_LENGTH)
            length =  UX_OTG_DESCRIPTOR_LENGTH;

        /* We may or may not have a device qualifier descriptor.  */
        device_framework =  _ux_system_slave -> ux_system_slave_device_framework;
        device_framework_length =  _ux_system_slave -> ux_system_slave_device_framework_length;
        device_framework_end = device_framework + device_framework_length;

        /* Parse the device framework and locate a device qualifier descriptor.  */
        while (device_framework < device_framework_end)
        {

            /* Get descriptor length.  */
            descriptor_length =  (ULONG) *device_framework;

            /* Check if this is a descriptor expected.  */
            if (*(device_framework + 1) == descriptor_type)
            {
                ::memcpy(transfer_request->ux_slave_transfer_request_data_pointer,
                    device_framework, length);

                /* Perform the data transfer.  */
                status =  _ux_device_stack_transfer_request(transfer_request, length, host_length);
                break;
            }

            /* Adjust what is left of the device framework.  */
            device_framework_length -=  descriptor_length;

            /* Point to the next descriptor.  */
            device_framework +=  descriptor_length;
        }
        break;

#ifndef UX_BOS_SUPPORT_DISABLE
    case UX_BOS_DESCRIPTOR_ITEM:
        /* Fall through.  */
#endif
    case UX_OTHER_SPEED_DESCRIPTOR_ITEM:
        /* Fall through.  */
    case UX_CONFIGURATION_DESCRIPTOR_ITEM:

        if (descriptor_type == UX_OTHER_SPEED_DESCRIPTOR_ITEM)
        {

            /* This request is used by the host to find out the capability of this device
            if it was running at full speed. The behavior is the same as in a GET_CONFIGURATIOn descriptor
            but we do not use the current device framework but rather the full speed framework. */
            device_framework =  _ux_system_slave -> ux_system_slave_device_framework_full_speed;
            device_framework_length =  _ux_system_slave -> ux_system_slave_device_framework_length_full_speed;
            device_framework_end = device_framework + device_framework_length;
        }
        else
        {

            /* We may have multiple configurations !, the index will tell us what
            configuration descriptor we need to return.  */
            device_framework =  _ux_system_slave -> ux_system_slave_device_framework;
            device_framework_length =  _ux_system_slave -> ux_system_slave_device_framework_length;
            device_framework_end = device_framework + device_framework_length;
        }

        /* Parse the device framework and locate a configuration descriptor.  */
        while (device_framework < device_framework_end)
        {

            /* Get descriptor length. */
            descriptor_length =  (ULONG) *device_framework;

            /* Check if we are finding BOS descriptor.  */
            if (descriptor_type == UX_BOS_DESCRIPTOR_ITEM)
            {
                if (*(device_framework + 1) == UX_BOS_DESCRIPTOR_ITEM)
                {
                  bos_descriptor = read_in_descriptor<BOSDescriptor>(device_framework);

                  /* Get the length of entire BOS descriptor.  */
                  target_descriptor_length = bos_descriptor.wTotalLength;

                  /* Descriptor is found.  */
                  status = UX_SUCCESS;
                  break;
                }
            }
            else
            {

                /* Check if this is a configuration descriptor.  We are cheating here. Instead of creating
                a OTHER SPEED descriptor, we simply scan the configuration descriptor for the Full Speed
                framework and return this configuration after we manually changed the configuration descriptor
                item into a Other Speed Descriptor. */
                if (*(device_framework + 1) == UX_CONFIGURATION_DESCRIPTOR_ITEM)
                {

                    /* Check the index. It must be the same as the one requested.  */
                    if (parsed_descriptor_index == descriptor_index)
                    {
                      configuration_descriptor = read_in_descriptor<ConfigurationDescriptor>(device_framework);

                        /* Get the length of entire configuration descriptor.  */
                        target_descriptor_length = configuration_descriptor.wTotalLength;

                        /* Descriptor is found.  */
                        status = UX_SUCCESS;
                        break;
                    }
                    else
                    {

                        /* There may be more configuration descriptors in this framework.  */
                        parsed_descriptor_index++;
                    }
                }
            }

            /* Adjust what is left of the device framework.  */
            device_framework_length -=  descriptor_length;

            /* Point to the next descriptor.  */
            device_framework +=  descriptor_length;
        }

        /* Send the descriptor.  */
        if (status == UX_SUCCESS)
        {

            /* Ensure the host does not demand a length beyond our descriptor (Windows does that)
                and do not return more than what is allowed.  */
            if (target_descriptor_length < host_length)
                length =  target_descriptor_length;
            else
                length =  host_length;

            /* Check buffer length, since total descriptors length may exceed buffer...  */
            if (length > UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH)
            {
                status =  dcd ->stall(endpoint);
                break;
            }

            ::memcpy(transfer_request -> ux_slave_transfer_request_data_pointer,
                device_framework, length);

            /* Now we need to hack the found descriptor because this request expect a requested
                descriptor type instead of the regular descriptor.  */
            *(transfer_request -> ux_slave_transfer_request_data_pointer + 1) = (UCHAR)descriptor_type;

            /* We can return the configuration descriptor.  */
            status =  _ux_device_stack_transfer_request(transfer_request, length, host_length);
        }
        break;

    case UX_STRING_DESCRIPTOR_ITEM:

        /* We need to filter for the index 0 which is the language ID string.  */
        if (descriptor_index == 0)
        {

            /* We need to check request buffer size in case it's possible exceed. */
            if (_ux_system_slave -> ux_system_slave_language_id_framework_length + 2 > UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH)
            {
                status = dcd->stall(endpoint);
                break;
            }

            /* We have a request to send back the language ID list. Use the transfer request buffer.  */
            string_memory =  transfer_request -> ux_slave_transfer_request_data_pointer;

            /* Store the total length of the response.  */
            *string_memory =  (UCHAR)(_ux_system_slave -> ux_system_slave_language_id_framework_length + 2);

            /* Store the descriptor type.  */
            *(string_memory +1) =  UX_STRING_DESCRIPTOR_ITEM;

            /* Store the language ID into the buffer.  */
            ::memcpy(string_memory+2, _ux_system_slave -> ux_system_slave_language_id_framework,
                                                        _ux_system_slave -> ux_system_slave_language_id_framework_length); /* Use case of memcpy is verified. */

            /* Filter the length asked/required.  */
            if (host_length > _ux_system_slave -> ux_system_slave_language_id_framework_length + 2)
                length =  _ux_system_slave -> ux_system_slave_language_id_framework_length + 2;
            else
                length =  host_length;

            /* We can return the string language ID descriptor.  */
            status =  _ux_device_stack_transfer_request(transfer_request, length, host_length);
        }
        else
        {
            /* The host wants a specific string index returned. Get the string framework pointer
               and length.  */
            string_framework =  _ux_system_slave -> ux_system_slave_string_framework;
            string_framework_length =  _ux_system_slave -> ux_system_slave_string_framework_length;

            /* We search through the string framework until we find the right index.
               The index is in the lower byte of the descriptor type. */
            while (string_framework_length != 0)
            {

                /* Ensure we have the correct language page.  */
                if (usb_get_short(string_framework) == request_index)
                {

                    /* Check the index.  */
                    if (*(string_framework + 2) == descriptor_index)
                    {

                        /* We need to check request buffer size in case it's possible exceed. */
                        if (((*(string_framework + 3)*2) + 2) > UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH)
                        {

                            /* Error trap. */
                            _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_DEVICE_STACK, UX_MEMORY_INSUFFICIENT);

                            /* If trace is enabled, insert this event into the trace buffer.  */
                            UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_MEMORY_INSUFFICIENT, device, 0, 0, UX_TRACE_ERRORS, 0, 0)

                            /* Stall the endpoint.  */
                            status =  dcd->stall(endpoint);
                            break;
                        }

                        /* We have a request to send back a string. Use the transfer request buffer.  */
                        string_memory =  transfer_request -> ux_slave_transfer_request_data_pointer;

                        /* Store the length in the string buffer. The length
                           of the string descriptor is stored in the third byte,
                           hence the ' + 3'. The encoding must be in 16-bit
                           unicode, hence the '*2'. The length includes the size
                           of the length itself as well as the descriptor type,
                           hence the ' + 2'.  */
                        *string_memory =  (UCHAR)((*(string_framework + 3)*2) + 2);

                        /* Store the Descriptor type. */
                        *(string_memory + 1) =  UX_STRING_DESCRIPTOR_ITEM;

                        /* Create the Unicode string.  */
                        for (string_length = 0; string_length <  *(string_framework + 3) ; string_length ++)
                        {

                            /* Insert a Unicode byte.  */
                            *(string_memory + 2 + (string_length * 2)) =  *(string_framework + 4 + string_length);

                            /* Insert a zero after the Unicode byte.  */
                            *(string_memory + 2 + (string_length * 2) + 1) =  0;
                        }

                        /* Filter the length asked/required.  */
                        if (host_length > (UINT)((*(string_framework + 3)*2) + 2))
                            length =  (ULONG)((*(string_framework + 3)*2) + 2);
                        else
                            length =  host_length;

                        /* We can return the string descriptor.  */
                        status =  _ux_device_stack_transfer_request(transfer_request, length, host_length);
                        break;
                    }
                }

                /* This is the wrong string descriptor, jump to the next.  */
                string_framework_length -=  (ULONG) *(string_framework + 3) + 4;
                string_framework +=  (ULONG) *(string_framework + 3) + 4;
            }

            /* Have we exhausted all the string descriptors?  */
            if (string_framework_length == 0)
            {

                /* Could not find the required string index. Stall the endpoint.  */
                dcd->stall(endpoint);
                return(UX_ERROR);
            }
        }
        break;

    default:
      // TODO -- SEND CDCACM descriptor

      dcd->stall(endpoint);
      return UX_ERROR;
        throw std::runtime_error("Invalid descriptor in send descriptor");
    }

    /* Return the status to the caller.  */
    return(status);
}

