/*
 * usbxxcdcacm.cpp
 *
 *  Created on: Feb 21, 2026
 *      Author: zapman
 */

#include <threadxx/dbgstream.hpp>

#include <ux_api.h>

#include <usbxx/cdcacm.hpp>

using namespace USBXX;

CDCACM *CDCACM::stupid_global = NULL;



CDCACM::CDCACM() : tx_queue("CDC ACM TX Queue"),
                   tx_thread("CDCACM TX Thread", this, &CDCACM::_tx_thread),
                   rx_mutex("CDCACM RX Mutex"),
                   tx_mutex("CDCACM TX Mutex"),
                   ep_out_mutex("CDCACM EP Out Mutex"),
                   ep_in_mutex("CDCACM EP In Mutex")
{
  add_class(USBXX::CLASS_TYPE_CDC_ACM);

  tx_event_flags_create(&flags, "CDCACM flags");
  
  if (stupid_global != NULL) {
    dbgprint("Multiple CDCACM instances");
  };
  
  stupid_global = this;
}

void CDCACM::wait_started()
{
  ULONG actual;

  tx_event_flags_get(&flags, FLAG_STARTED, TX_AND, &actual, TX_WAIT_FOREVER);
}


bool CDCACM::get_dtr()
{
  return dtr_state;
}


void CDCACM::set_dtr(bool dtr)
{
  if (dtr) {
    tx_event_flags_set(&flags, ~FLAG_DTR, TX_AND);
  } else {
    tx_event_flags_set(&flags, FLAG_DTR, TX_OR);
  }
}

void CDCACM::set_rts(bool rts)
{
  if (rts) {
    tx_event_flags_set(&flags, ~FLAG_RTS, TX_AND);
  } else {
    tx_event_flags_set(&flags, FLAG_RTS, TX_OR);
  }
}


void CDCACM::class_init()
{
  tx_semaphore_create(&flush_sema, (char *)"Terminal Flush Semaphore", 0);
  tx_queue.create();
  tx_thread.create();
}

void CDCACM::flush()
{
  putc(FLUSH);
  tx_semaphore_get(&flush_sema, TX_WAIT_FOREVER);
}

int CDCACM::getc()
{
  ULONG status;

  wait_started();
  
  TXX::Mutex::guard g(rx_mutex);
  
  // Refill buffer
  while (rx_cur >= rx_len) {
    status = read(rx_buf, 64, &rx_len);
    
    while(status == UX_TRANSFER_BUS_RESET) {      
      tx_thread_sleep(10);

      status = read(rx_buf, 64, &rx_len);
    } 

    if (status != TX_SUCCESS) {
      printf("Failed read status: %ld\n", status);
      tx_thread_sleep(100);
      continue;
    }

    rx_cur = 0;
  };

  return rx_buf[rx_cur++];
}

void CDCACM::putc(int c)
{
  tx_queue.send(c);
}

void CDCACM::flush_buffer()
{
  TXX::Mutex::guard g(tx_mutex);
  
  while (!tx_buf.empty()) {
    auto res = tx_buf.get_seg();
    UCHAR *p = (UCHAR *)res.first;
    ULONG l = res.second;
    ULONG result;
    
    while (l) {
      uint32_t tx_len;

      result = write(p, l, &tx_len);
      
      while (result != TX_SUCCESS) {
        tx_thread_sleep(10);
        result = write(p, l, &tx_len);
      }
      
      l -= tx_len;
      p += tx_len;
    }
  }  
}

void CDCACM::_tx_thread()
{
  ULONG c;
  ULONG wait;

  wait_started();

  dbgprint("tx usb started\n");

  wait = TX_WAIT_FOREVER;

  while(1) {
    try {
      c = tx_queue.recv_wait(wait);
    } catch(TXX::QueueEmpty e) {
      flush_buffer();
      wait = TX_WAIT_FOREVER;
      continue;
    }
    
    wait = 1;

    if (c == FLUSH) {
      flush_buffer();
      tx_semaphore_put(&flush_sema);
      wait = TX_WAIT_FOREVER;
      continue;
    }

    if (tx_buf.full()) {
      flush_buffer();
    }

    tx_buf.pushc(c);

    tx_count++;
  }
}

#include <usbxx/descriptor.hpp>
#include <usb.h>
#include <ux_device_descriptors.h>

