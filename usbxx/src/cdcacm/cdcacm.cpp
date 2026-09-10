/*
 * usbxxcdcacm.cpp
 *
 *  Created on: Feb 21, 2026
 *      Author: zapman
 */

#include <threadxx/dbgstream.hpp>
#include <usbxx/cdcacm.hpp>
#include <usbxx/event_log.hpp>

#include <cassert>

using namespace USBXX;

CDCACMClass::CDCACMClass(DeviceBase *_dev) :
    USBClass("CDC ACM", _dev),
    ep_in_mutex("CDC ACM EP In Mutex"),
    ep_out_mutex("CDC ACM EP Out Mutex"),
    tx_queue("CDC ACM TX Queue"),
    tx_thread("CDC ACM TX Thread", this, &CDCACMClass::_tx_thread),
    rx_mutex("CDC ACM RX Mutex"),
    rx_cur(0), rx_len(0),
    tx_mutex("CDC ACM TX Mutex"),
    tx_count(0), rx_count(0)
{
  tx_event_flags_create(&flags, (char *)"CDCACM flags");

  tx_semaphore_create(&flush_sema, (char *)"Terminal Flush Semaphore", 0);

  tx_queue.create();

}




void CDCACMClass::wait_activated()
{
  uint32_t actual;

  tx_event_flags_get(&flags, FLAG_ACTIVATED, TX_AND, &actual, TX_WAIT_FOREVER);
}


bool CDCACMClass::get_dtr()
{
  return dtr_state;
}


void CDCACMClass::set_dtr(bool dtr)
{
  dtr_state = dtr;
  if (dtr) {
    tx_event_flags_set(&flags, ~FLAG_DTR, TX_AND);
  } else {
    tx_event_flags_set(&flags, FLAG_DTR, TX_OR);
  }
}

void CDCACMClass::set_rts(bool rts)
{
  rts_state = rts;
  if (rts) {
    tx_event_flags_set(&flags, ~FLAG_RTS, TX_AND);
  } else {
    tx_event_flags_set(&flags, FLAG_RTS, TX_OR);
  }
}




bool CDCACMClass::query(Interface::ptr iface)
{
  return iface->descriptor.bInterfaceClass == 2 ||
      iface->descriptor.bInterfaceClass == 10; // TODO -- Make a constant
}

void CDCACMClass::flush()
{
  putc(FLUSH);
  tx_semaphore_get(&flush_sema, TX_WAIT_FOREVER);
}

