#pragma once


#include <threadxx/thread.hpp>
#include <threadxx/ring_buffer.hpp>
#include <threadxx/mutex.hpp>
#include <threadxx/queue.hpp>

#include <usbxx/class.hpp>
#include <usbxx/device.hpp>

#define UX_SLAVE_CLASS_CDC_ACM_SEND_ENCAPSULATED_COMMAND                0x00
#define UX_SLAVE_CLASS_CDC_ACM_GET_ENCAPSULATED_RESPONSE                0x01
#define UX_SLAVE_CLASS_CDC_ACM_SET_COMM_FEATURE                         0x02
#define UX_SLAVE_CLASS_CDC_ACM_GET_COMM_FEATURE                         0x03
#define UX_SLAVE_CLASS_CDC_ACM_CLEAR_COMM_FEATURE                       0x04
#define UX_SLAVE_CLASS_CDC_ACM_SET_AUX_LINE_STATE                       0x10
#define UX_SLAVE_CLASS_CDC_ACM_SET_HOOK_STATE                           0x11
#define UX_SLAVE_CLASS_CDC_ACM_PULSE_SETUP                              0x12
#define UX_SLAVE_CLASS_CDC_ACM_SEND_PULSE                               0x13
#define UX_SLAVE_CLASS_CDC_ACM_SET_PULSE_TIME                           0x14
#define UX_SLAVE_CLASS_CDC_ACM_RING_AUX_JACK                            0x15
#define UX_SLAVE_CLASS_CDC_ACM_SET_LINE_CODING                          0x20
#define UX_SLAVE_CLASS_CDC_ACM_GET_LINE_CODING                          0x21
#define UX_SLAVE_CLASS_CDC_ACM_SET_CONTROL_LINE_STATE                   0x22
#define UX_SLAVE_CLASS_CDC_ACM_SEND_BREAK                               0x23
#define UX_SLAVE_CLASS_CDC_ACM_SET_RINGER_PARMS                         0x30
#define UX_SLAVE_CLASS_CDC_ACM_GET_RINGER_PARMS                         0x31
#define UX_SLAVE_CLASS_CDC_ACM_SET_OPERATION_PARMS                      0x32
#define UX_SLAVE_CLASS_CDC_ACM_GET_OPERATION_PARMS                      0x33
#define UX_SLAVE_CLASS_CDC_ACM_SET_LINE_PARMS                           0x34
#define UX_SLAVE_CLASS_CDC_ACM_GET_LINE_PARMS                           0x35
#define UX_SLAVE_CLASS_CDC_ACM_DIAL_DIGITS                              0x36
#define UX_SLAVE_CLASS_CDC_ACM_SET_UNIT_PARAMETER                       0x37
#define UX_SLAVE_CLASS_CDC_ACM_GET_UNIT_PARAMETER                       0x38
#define UX_SLAVE_CLASS_CDC_ACM_CLEAR_UNIT_PARAMETER                     0x39
#define UX_SLAVE_CLASS_CDC_ACM_GET_PROFILE                              0x3A
#define UX_SLAVE_CLASS_CDC_ACM_SET_ETHERNET_MULTICAST_FILTERS           0x40
#define UX_SLAVE_CLASS_CDC_ACM_SET_ETHERNET_POWER_MANAGEMENT_PATTERN    0x41
#define UX_SLAVE_CLASS_CDC_ACM_GET_ETHERNET_POWER_MANAGEMENT_PATTERN    0x42
#define UX_SLAVE_CLASS_CDC_ACM_SET_ETHERNET_PACKET_FILTER               0x43
#define UX_SLAVE_CLASS_CDC_ACM_GET_ETHERNET_STATISTIC                   0x44
#define UX_SLAVE_CLASS_CDC_ACM_SET_ATM_DATA_FORMAT                      0x50
#define UX_SLAVE_CLASS_CDC_ACM_GET_ATM_DEVICE_STATISTICS                0x51
#define UX_SLAVE_CLASS_CDC_ACM_SET_ATM_DEFAULT_VC                       0x52
#define UX_SLAVE_CLASS_CDC_ACM_GET_ATM_VC_STATISTICS                    0x53

/* Default line coding values.  */
#define UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_BAUDRATE                     115200
#define UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_STOP_BIT                     1
#define UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARITY                       0
#define UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_DATA_BIT                     8

/* Define line coding structure.  */
#define UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_BAUDRATE_STRUCT              0
#define UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_STOP_BIT_STRUCT              4
#define UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARITY_STRUCT                5
#define UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_DATA_BIT_STRUCT              6
#define UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_RESPONSE_SIZE                7

/* Define line state bits.  */
#define UX_SLAVE_CLASS_CDC_ACM_LINE_STATE_DTR                           1
#define UX_SLAVE_CLASS_CDC_ACM_LINE_STATE_RTS                           2

/* Define Transfer direction bits.  */
#define UX_SLAVE_CLASS_CDC_ACM_ENDPOINT_XMIT                            1
#define UX_SLAVE_CLASS_CDC_ACM_ENDPOINT_RCV                             2