void USBXX::CDCACM::register_class()
{
  cdc_acm_configuration_number = get_configuration_number(CLASS_TYPE_CDC_ACM, 0);

  cdc_acm_interface_number = get_interface_number(CLASS_TYPE_CDC_ACM, 0);

  /* Initialize the device cdc acm class */
  if (ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name,
                                     _device_entry,
                                     cdc_acm_configuration_number,
                                     cdc_acm_interface_number,
                                     NULL) != UX_SUCCESS)
  {
    throw std::runtime_error("Failed to register CDC ACM class");
  }

}

UINT USBXX::CDCACM::device_entry(UX_SLAVE_CLASS_COMMAND *command)
{
  /* The command request will tell us we need to do here, either a enumeration
     query, an activation or a deactivation.  */
  switch (command->ux_slave_class_command_request)
  {
  case UX_SLAVE_CLASS_COMMAND_INITIALIZE:
    return acm_initialize(command);

  case UX_SLAVE_CLASS_COMMAND_UNINITIALIZE:
    return acm_uninitialize(command);

  case UX_SLAVE_CLASS_COMMAND_QUERY:
    if (command -> ux_slave_class_command_class == UX_SLAVE_CLASS_CDC_ACM_CLASS)
        return(UX_SUCCESS);
    else
        return(UX_NO_CLASS_MATCH);

  case UX_SLAVE_CLASS_COMMAND_ACTIVATE:
    return activate(command);

  case UX_SLAVE_CLASS_COMMAND_DEACTIVATE:
    return deactivate(command);

  case UX_SLAVE_CLASS_COMMAND_REQUEST:
    return control_request(command);

  default:
    return(UX_FUNCTION_NOT_SUPPORTED);
  }
}

UINT USBXX::CDCACM::_device_entry(UX_SLAVE_CLASS_COMMAND *command)
{
  return stupid_global->device_entry(command);
}

UINT USBXX::CDCACM::acm_initialize(UX_SLAVE_CLASS_COMMAND *command)
{
  UX_SLAVE_CLASS *class_ptr;
  UINT status;

  class_ptr = command->ux_slave_class_command_class_ptr;

  class_ptr->ux_slave_class_instance = this;

  /* Update the line coding fields with default values.  */
  baudrate  =  UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_BAUDRATE;
  stop_bit  =  UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_STOP_BIT;
  parity    =  UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARITY;
  data_bit  =  UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_DATA_BIT;

  return UX_SUCCESS;
}
UINT USBXX::CDCACM::acm_uninitialize(UX_SLAVE_CLASS_COMMAND *command)
{
  return UX_SUCCESS;
}


UINT USBXX::CDCACM::activate(UX_SLAVE_CLASS_COMMAND *command)
{
    UX_SLAVE_INTERFACE *interface_ptr;

    /* Get the interface that owns this instance.  */
    interface_ptr =  (UX_SLAVE_INTERFACE  *) command -> ux_slave_class_command_interface;

    /* Store the class instance into the interface.  */
    interface_ptr -> ux_slave_interface_class_instance =  (VOID *)this;

    /* Now the opposite, store the interface in the class instance.  */
    cdc_acm_interface = interface_ptr;

    tx_event_flags_set(&flags, FLAG_STARTED, TX_OR);

    return UX_SUCCESS;
}

extern "C" UINT  _ux_device_stack_transfer_abort(UX_SLAVE_TRANSFER *transfer_request, ULONG completion_code);
extern "C" UINT  _ux_device_stack_transfer_all_request_abort(UX_SLAVE_ENDPOINT *endpoint, ULONG completion_code);

