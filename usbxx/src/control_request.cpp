#include <threadxx/intr.hpp>

#include <usbxx/ux_api.h>

#include <usbxx/device.hpp>

#include <usbxx/event_log.hpp>

using namespace USBXX;

#if 0
uint8_t _ux_system_slave_class_storage_name[] =                               "storage";
uint8_t _ux_system_slave_class_dpump_name[] =                                 "dpump";
uint8_t _ux_system_slave_class_pima_name[] =                                  "pima";
uint8_t _ux_system_slave_class_hid_name[] =                                   "hid";
uint8_t _ux_system_slave_class_rndis_name[] =                                 "rndis";
uint8_t _ux_system_slave_class_cdc_ecm_name[] =                               "cdc_ecm";
uint8_t _ux_system_slave_class_dfu_name[] =                                   "dfu";
uint8_t _ux_system_slave_class_audio_name[] =                                 "audio";

uint8_t _ux_system_device_class_printer_name[] =                              "ux_device_class_printer";
uint8_t _ux_system_device_class_ccid_name[] =                                 "ux_device_class_ccid";
uint8_t _ux_system_device_class_video_name[] =                                "ux_device_class_video";
#endif

void DeviceBase::handle_control_request(const ControlRequest &req)
{
  uint32_t status;

  /* Check if there is a vendor registered function at the application layer.  If the request
     is VENDOR and the request match, pass the request to the application.  */
  if (req.type == RequestType::VENDOR)
  {
    auto xfer = get_control_transfer();
    uint32_t application_data_length = xfer->buffer_size;

    if (on_vendor_request(req, xfer->data, &application_data_length))
    {
      get_control_endpoint()->stall();
      return;
    }

    xfer -> phase =  TransferPhase::DATA_OUT;

    transfer_request(xfer, application_data_length, req.length);

    return;
  }

  if ((req.type == RequestType::CLASS) ||
      (req.type == RequestType::VENDOR))
  {
    /* We need to find which class this request is for.  */
    for (auto iface : interfaces)
    {
#if 0  /* Printer crap */
      if ((req.type == 0xA1) && (req.code == 0x00) &&
          (class_ptr -> interface -> descriptor.bInterfaceClass == 0x07))
      {
        if(*(get_control_transfer() -> setup + UX_SETUP_INDEX + 1) != class_index)
          continue;
      }
      //else
#endif

      if ((req.index & 0xFF) != iface->descriptor.bInterfaceNumber)
        continue;

      status = /*class_ptr ->*/ this->class_command_request(req);

      /* The status simply tells us if the registered class handled the
           command - if there was an issue processing the command, it would've
           stalled the control endpoint, notifying the host (and not us).  */
      if (status == UX_SUCCESS)
        break;
    }

    goto exit;
  }

  switch (req.code)
  {
  case StdControlRequest::GET_STATUS:
    status = get_entity_status(req);
    break;

  case StdControlRequest::CLEAR_FEATURE:
    status = clear_feature(req);
    break;

  case StdControlRequest::SET_FEATURE:
    status = set_feature(req);
    break;

  case StdControlRequest::SET_ADDRESS:
    status = UX_SUCCESS;
    dcd->set_device_address(req.value);
    break;

  case StdControlRequest::GET_DESCRIPTOR:
    status = send_descriptor(req); //request_value, request_index, request_length);
    break;

  case StdControlRequest::SET_DESCRIPTOR:
    status = UX_FUNCTION_NOT_SUPPORTED;
    break;

  case StdControlRequest::GET_CONFIGURATION:
    status = on_get_configuration();
    break;

  case StdControlRequest::SET_CONFIGURATION:
    event_log.push_event(UsbEvent::SET_CONFIGURATION_START);
    status = on_set_configuration(req.value);
    event_log.push_event(UsbEvent::SET_CONFIGURATION_END);
    if (req.value != 0)
      int a = 0;
    break;

  case StdControlRequest::GET_INTERFACE:
    status = on_get_alternate_setting(req.index);
    break;

  case StdControlRequest::SET_INTERFACE:
    status = on_set_alternate_setting(req.index, req.value);
    break;

  case StdControlRequest::SYNCH_FRAME:
    status = UX_SUCCESS;
    break;

  default:
    status = UX_FUNCTION_NOT_SUPPORTED;
    break;
  }

exit:
  if (status == UX_SUCCESS) {
    get_control_endpoint()->ack_ctrl();
  } else {
    get_control_endpoint()->stall();
  }
}

void DeviceBase::control_thread_main()
{
  while (true) {
    Transfer *xfer;

    control_request_sema.get();

    {
      TXX::lock_intr l;

#if 0
      xfer = control_requests.front();
      control_requests.pop_front();
#else
      xfer = control_xfer;
      control_xfer = nullptr;
#endif

      if (!xfer->is_valid())
        continue;


      ControlRequest req(xfer->setup);

      event_log.push_event(UsbEvent::START_CONTROL_REQUEST, req.code);

      /* Filter for GET_DESCRIPTOR/SET_DESCRIPTOR commands. If the descriptor to be returned is not a standard descriptor,
         treat the command as a CLASS command.  */
      if ((req.code == StdControlRequest::GET_DESCRIPTOR ||
          req.code == StdControlRequest::SET_DESCRIPTOR) &&
          (((req.value >> 8) & ControlRequestType::MASK) != ControlRequestType::STANDARD))
        req.type = RequestType::CLASS;


      handle_control_request(req);

      event_log.push_event(UsbEvent::END_CONTROL_REQUEST);
    }
  }
}

void DeviceBase::process_control_event(Transfer *xfer)
{
  event_log.push_event(UsbEvent::RX_CONTROL_REQUEST);

  {
    TXX::lock_intr l;

#if 0
    control_requests.push_back(xfer);
#else
    assert(control_xfer == nullptr);
    control_xfer = xfer;
#endif
  }

  control_request_sema.put();
}

uint32_t DeviceBase::get_entity_status(const ControlRequest &req)
{
Transfer       *xfer;
uint32_t                    status;
uint32_t                   data_length;

    /* Get the control endpoint for the device.  */
    auto endpoint = get_control_endpoint();

    /* Get the pointer to the transfer request associated with the endpoint.  */
    xfer = get_control_transfer();

    /* Reset the status buffer.  */
    *xfer -> data =  0;
    *(xfer -> data + 1) =  0;

    /* The default length for GET_STATUS is 2, except for OTG get Status.  */
    data_length = 2;

    /* The status can be for either the device or the endpoint.  */
    switch (req.recipient)
    {

    case RequestRecipient::DEVICE:

        /* When the device is probed, it is either for the power/remote capabilities or OTG role swap.
           We differentiate with the Windex, 0 or OTG status Selector.  */
        if (req.index == UX_OTG_STATUS_SELECTOR)
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

    case RequestRecipient::ENDPOINT:
    {
      auto tgt = dcd->get_endpoint(req.index);

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
    status = transfer_request(xfer, data_length, data_length);

    /* Return the function status.  */
    return(status);
}
