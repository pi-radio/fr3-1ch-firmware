#pragma once

#include <usbxx/device.hpp>
#include <ux_device_class_cdc_acm.h>

#include <threadxx/thread.hpp>
#include <threadxx/queue.hpp>
#include <threadxx/ring_buffer.hpp>
#include <threadxx/mutex.hpp>

extern "C" VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance);
extern "C" VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance);
extern "C" VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance);

namespace USBXX
{
  // Make app stack paramaterizable
  class CDCACM : public Device<8192, 2048> {
    ULONG cdc_acm_interface_number;
    ULONG cdc_acm_configuration_number;
    UX_SLAVE_CLASS_CDC_ACM_PARAMETER cdc_acm_parameter;
    UX_SLAVE_CLASS_CDC_ACM *cdc_acm;
    TX_EVENT_FLAGS_GROUP flags;
    
    static constexpr uint32_t FLAG_STARTED = 0x00000001;
    static constexpr uint32_t FLAG_CONNECTED = 0x00000002;
    static constexpr uint32_t FLAG_ATTACHED = 0x00000004;
    static constexpr uint32_t FLAG_DTR = 0x00000008;
    static constexpr uint32_t FLAG_RTS = 0x00000010;
    
    static CDCACM *stupid_global;

    friend VOID ::USBD_CDC_ACM_Activate(VOID *cdc_acm_instance);
    friend VOID ::USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance);
    friend VOID ::USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance);
      
    void register_class() override;

    static constexpr int RX_QUEUE_LEN = 64;
    static constexpr int TX_QUEUE_LEN = 64;
    
    TXX::Queue<1, TX_QUEUE_LEN> tx_queue;

    TXX::MemberThread<CDCACM, 4096> tx_thread;

    void _tx_thread();

    TXX::Mutex rx_mutex;
    uint8_t rx_buf[64];
    uint32_t rx_cur, rx_len;

    TXX::Mutex tx_mutex;
    TXX::ring_buffer<64> tx_buf;

    TX_SEMAPHORE flush_sema;

    void flush_buffer();
    
    void class_init() override;

    uint32_t tx_count;
    uint32_t rx_count;

    void set_dtr(bool);
    void set_rts(bool);
    void set_instance(UX_SLAVE_CLASS_CDC_ACM *);

    
  public:

    static constexpr uint32_t FLUSH = 0xFFFF0000;
    
    CDCACM();

    void wait_started();
    
    void flush();
    void putc(int c);
    int getc();

    bool get_dtr();
  };  
}