UINT USBXX::CDCACM::deactivate(UX_SLAVE_CLASS_COMMAND *command)
{

  UX_SLAVE_INTERFACE          *interface_ptr;
  //UX_SLAVE_CLASS              *class_ptr;
  UX_SLAVE_ENDPOINT           *endpoint_in;
  UX_SLAVE_ENDPOINT           *endpoint_out;

  interface_ptr =  cdc_acm_interface;

  /* Locate the endpoints.  */
  endpoint_in = interface_ptr->ux_slave_interface_first_endpoint;

  /* Check the endpoint direction, if IN we have the correct endpoint.  */
  if ((endpoint_in->ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) != UX_ENDPOINT_IN)
  {
      endpoint_out =  endpoint_in;
      endpoint_in =  endpoint_out->ux_slave_endpoint_next_endpoint;
  }
  else
  {
      endpoint_out =  endpoint_in->ux_slave_endpoint_next_endpoint;
  }

  /* Terminate the transactions pending on the endpoints.  */
  _ux_device_stack_transfer_all_request_abort(endpoint_in, UX_TRANSFER_BUS_RESET);
  _ux_device_stack_transfer_all_request_abort(endpoint_out, UX_TRANSFER_BUS_RESET);

  /* Terminate transmission and free resources.  */
  ioctl(UX_SLAVE_CLASS_CDC_ACM_IOCTL_TRANSMISSION_STOP, UX_NULL);

  /* We need to reset the DTR and RTS values so they do not carry over to the
     next connection.  */
  dtr_state = 0;
  rts_state = 0;

  return UX_SUCCESS;
}

UINT USBXX::CDCACM::control_request(UX_SLAVE_CLASS_COMMAND *command)
{
  //UX_SLAVE_CLASS                          *class_ptr;
  UX_SLAVE_TRANSFER                       *transfer_request;
  UX_SLAVE_DEVICE                         *device;
  ULONG                                   request;
  ULONG                                   value;
  ULONG                                   request_length;
  ULONG                                   transmit_length;

    /* Get the pointer to the device.  */
    device =  &_ux_system_slave->ux_system_slave_device;

    /* Get the pointer to the transfer request associated with the control endpoint.  */
    transfer_request =  &device->ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;

    /* Extract all necessary fields of the request.  */
    request =  *(transfer_request -> ux_slave_transfer_request_setup + UX_SETUP_REQUEST);

    /* Extract all necessary fields of the value.  */
    value =  _ux_utility_short_get(transfer_request -> ux_slave_transfer_request_setup + UX_SETUP_VALUE);

    /* Pickup the request length.  */
    request_length =   _ux_utility_short_get(transfer_request -> ux_slave_transfer_request_setup + UX_SETUP_LENGTH);

    transmit_length = request_length ;

    /* Here we proceed only the standard request we know of at the device level.  */
    switch (request)
    {

        case UX_SLAVE_CLASS_CDC_ACM_SET_CONTROL_LINE_STATE:
            dtr_state = 0;
            rts_state = 0;

            /* Get the line state parameters from the host.  DTR signal. */
            if (value & UX_SLAVE_CLASS_CDC_ACM_LINE_STATE_DTR)
                dtr_state = UX_TRUE;

            /* Get the line state parameters from the host.  RTS signal. */
            if (value & UX_SLAVE_CLASS_CDC_ACM_LINE_STATE_RTS)
                rts_state = UX_TRUE;

            break ;

        case UX_SLAVE_CLASS_CDC_ACM_GET_LINE_CODING:

            /* Setup the length appropriately.  */
            if (request_length >  UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_RESPONSE_SIZE)
                transmit_length = UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_RESPONSE_SIZE;

            /* Send the line coding default parameters back to the host.  */
            _ux_utility_long_put(transfer_request->ux_slave_transfer_request_data_pointer + UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_BAUDRATE_STRUCT,
                                 baudrate);
            *(transfer_request->ux_slave_transfer_request_data_pointer + UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_STOP_BIT_STRUCT) = stop_bit;
            *(transfer_request -> ux_slave_transfer_request_data_pointer + UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARITY_STRUCT) = parity;
            *(transfer_request -> ux_slave_transfer_request_data_pointer + UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_DATA_BIT_STRUCT) = data_bit;

            /* Set the phase of the transfer to data out.  */
            transfer_request -> ux_slave_transfer_request_phase =  UX_TRANSFER_PHASE_DATA_OUT;

            /* Perform the data transfer.  */
            _ux_device_stack_transfer_request(transfer_request, transmit_length, request_length);
            break;

        case UX_SLAVE_CLASS_CDC_ACM_SET_LINE_CODING:

            /* Get the line coding parameters from the host.  */
            baudrate  = _ux_utility_long_get(transfer_request -> ux_slave_transfer_request_data_pointer + UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_BAUDRATE_STRUCT);
            stop_bit  = *(transfer_request -> ux_slave_transfer_request_data_pointer + UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_STOP_BIT_STRUCT);
            parity    = *(transfer_request -> ux_slave_transfer_request_data_pointer + UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARITY_STRUCT);
            data_bit  = *(transfer_request -> ux_slave_transfer_request_data_pointer + UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_DATA_BIT_STRUCT);

            break ;

        default:

            /* Unknown function. It's not handled.  */
            return(UX_ERROR);
    }

    set_dtr(dtr_state);
    set_rts(rts_state);

    /* It's handled.  */
    return(UX_SUCCESS);
}

