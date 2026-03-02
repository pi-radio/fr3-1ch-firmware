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
                   tx_mutex("CDCACM TX Mutex")
{
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


void CDCACM::set_instance(UX_SLAVE_CLASS_CDC_ACM *p)
{
  cdc_acm = p;
  tx_event_flags_set(&flags, FLAG_STARTED, TX_OR);
}

bool CDCACM::get_dtr()
{
  if (cdc_acm == nullptr) return false;

  return cdc_acm->ux_slave_class_cdc_acm_data_dtr_state;
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
    status = ux_device_class_cdc_acm_read(cdc_acm, rx_buf, 64, &rx_len);
    
    while(status == UX_TRANSFER_BUS_RESET) {      
      tx_thread_sleep(10);

      status = ux_device_class_cdc_acm_read(cdc_acm, rx_buf, 64, &rx_len);      
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

      result = ux_device_class_cdc_acm_write(cdc_acm, p, l, &tx_len);
      
      while (result != TX_SUCCESS) {
        tx_thread_sleep(10);
        result = ux_device_class_cdc_acm_write(cdc_acm, p, l, &tx_len);
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


Class_CDCACM::Class_CDCACM(DeviceInfo *_dev) : ClassInfo(_dev, Descriptors::CDC_ACM, 2, 0)
{
}

extern "C" VOID USBD_CDC_ACM_Activate(VOID *i)
{
  if (CDCACM::stupid_global == NULL)
    return;
  
  CDCACM::stupid_global->set_instance((UX_SLAVE_CLASS_CDC_ACM *)i);

  return;
}

extern "C" VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance)
{
}

extern "C" VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance)
{
  CDCACM::stupid_global->set_dtr(CDCACM::stupid_global->cdc_acm->ux_slave_class_cdc_acm_data_dtr_state);
  CDCACM::stupid_global->set_rts(CDCACM::stupid_global->cdc_acm->ux_slave_class_cdc_acm_data_rts_state);
}

#include <usbxx/descriptor.hpp>
#include <usb.h>
#include <ux_device_descriptors.h>

void USBXX::CDCACM::register_class()
{
  cdc_acm_parameter.ux_slave_class_cdc_acm_instance_activate   = USBD_CDC_ACM_Activate;
  cdc_acm_parameter.ux_slave_class_cdc_acm_instance_deactivate = USBD_CDC_ACM_Deactivate;
  cdc_acm_parameter.ux_slave_class_cdc_acm_parameter_change    = USBD_CDC_ACM_ParameterChange;

  cdc_acm_configuration_number = USBD_Get_Configuration_Number(CLASS_TYPE_CDC_ACM, 0);

  cdc_acm_interface_number = USBD_Get_Interface_Number(CLASS_TYPE_CDC_ACM, 0);

  /* Initialize the device cdc acm class */
  if (ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name,
                                     ux_device_class_cdc_acm_entry,
                                     cdc_acm_configuration_number,
                                     cdc_acm_interface_number,
                                     &cdc_acm_parameter) != UX_SUCCESS)
  {
    throw std::runtime_error("Failed to register CDC ACM class");
  }

}