int CDCACMClass::getc()
{
  uint32_t status;

  wait_activated();
  
  TXX::Mutex::guard g(rx_mutex);
  
  // Refill buffer
  while (rx_cur >= rx_len) {
    status = read(rx_buf, 64, &rx_len);
    
    while(status == UX_TRANSFER_BUS_RESET) {      
      wait_activated();

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

void CDCACMClass::putc(int c)
{
  tx_queue.send(c);
}

void CDCACMClass::flush_buffer()
{
  TXX::Mutex::guard g(tx_mutex);
  
  while (!tx_buf.empty()) {
    auto res = tx_buf.get_seg();
    uint8_t *p = (uint8_t *)res.first;
    uint32_t l = res.second;
    uint32_t result;
    
    while (l) {
      uint32_t tx_len;

      result = write(p, l, &tx_len);
      
      while (result != TX_SUCCESS) {
        uint32_t _flags;
        if (tx_event_flags_get(&flags, FLAG_ACTIVATED, TX_AND, &_flags, TX_WAIT_FOREVER) != 0) {
          tx_thread_sleep(10);
          // Maybe clear tx_buf??
        }

        result = write(p, l, &tx_len);
      }
      
      l -= tx_len;
      p += tx_len;
    }
  }  
}

void CDCACMClass::_tx_thread()
{
  uint32_t c;
  uint32_t wait;

  wait_activated();

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

    tx_buf.push(c);

    tx_count++;
  }
}

#include <usbxx/descriptor.hpp>
#include <usb.h>


uint32_t CDCACMClass::initialize()
{
  /* Update the line coding fields with default values.  */
  baudrate  =  USBClass_CDC_ACM_LINE_CODING_BAUDRATE;
  stop_bit  =  USBClass_CDC_ACM_LINE_CODING_STOP_BIT;
  parity    =  USBClass_CDC_ACM_LINE_CODING_PARITY;
  data_bit  =  USBClass_CDC_ACM_LINE_CODING_DATA_BIT;

  return 0;
}

uint32_t CDCACMClass::uninitialize()
{
  return 0;
}


uint32_t CDCACMClass::activate(std::shared_ptr<Interface> iface)
{
  iface->class_instance = (VOID *)this;

  if (iface->descriptor.bInterfaceClass == 2)
    return 0;


  cdc_acm_interface = iface;

  for (auto endpoint : iface->endpoints) {
    if (endpoint->is_in())
      in_endpoint = endpoint;
    else
      out_endpoint = endpoint;
  }

  tx_event_flags_set(&flags, FLAG_ACTIVATED, TX_OR);

  return 0;
}

uint32_t CDCACMClass::deactivate()
{
  tx_event_flags_set(&flags, ~FLAG_ACTIVATED, TX_AND);

  /* Terminate the transactions pending on the endpoints.  */
  in_endpoint->abort_all_transfers(UX_TRANSFER_BUS_RESET);
  out_endpoint->abort_all_transfers(UX_TRANSFER_BUS_RESET);

  /* Terminate transmission and free resources.  */
  ioctl(USBClass_CDC_ACM_IOCTL_TRANSMISSION_STOP, nullptr);

  /* We need to reset the DTR and RTS values so they do not carry over to the
     next connection.  */
  set_dtr(0);
  set_rts(0);

  return 0;
}

uint32_t CDCACMClass::command_request(const ControlRequest &req)
{
  Transfer *xfer;
  uint32_t    transmit_length;


  /* Get the pointer to the transfer request associated with the control endpoint.  */
  xfer = device->get_control_transfer();

  event_log.push_event(UsbEvent::CDCACM_COMMAND, req.code);

  /* Extract all necessary fields of the value.  */
  //value =  usb_get_short(xfer -> setup + UX_SETUP_VALUE);

  transmit_length = req.length ;

  /* Here we proceed only the standard request we know of at the device level.  */
  switch (req.code)
  {

      case USBClass_CDC_ACM_SET_CONTROL_LINE_STATE:
          dtr_state = 0;
          rts_state = 0;

          /* Get the line state parameters from the host.  DTR signal. */
          if (req.value & USBClass_CDC_ACM_LINE_STATE_DTR)
              dtr_state = true;

          /* Get the line state parameters from the host.  RTS signal. */
          if (req.value & USBClass_CDC_ACM_LINE_STATE_RTS)
              rts_state = true;

          break ;

      case USBClass_CDC_ACM_GET_LINE_CODING:

          /* Setup the length appropriately.  */
          if (req.length >  USBClass_CDC_ACM_LINE_CODING_RESPONSE_SIZE)
              transmit_length = USBClass_CDC_ACM_LINE_CODING_RESPONSE_SIZE;

          /* Send the line coding default parameters back to the host.  */
          usb_put_long(xfer->data + USBClass_CDC_ACM_LINE_CODING_BAUDRATE_STRUCT,
                               baudrate);
          *(xfer->data + USBClass_CDC_ACM_LINE_CODING_STOP_BIT_STRUCT) = stop_bit;
          *(xfer -> data + USBClass_CDC_ACM_LINE_CODING_PARITY_STRUCT) = parity;
          *(xfer -> data + USBClass_CDC_ACM_LINE_CODING_DATA_BIT_STRUCT) = data_bit;

          /* Set the phase of the transfer to data out.  */
          xfer -> phase =  TransferPhase::DATA_OUT;

          /* Perform the data transfer.  */
          device->transfer_request(xfer, transmit_length, req.length);
          break;

      case USBClass_CDC_ACM_SET_LINE_CODING:

          /* Get the line coding parameters from the host.  */
          baudrate  = usb_get_long(xfer -> data + USBClass_CDC_ACM_LINE_CODING_BAUDRATE_STRUCT);
          stop_bit  = *(xfer -> data + USBClass_CDC_ACM_LINE_CODING_STOP_BIT_STRUCT);
          parity    = *(xfer -> data + USBClass_CDC_ACM_LINE_CODING_PARITY_STRUCT);
          data_bit  = *(xfer -> data + USBClass_CDC_ACM_LINE_CODING_DATA_BIT_STRUCT);

          break ;

      default:

          /* Unknown function. It's not handled.  */
          return(UX_ERROR);
  }

  set_dtr(dtr_state);
  set_rts(rts_state);

  /* It's handled.  */
  return 0;
}

uint32_t CDCACMClass::read(uint8_t *buffer, uint32_t requested_length, uint32_t *actual_length)
{
  uint32_t status = 0;
  uint32_t local_requested_length;

  /* As long as the device is in the CONFIGURED state.  */
  if (!device->is_configured())
    return UX_TRANSFER_NO_ANSWER;

  /* Locate the endpoints.  */
  auto endpoint = out_endpoint;

  /* Check the endpoint direction, if OUT we have the correct endpoint.  */
  {
    TXX::Mutex::guard guard(ep_in_mutex);
    auto xfer = endpoint->get_transfer();

    *actual_length =  0;

    while (requested_length)
    {
      wait_activated();

      /* Check if we have enough in the local buffer.  */
      if (requested_length > endpoint->descriptor.wMaxPacketSize)
          local_requested_length = endpoint->descriptor.wMaxPacketSize;
      else
          local_requested_length = requested_length;

      /* Send the request to the device controller.  */
      status = device->transfer_request(xfer, local_requested_length, local_requested_length);

      if (status == UX_TRANSFER_BUS_RESET) {
        continue;
      }

      if (status) {
        throw USBXX::runtime_error("read transfer failed");
      }

      /* We need to copy the buffer locally.  */
      ::memcpy(buffer, xfer->data, xfer->actual_length); /* Use case of memcpy is verified. */

      buffer += xfer->actual_length;
      *actual_length += xfer->actual_length;
      requested_length -= xfer -> actual_length;

      if (xfer->actual_length < endpoint->descriptor.wMaxPacketSize)
          return 0;
    }
  }

  if (!device->is_configured())
    return UX_TRANSFER_NO_ANSWER;

  return status;
}


uint32_t CDCACMClass::write(uint8_t *buffer,
                          uint32_t requested_length,
                          uint32_t *actual_length)
{
  Transfer           *xfer;
  uint32_t                       local_requested_length;
  uint32_t                       local_host_length;
  uint32_t                        status = 0;

  /* Get the pointer to the device.  */

  /* As long as the device is in the CONFIGURED state.  */
  if (!device->is_configured())
  {
    return UX_CONFIGURATION_HANDLE_UNKNOWN;
  }

  /* We need the interface to the class.  */
  auto iface = cdc_acm_interface;

  /* Locate the endpoints.  */
  auto endpoint = in_endpoint;

  {
    TXX::Mutex::guard guard(ep_out_mutex);

    /* We are writing to the IN endpoint.  */
    xfer = endpoint->get_transfer();

    /* Reset the actual length.  */
    *actual_length =  0;

    /* Check if the application forces a 0 length packet.  */
    if (device->is_configured() && requested_length == 0)
      return device->transfer_request(xfer, 0, 0);


    /* Check if we need more transactions.  */
    local_host_length = xfer->buffer_size;

    while (device->is_configured() && requested_length != 0)
    {
      wait_activated();

      /* Check if we have enough in the local buffer.  */
      if (requested_length > xfer->buffer_size)
          /* We have too much to transfer.  */
          local_requested_length = xfer->buffer_size;
      else
      {
          local_requested_length = requested_length;
          local_host_length = requested_length;
      }

      /* On a out, we copy the buffer to the caller. Not very efficient but it makes the API
         easier.  */
      ::memcpy(xfer->data, buffer, local_requested_length); /* Use case of memcpy is verified. */

      /* Send the request to the device controller.  */
      status = device->transfer_request(xfer, local_requested_length, local_host_length);

      if (status == UX_TRANSFER_BUS_RESET) {
        return UX_TRANSFER_NO_ANSWER;
      }

      if (status) {
        throw USBXX::runtime_error("Unable to complete transfer on CDCACM write");
      }
          /* Next buffer address.  */
      buffer += xfer -> actual_length;

      /* Set the length actually received. */
      *actual_length += xfer -> actual_length;

      /* Decrement what left has to be done.  */
      requested_length -= xfer -> actual_length;

    }
  }

  /* Check why we got here, either completion or device was extracted.  */
  if (!device->is_configured())
      return UX_TRANSFER_NO_ANSWER;

  /* Simply return the last transaction result.  */
  return status;
}

uint32_t CDCACMClass::ioctl(uint32_t ioctl_function,
                          void *parameter)
{
  uint32_t status;
  USBClass_CDC_ACM_LINE_CODING_PARAMETER *line_coding;
  USBClass_CDC_ACM_LINE_STATE_PARAMETER *line_state;
  Endpoint::ptr endpoint;
  Transfer *xfer;

  /* Let's be optimist ! */
  status = 0;

  /* The command request will tell us what we need to do here.  */
  switch (ioctl_function)
  {
  case USBClass_CDC_ACM_IOCTL_SET_LINE_CODING:
    line_coding = (USBClass_CDC_ACM_LINE_CODING_PARAMETER *) parameter;

    baudrate  =  line_coding -> cdc_acm_parameter_baudrate;
    stop_bit  =  line_coding -> cdc_acm_parameter_stop_bit;
    parity    =  line_coding -> cdc_acm_parameter_parity;
    data_bit  =  line_coding -> cdc_acm_parameter_data_bit;

    break;

  case USBClass_CDC_ACM_IOCTL_GET_LINE_CODING:

    /* Properly cast the parameter pointer.  */
    line_coding = (USBClass_CDC_ACM_LINE_CODING_PARAMETER *) parameter;

    /* Save the parameters in the cdc_acm function.  */
    line_coding->cdc_acm_parameter_baudrate = baudrate;
    line_coding->cdc_acm_parameter_stop_bit = stop_bit;
    line_coding->cdc_acm_parameter_parity   = parity;
    line_coding->cdc_acm_parameter_data_bit = data_bit;

    break;


  case USBClass_CDC_ACM_IOCTL_GET_LINE_STATE:

            /* Properly cast the parameter pointer.  */
    line_state = (USBClass_CDC_ACM_LINE_STATE_PARAMETER *) parameter;

    /* Return the DTR/RTS signals.  */
    line_state -> cdc_acm_parameter_rts = rts_state;
    line_state -> cdc_acm_parameter_dtr = dtr_state;

    break;

  case USBClass_CDC_ACM_IOCTL_SET_LINE_STATE:

    /* Properly cast the parameter pointer.  */
    line_state = (USBClass_CDC_ACM_LINE_STATE_PARAMETER *) parameter;

    /* Set the DTR/RTS signals.  */
    rts_state = line_state -> cdc_acm_parameter_rts;
    dtr_state = line_state -> cdc_acm_parameter_dtr;

    break;


  case USBClass_CDC_ACM_IOCTL_ABORT_PIPE:
  {

    /* Get the interface from the instance.  */
    auto iface =  cdc_acm_interface;

    /* What direction ?  */
    switch( (uint32_t) (ALIGN_TYPE) parameter)
    {
    case USBClass_CDC_ACM_ENDPOINT_XMIT :
      endpoint = in_endpoint;
      break;

    case USBClass_CDC_ACM_ENDPOINT_RCV :
      endpoint = out_endpoint;
      break;

    default:
      throw USBXX::runtime_error("Unknown endpoint handle");
    }

    /* Get the transfer request associated with the endpoint.  */
    xfer =  endpoint->get_transfer();


    /* Check the status of the transfer. */
    if (xfer->is_pending())
    {
      xfer->abort(UX_ABORTED);

    }
    break;
  }

  case USBClass_CDC_ACM_IOCTL_SET_READ_TIMEOUT:
    if (out_endpoint)
    {
      auto xfer = out_endpoint->get_transfer();

      if (xfer->is_pending())
        return UX_ERROR;

      xfer->timeout = (uint32_t) (ALIGN_TYPE) parameter;
    }
    break;

  case USBClass_CDC_ACM_IOCTL_SET_WRITE_TIMEOUT:
    if (in_endpoint)
    {
      auto xfer = in_endpoint->get_transfer();

      if (xfer->is_pending())
        return UX_ERROR;

      xfer->timeout = (uint32_t) (ALIGN_TYPE) parameter;
    }
    break;

  case USBClass_CDC_ACM_IOCTL_TRANSMISSION_STOP:
    break;

  default:
    __asm volatile ("BKPT     %0" : : "i"(0));
    status =  UX_FUNCTION_NOT_SUPPORTED;
  }

  /* Return status to caller.  */
  return status;
}


CDCACMDevice::CDCACMDevice()
{
  add_class(USBXX::CLASS_TYPE_CDC_ACM);

  cdcacm = std::make_shared<CDCACMClass>(this);
}

void CDCACMDevice::class_init()
{
  cdc_acm_configuration_number = get_configuration_number(CLASS_TYPE_CDC_ACM, 0);

  cdc_acm_interface_number = get_interface_number(CLASS_TYPE_CDC_ACM, 0);


  /* Initialize the device cdc acm class */
  if (register_class(cdcacm,
                     cdc_acm_configuration_number,
                     cdc_acm_interface_number,
                     NULL))
  {
    throw USBXX::runtime_error("Failed to register CDC ACM class");
  }
}