UINT USBXX::CDCACM::read(UCHAR *buffer, ULONG requested_length, ULONG *actual_length)
{
  UX_SLAVE_ENDPOINT           *endpoint;
  UX_SLAVE_DEVICE             *device;
  UX_SLAVE_INTERFACE          *interface_ptr;
  UX_SLAVE_TRANSFER           *transfer_request;
  UINT                        status= UX_SUCCESS;
  ULONG                       local_requested_length;

  /* Get the pointer to the device.  */
  device =  &_ux_system_slave->ux_system_slave_device;

  /* As long as the device is in the CONFIGURED state.  */
  if (device->ux_slave_device_state != UX_DEVICE_CONFIGURED)
  {
      _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_CONFIGURATION_HANDLE_UNKNOWN);

      return UX_CONFIGURATION_HANDLE_UNKNOWN;
  }

  /* This is the first time we are activated. We need the interface to the class.  */
  interface_ptr =  cdc_acm_interface;

  /* Locate the endpoints.  */
  endpoint =  interface_ptr->ux_slave_interface_first_endpoint;

  /* Check the endpoint direction, if OUT we have the correct endpoint.  */
  if ((endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) != UX_ENDPOINT_OUT)
  {
      /* So the next endpoint has to be the OUT endpoint.  */
      endpoint = endpoint->ux_slave_endpoint_next_endpoint;
  }

  {
    TXX::Mutex::guard guard(ep_in_mutex);
    transfer_request = &endpoint->ux_slave_endpoint_transfer_request;

    *actual_length =  0;

    while (device->ux_slave_device_state == UX_DEVICE_CONFIGURED && requested_length)
    {
      /* Check if we have enough in the local buffer.  */
      if (requested_length > endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize)
          local_requested_length = endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
      else
          local_requested_length = requested_length;

      /* Send the request to the device controller.  */
      status =  _ux_device_stack_transfer_request(transfer_request, local_requested_length, local_requested_length);

      if (status != UX_SUCCESS) {
        return status;
      }

      /* We need to copy the buffer locally.  */
      _ux_utility_memory_copy(buffer, transfer_request -> ux_slave_transfer_request_data_pointer,
                      transfer_request -> ux_slave_transfer_request_actual_length); /* Use case of memcpy is verified. */

      /* Next buffer address.  */
      buffer += transfer_request -> ux_slave_transfer_request_actual_length;

      /* Set the length actually received. */
      *actual_length += transfer_request -> ux_slave_transfer_request_actual_length;

      /* Decrement what left has to be done.  */
      requested_length -= transfer_request -> ux_slave_transfer_request_actual_length;


      /* Is this a short packet or a ZLP indicating we are done with this transfer ?  */
      if (transfer_request->ux_slave_transfer_request_actual_length < endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize)
          return(UX_SUCCESS);
    }
  }

  if (device -> ux_slave_device_state != UX_DEVICE_CONFIGURED)
      return UX_TRANSFER_NO_ANSWER;

  return status;
}


