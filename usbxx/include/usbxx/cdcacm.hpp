#pragma once


#include <threadxx/thread.hpp>
#include <threadxx/ring_buffer.hpp>
#include <threadxx/mutex.hpp>
#include <threadxx/eventflags.hpp>
#include <threadxx/queue.hpp>

#include <usbxx/class.hpp>
#include <usbxx/device.hpp>

namespace USBXX
{
//#define USBClass_CDC_ACM_CLASS                                    10
//#define UX_DEVICE_CLASS_CDC_ACM_WRITE_BUFFER_SIZE                       UX_SLAVE_REQUEST_DATA_MAX_LENGTH

  class CDCACMClass : public USBClass
  {
  public:
    static constexpr uint32_t FLAG_ACTIVATED = 0;
    static constexpr uint32_t FLAG_CONNECTED = 1;
    static constexpr uint32_t FLAG_ATTACHED = 2;
    static constexpr uint32_t FLAG_DTR = 3;
    static constexpr uint32_t FLAG_RTS = 4;

    static constexpr int RX_QUEUE_LEN = 64;
    static constexpr int TX_QUEUE_LEN = 64;

    static constexpr uint32_t FLUSH = 0xFFFF0000;

    struct Commands
    {
      static constexpr uint32_t SEND_ENCAPSULATED_COMMAND = 0x00;
      static constexpr uint32_t GET_ENCAPSULATED_RESPONSE = 0x01;
      static constexpr uint32_t SET_COMM_FEATURE = 0x02;
      static constexpr uint32_t GET_COMM_FEATURE = 0x03;
      static constexpr uint32_t CLEAR_COMM_FEATURE = 0x04;
      static constexpr uint32_t SET_AUX_LINE_STATE = 0x10;
      static constexpr uint32_t SET_HOOK_STATE = 0x11;
      static constexpr uint32_t PULSE_SETUP = 0x12;
      static constexpr uint32_t SEND_PULSE = 0x13;
      static constexpr uint32_t SET_PULSE_TIME = 0x14;
      static constexpr uint32_t RING_AUX_JACK = 0x15;
      static constexpr uint32_t SET_LINE_CODING = 0x20;
      static constexpr uint32_t GET_LINE_CODING = 0x21;
      static constexpr uint32_t SET_CONTROL_LINE_STATE = 0x22;
      static constexpr uint32_t SEND_BREAK = 0x23;
      static constexpr uint32_t SET_RINGER_PARMS = 0x30;
      static constexpr uint32_t GET_RINGER_PARMS = 0x31;
      static constexpr uint32_t SET_OPERATION_PARMS = 0x32;
      static constexpr uint32_t GET_OPERATION_PARMS = 0x33;
      static constexpr uint32_t SET_LINE_PARMS = 0x34;
      static constexpr uint32_t GET_LINE_PARMS = 0x35;
      static constexpr uint32_t DIAL_DIGITS = 0x36;
      static constexpr uint32_t SET_UNIT_PARAMETER = 0x37;
      static constexpr uint32_t GET_UNIT_PARAMETER = 0x38;
      static constexpr uint32_t CLEAR_UNIT_PARAMETER = 0x39;
      static constexpr uint32_t GET_PROFILE = 0x3A;
      static constexpr uint32_t SET_ETHERNET_MULTICAST_FILTERS = 0x40;
      static constexpr uint32_t SET_ETHERNET_POWER_MANAGEMENT_PATTERN = 0x41;
      static constexpr uint32_t GET_ETHERNET_POWER_MANAGEMENT_PATTERN = 0x42;
      static constexpr uint32_t SET_ETHERNET_PACKET_FILTER= 0x43;
      static constexpr uint32_t GET_ETHERNET_STATISTIC=     0x44;
      static constexpr uint32_t SET_ATM_DATA_FORMAT=        0x50;
      static constexpr uint32_t GET_ATM_DEVICE_STATISTICS=  0x51;
      static constexpr uint32_t SET_ATM_DEFAULT_VC=0x52;
      static constexpr uint32_t GET_ATM_VC_STATISTICS=0x53;
    };

    struct LineState
    {
      static constexpr uint32_t DTR = 1;
      static constexpr uint32_t RTS = 2;
    };

    static constexpr uint32_t CONTROL_INTERFACE_CLASS = 0x02;
    static constexpr uint32_t DATA_INTERFACE_CLASS = 0x0A;

    using ptr = std::shared_ptr<CDCACMClass>;

    struct __attribute__((packed)) line_coding
    {
      uint32_t                              baudrate;
      uint8_t                               stop_bit;
      uint8_t                               parity;
      uint8_t                               data_bit;
    };

    struct __attribute__((packed)) line_state
    {
      uint8_t                               rts;
      uint8_t                               dtr;
    };

    //USBClass_CDC_ACM cdc_acm;
    TXX::EventFlagsGroup flags;

    TXX::Mutex ep_in_mutex;
    TXX::Mutex ep_out_mutex;
    uint32_t baudrate;
    uint8_t stop_bit;
    uint8_t parity;
    uint8_t data_bit;
    bool dtr_state;
    bool rts_state;

    USBXX::Endpoint::ptr in_endpoint;
    USBXX::Endpoint::ptr out_endpoint;


    TXX::Queue<1, TX_QUEUE_LEN> tx_queue;

    TXX::MemberThread<CDCACMClass, 4096> tx_thread;

    TXX::Mutex rx_mutex;
    uint8_t rx_buf[64];
    uint32_t rx_cur, rx_len;

    TXX::Mutex tx_mutex;
    TXX::ring_buffer<64> tx_buf;

    TXX::Semaphore flush_sema;

    uint32_t tx_count;
    uint32_t rx_count;


    CDCACMClass(DeviceBase *_dev);

    uint32_t ioctl(uint32_t ioctl_function, void *parameter);

    uint32_t read(uint8_t *buffer, uint32_t requested_length, uint32_t *actual_length);
    uint32_t write(uint8_t *buffer, uint32_t requested_length, uint32_t *actual_length);

    void set_dtr(bool);
    void set_rts(bool);


    void _tx_thread();
    void flush_buffer();


    virtual uint32_t initialize();
    virtual uint32_t uninitialize() override;
    virtual uint32_t activate(std::shared_ptr<Interface>) override;
    virtual uint32_t deactivate() override;
    virtual bool query(std::shared_ptr<Interface>) override;
    virtual uint32_t command_request(const ControlRequest &) override;
    virtual uint32_t on_change() override { return 0; }

    void wait_activated();

    void flush();
    void putc(int c);
    int getc();

    bool get_dtr();

    void stop_transmission() {}
  };

  // Make app stack paramaterizable
  class CDCACMDevice : public Device<8192, 2048>
  {
    CDCACMClass::ptr cdcacm;

    uint32_t cdc_acm_interface_number;
    uint32_t cdc_acm_configuration_number;

    void class_init() override;

  public:
    CDCACMDevice();
    
    CDCACMClass::ptr get_cdcacm()
    {
      return cdcacm;
    }
  };  
}