/* Define IOCTL functions.  */
#define UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_LINE_CODING                    1
#define UX_SLAVE_CLASS_CDC_ACM_IOCTL_GET_LINE_CODING                    2
#define UX_SLAVE_CLASS_CDC_ACM_IOCTL_GET_LINE_STATE                     3
#define UX_SLAVE_CLASS_CDC_ACM_IOCTL_ABORT_PIPE                         4
#define UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_LINE_STATE                     5
#define UX_SLAVE_CLASS_CDC_ACM_IOCTL_TRANSMISSION_START                 6
#define UX_SLAVE_CLASS_CDC_ACM_IOCTL_TRANSMISSION_STOP                  7
#define UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_READ_TIMEOUT                   8
#define UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_WRITE_TIMEOUT                  9

/* Define event group flag.  */
#define UX_DEVICE_CLASS_CDC_ACM_WRITE_EVENT                             1


/* CDC ACM read state machine states.  */
#define UX_DEVICE_CLASS_CDC_ACM_READ_START      (UX_STATE_STEP + 1)
#define UX_DEVICE_CLASS_CDC_ACM_READ_WAIT       (UX_STATE_STEP + 2)

/* CDC ACM write state machine states.  */
#define UX_DEVICE_CLASS_CDC_ACM_WRITE_START     (UX_STATE_STEP + 1)
#define UX_DEVICE_CLASS_CDC_ACM_WRITE_WAIT      (UX_STATE_STEP + 2)


/* Define Slave CDC Class Calling Parameter structure */

typedef struct UX_SLAVE_CLASS_CDC_ACM_PARAMETER_STRUCT
{
    VOID                    (*ux_slave_class_cdc_acm_instance_activate)(VOID *);
    VOID                    (*ux_slave_class_cdc_acm_instance_deactivate)(VOID *);
    VOID                    (*ux_slave_class_cdc_acm_parameter_change)(VOID *);

} UX_SLAVE_CLASS_CDC_ACM_PARAMETER;

/* Define CDC Class structure.  */


#define UX_DEVICE_CLASS_CDC_ACM_ENDPOINT_BUFFER_SIZE_CALC_OVERFLOW              \
        (UX_OVERFLOW_CHECK_ADD_ULONG(UX_DEVICE_CLASS_CDC_ACM_READ_BUFFER_SIZE, UX_DEVICE_CLASS_CDC_ACM_WRITE_BUFFER_SIZE))
#define UX_DEVICE_CLASS_CDC_ACM_ENDPOINT_BUFFER_SIZE    (UX_DEVICE_CLASS_CDC_ACM_READ_BUFFER_SIZE + UX_DEVICE_CLASS_CDC_ACM_WRITE_BUFFER_SIZE)
#define UX_DEVICE_CLASS_CDC_ACM_READ_BUFFER(acm)        ((acm) -> ux_device_class_cdc_acm_endpoint_buffer)
#define UX_DEVICE_CLASS_CDC_ACM_WRITE_BUFFER(acm)       (UX_DEVICE_CLASS_CDC_ACM_READ_BUFFER(acm) + UX_DEVICE_CLASS_CDC_ACM_READ_BUFFER_SIZE)


/* Define some CDC Class structures */

typedef struct UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER_STRUCT
{
    ULONG                               ux_slave_class_cdc_acm_parameter_baudrate;
    UCHAR                               ux_slave_class_cdc_acm_parameter_stop_bit;
    UCHAR                               ux_slave_class_cdc_acm_parameter_parity;
    UCHAR                               ux_slave_class_cdc_acm_parameter_data_bit;

} UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER;

typedef struct UX_SLAVE_CLASS_CDC_ACM_LINE_STATE_PARAMETER_STRUCT
{
    UCHAR                               ux_slave_class_cdc_acm_parameter_rts;
    UCHAR                               ux_slave_class_cdc_acm_parameter_dtr;

} UX_SLAVE_CLASS_CDC_ACM_LINE_STATE_PARAMETER;

typedef struct UX_SLAVE_CLASS_CDC_ACM_CALLBACK_PARAMETER_STRUCT
{
    UINT                                (*ux_device_class_cdc_acm_parameter_write_callback)(struct UX_SLAVE_CLASS_CDC_ACM_STRUCT *cdc_acm, UINT status, ULONG length);
    UINT                                (*ux_device_class_cdc_acm_parameter_read_callback)(struct UX_SLAVE_CLASS_CDC_ACM_STRUCT *cdc_acm, UINT status, UCHAR *data_pointer, ULONG length);

} UX_SLAVE_CLASS_CDC_ACM_CALLBACK_PARAMETER;



/* Requests - Ethernet Networking Control Model */