UINT USBXX::CDCACM::write(UCHAR *buffer,
                          ULONG requested_length,
                          ULONG *actual_length)
{
  UX_SLAVE_ENDPOINT           *endpoint;
  UX_SLAVE_DEVICE             *device;
  UX_SLAVE_INTERFACE          *interface_ptr;
  UX_SLAVE_TRANSFER           *transfer_request;
  ULONG                       local_requested_length;
  ULONG                       local_host_length;
  UINT                        status = 0;

  /* Get the pointer to the device.  */
  device =  &_ux_system_slave->ux_system_slave_device;

  /* As long as the device is in the CONFIGURED state.  */
  if (device->ux_slave_device_state != UX_DEVICE_CONFIGURED)
  {
    return UX_CONFIGURATION_HANDLE_UNKNOWN;
  }

  /* We need the interface to the class.  */
  interface_ptr = cdc_acm_interface;

  /* Locate the endpoints.  */
  endpoint = interface_ptr->ux_slave_interface_first_endpoint;

  /* Check the endpoint direction, if IN we have the correct endpoint.  */
  if ((endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) != UX_ENDPOINT_IN)
  {
      /* So the next endpoint has to be the IN endpoint.  */
      endpoint = endpoint->ux_slave_endpoint_next_endpoint;
  }

  {
    TXX::Mutex::guard guard(ep_out_mutex);

    /* We are writing to the IN endpoint.  */
    transfer_request =  &endpoint -> ux_slave_endpoint_transfer_request;

    /* Reset the actual length.  */
    *actual_length =  0;

    /* Check if the application forces a 0 length packet.  */
    if (device->ux_slave_device_state == UX_DEVICE_CONFIGURED && requested_length == 0)
      return _ux_device_stack_transfer_request(transfer_request, 0, 0);


    /* Check if we need more transactions.  */
    local_host_length = UX_DEVICE_CLASS_CDC_ACM_WRITE_BUFFER_SIZE;

    while (device -> ux_slave_device_state == UX_DEVICE_CONFIGURED && requested_length != 0)
    {

      /* Check if we have enough in the local buffer.  */
      if (requested_length > UX_DEVICE_CLASS_CDC_ACM_WRITE_BUFFER_SIZE)
          /* We have too much to transfer.  */
          local_requested_length = UX_DEVICE_CLASS_CDC_ACM_WRITE_BUFFER_SIZE;
      else
      {
          local_requested_length = requested_length;
          local_host_length = requested_length;
      }

      /* On a out, we copy the buffer to the caller. Not very efficient but it makes the API
         easier.  */
      _ux_utility_memory_copy(transfer_request -> ux_slave_transfer_request_data_pointer,
                          buffer, local_requested_length); /* Use case of memcpy is verified. */

      /* Send the request to the device controller.  */
      status =  _ux_device_stack_transfer_request(transfer_request, local_requested_length, local_host_length);

      if (status != UX_SUCCESS) {
        return status;
      }
          /* Next buffer address.  */
      buffer += transfer_request -> ux_slave_transfer_request_actual_length;

      /* Set the length actually received. */
      *actual_length += transfer_request -> ux_slave_transfer_request_actual_length;

      /* Decrement what left has to be done.  */
      requested_length -= transfer_request -> ux_slave_transfer_request_actual_length;

    }
  }

  /* Check why we got here, either completion or device was extracted.  */
  if (device -> ux_slave_device_state != UX_DEVICE_CONFIGURED)
      return UX_TRANSFER_NO_ANSWER;

  /* Simply return the last transaction result.  */
  return status;
}

UINT USBXX::CDCACM::ioctl(ULONG ioctl_function,
                          VOID *parameter)
{
  UINT status;
  UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER *line_coding;
  UX_SLAVE_CLASS_CDC_ACM_LINE_STATE_PARAMETER *line_state;
  UX_SLAVE_ENDPOINT *endpoint;
  UX_SLAVE_INTERFACE *interface_ptr;
  UX_SLAVE_TRANSFER *transfer_request;

  /* Let's be optimist ! */
  status = UX_SUCCESS;

  /* The command request will tell us what we need to do here.  */
  switch (ioctl_function)
  {
  case UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_LINE_CODING:
    line_coding = (UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER *) parameter;

    baudrate  =  line_coding -> ux_slave_class_cdc_acm_parameter_baudrate;
    stop_bit  =  line_coding -> ux_slave_class_cdc_acm_parameter_stop_bit;
    parity    =  line_coding -> ux_slave_class_cdc_acm_parameter_parity;
    data_bit  =  line_coding -> ux_slave_class_cdc_acm_parameter_data_bit;

    break;

  case UX_SLAVE_CLASS_CDC_ACM_IOCTL_GET_LINE_CODING:

    /* Properly cast the parameter pointer.  */
    line_coding = (UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER *) parameter;

    /* Save the parameters in the cdc_acm function.  */
    line_coding->ux_slave_class_cdc_acm_parameter_baudrate = baudrate;
    line_coding->ux_slave_class_cdc_acm_parameter_stop_bit = stop_bit;
    line_coding->ux_slave_class_cdc_acm_parameter_parity   = parity;
    line_coding->ux_slave_class_cdc_acm_parameter_data_bit = data_bit;

    break;


  case UX_SLAVE_CLASS_CDC_ACM_IOCTL_GET_LINE_STATE:

            /* Properly cast the parameter pointer.  */
    line_state = (UX_SLAVE_CLASS_CDC_ACM_LINE_STATE_PARAMETER *) parameter;

    /* Return the DTR/RTS signals.  */
    line_state -> ux_slave_class_cdc_acm_parameter_rts = rts_state;
    line_state -> ux_slave_class_cdc_acm_parameter_dtr = dtr_state;

    break;

  case UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_LINE_STATE:

    /* Properly cast the parameter pointer.  */
    line_state = (UX_SLAVE_CLASS_CDC_ACM_LINE_STATE_PARAMETER *) parameter;

    /* Set the DTR/RTS signals.  */
    rts_state = line_state -> ux_slave_class_cdc_acm_parameter_rts;
    dtr_state = line_state -> ux_slave_class_cdc_acm_parameter_dtr;

    break;


  case UX_SLAVE_CLASS_CDC_ACM_IOCTL_ABORT_PIPE:

    /* Get the interface from the instance.  */
    interface_ptr =  cdc_acm_interface;

    /* Locate the endpoints.  */
    endpoint =  interface_ptr -> ux_slave_interface_first_endpoint;

    /* What direction ?  */
    switch( (ULONG) (ALIGN_TYPE) parameter)
    {
        case UX_SLAVE_CLASS_CDC_ACM_ENDPOINT_XMIT :

        /* Check the endpoint direction, if IN we have the correct endpoint.  */
        if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) != UX_ENDPOINT_IN)
        {

            /* So the next endpoint has to be the XMIT endpoint.  */
            endpoint =  endpoint -> ux_slave_endpoint_next_endpoint;
        }
        break;

        case UX_SLAVE_CLASS_CDC_ACM_ENDPOINT_RCV :

        /* Check the endpoint direction, if OUT we have the correct endpoint.  */
        if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) != UX_ENDPOINT_OUT)
        {

            /* So the next endpoint has to be the RCV endpoint.  */
            endpoint =  endpoint -> ux_slave_endpoint_next_endpoint;
        }
        break;



        default :

        /* Parameter not supported. Return an error.  */
        status =  UX_ENDPOINT_HANDLE_UNKNOWN;
    }

    /* Get the transfer request associated with the endpoint.  */
    transfer_request =  &endpoint -> ux_slave_endpoint_transfer_request;


    /* Check the status of the transfer. */
    if (transfer_request -> ux_slave_transfer_request_status ==  UX_TRANSFER_STATUS_PENDING)
    {

        /* Abort the transfer.  */
    _ux_device_stack_transfer_abort(transfer_request, UX_ABORTED);

    }
    break;

  case UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_READ_TIMEOUT:
  case UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_WRITE_TIMEOUT:

      /* Get the interface from the instance.  */
      interface_ptr =  cdc_acm_interface;

      /* Locate the endpoints.  */
      endpoint =  interface_ptr -> ux_slave_interface_first_endpoint;

      /* If it's reading timeout but endpoint is OUT, it should be the next one.  */
      if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) !=
          (ULONG)((ioctl_function == UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_READ_TIMEOUT) ? UX_ENDPOINT_OUT : UX_ENDPOINT_IN))
          endpoint = endpoint -> ux_slave_endpoint_next_endpoint;

      /* Get the transfer request associated with the endpoint.  */
      transfer_request =  &endpoint -> ux_slave_endpoint_transfer_request;

      /* Check the status of the transfer.  */
      if (transfer_request -> ux_slave_transfer_request_status ==  UX_TRANSFER_STATUS_PENDING)
          status = UX_ERROR;
      else
          transfer_request -> ux_slave_transfer_request_timeout = (ULONG) (ALIGN_TYPE) parameter;

      break;

  default:
    /* Error trap. */
    _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_FUNCTION_NOT_SUPPORTED);

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_FUNCTION_NOT_SUPPORTED, 0, 0, 0, UX_TRACE_ERRORS, 0, 0)

    /* Function not supported. Return an error.  */
    status =  UX_FUNCTION_NOT_SUPPORTED;
  }

  /* Return status to caller.  */
  return status;
}