#define UX_SLAVE_CLASS_CDC_ACM_SEND_ENCAPSULATED_COMMAND                        0x00
                                        /* Issues a command in the format of the supported control
                                           protocol. The intent of this mechanism is to support
                                           networking devices (e.g., host-based cable modems)
                                           that require an additional vendor-defined interface for
                                           media specific hardware configuration and
                                           management.  */
#define UX_SLAVE_CLASS_CDC_ACM_GET_ENCAPSULATED_RESPONSE                        0x01
                                        /* Requests a response in the format of the supported
                                           control protocol.  */
#define UX_SLAVE_CLASS_CDC_ACM_SET_ETHERNET_MULTICAST_FILTERS                   0x40
                                        /* As applications are loaded and unloaded on the host,
                                           the networking transport will instruct the device's MAC
                                           driver to change settings of the Networking device's
                                           multicast filters.  */
#define UX_SLAVE_CLASS_CDC_ACM_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER     0x41
                                        /* Some hosts are able to conserve energy and stay quiet
                                           in a 'sleeping' state while not being used. USB
                                           Networking devices may provide special pattern filtering
                                           hardware that enables it to wake up the attached host
                                           on demand when something is attempting to contact the
                                           host (e.g., an incoming web browser connection).
                                           Primitives are needed in management plane to negotiate
                                           the setting of these special filters  */
#define UX_SLAVE_CLASS_CDC_ACM_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER     0x42
                                        /* Retrieves the status of the above power management
                                           pattern filter setting  */
#define UX_SLAVE_CLASS_CDC_ACM_SET_ETHERNET_PACKET_FILTER                       0x43
                                        /* Sets device filter for running a network analyzer
                                           application on the host machine  */
#define UX_SLAVE_CLASS_CDC_ACM_GET_ETHERNET_STATISTIC                           0x44
                                        /* Retrieves Ethernet device statistics such as frames
                                           transmitted, frames received, and bad frames received.  */

/* Define buffer length for IN/OUT pipes.  */

#define UX_SLAVE_CLASS_CDC_ACM_BUFFER_SIZE                  4096


extern "C" VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance);
extern "C" VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance);
extern "C" VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance);


namespace USBXX
{
#define UX_SLAVE_CLASS_CDC_ACM_CLASS                                    10
#define UX_DEVICE_CLASS_CDC_ACM_WRITE_BUFFER_SIZE                       UX_SLAVE_REQUEST_DATA_MAX_LENGTH

  // Make app stack paramaterizable
  class CDCACMDevice : public Device<8192, 2048> {
    ULONG cdc_acm_interface_number;
    ULONG cdc_acm_configuration_number;
    UX_SLAVE_CLASS_CDC_ACM_PARAMETER cdc_acm_parameter;
    //UX_SLAVE_CLASS_CDC_ACM cdc_acm;
    TX_EVENT_FLAGS_GROUP flags;
    
    // replace cdc_acm soon enough
    std::shared_ptr<Interface> cdc_acm_interface;
    TXX::Mutex ep_in_mutex;
    TXX::Mutex ep_out_mutex;
    ULONG baudrate;
    UCHAR stop_bit;
    UCHAR parity;
    UCHAR data_bit;
    UCHAR dtr_state;
    UCHAR rts_state;

    USBXX::Endpoint *in_endpoint;
    USBXX::Endpoint *out_endpoint;

    static constexpr uint32_t FLAG_STARTED = 0x00000001;
    static constexpr uint32_t FLAG_CONNECTED = 0x00000002;
    static constexpr uint32_t FLAG_ATTACHED = 0x00000004;
    static constexpr uint32_t FLAG_DTR = 0x00000008;
    static constexpr uint32_t FLAG_RTS = 0x00000010;
    
    static CDCACMDevice *stupid_global;

    void register_class() override;

    UINT device_entry(UX_SLAVE_CLASS_COMMAND *);
    UINT acm_initialize(UX_SLAVE_CLASS_COMMAND *);
    UINT acm_uninitialize(UX_SLAVE_CLASS_COMMAND *);
    UINT activate(UX_SLAVE_CLASS_COMMAND *);
    UINT deactivate(UX_SLAVE_CLASS_COMMAND *);
    UINT control_request(UX_SLAVE_CLASS_COMMAND *);

    UINT ioctl(ULONG ioctl_function, VOID *parameter);

    UINT read(UCHAR *buffer, ULONG requested_length, ULONG *actual_length);
    UINT write(UCHAR *buffer, ULONG requested_length, ULONG *actual_length);

    static UINT _device_entry(UX_SLAVE_CLASS_COMMAND *);

    static constexpr int RX_QUEUE_LEN = 64;
    static constexpr int TX_QUEUE_LEN = 64;
    
    TXX::Queue<1, TX_QUEUE_LEN> tx_queue;

    TXX::MemberThread<CDCACMDevice, 4096> tx_thread;

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

    
  public:

    static constexpr uint32_t FLUSH = 0xFFFF0000;
    
    CDCACMDevice();

    void wait_started();
    
    void flush();
    void putc(int c);
    int getc();

    bool get_dtr();
  };  
}
