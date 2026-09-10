#pragma once


namespace USBXX
{
  enum class DeviceSpeed
  {
    FS,
    HS
  };

  struct DeviceFeatures
  {
    static constexpr uint32_t HALT_ENDPOINT = 0;
    static constexpr uint32_t REMOTE_WAKEUP = 1;
    static constexpr uint32_t TEST_MODE = 2;
  };

  struct StdControlRequest
  {
    static constexpr uint32_t GET_STATUS =         0u;
    static constexpr uint32_t CLEAR_FEATURE =      1u;
    static constexpr uint32_t SET_FEATURE =        3u;
    static constexpr uint32_t SET_ADDRESS =        5u;
    static constexpr uint32_t GET_DESCRIPTOR =     6u;
    static constexpr uint32_t SET_DESCRIPTOR =     7u;
    static constexpr uint32_t GET_CONFIGURATION =  8u;
    static constexpr uint32_t SET_CONFIGURATION =  9u;
    static constexpr uint32_t GET_INTERFACE =      10u;
    static constexpr uint32_t SET_INTERFACE =      11u;
    static constexpr uint32_t SYNCH_FRAME =        12u;
  };

  struct ControlRequestType
  {
    static constexpr uint32_t MASK = 0x60;
    static constexpr uint32_t STANDARD = 0;
  };


  struct EndpointType
  {
    static constexpr uint32_t MASK = 0x03;

    static constexpr uint32_t CONTROL = 0;
    static constexpr uint32_t ISOCHRONOUS = 1;
    static constexpr uint32_t BULK = 2;
    static constexpr uint32_t INTERRUPT = 3;
  };

#if 0
#define UX_CAPABILITY_WIRELESS_USB                                      0x01u
#define UX_CAPABILITY_USB_2_0_EXTENSION                                 0x02u
#define UX_CAPABILITY_SUPERSPEED_USB                                    0x03u
#define UX_CAPABILITY_CONTAINER_ID                                      0x04u
#define UX_CAPABILITY_PLATFORM                                          0x05u
#define UX_CAPABILITY_POWER_DELIVERY                                    0x06u
#define UX_CAPABILITY_BATTERY_INFO                                      0x07u
#define UX_CAPABILITY_PD_CONSUMER_PORT                                  0x08u
#define UX_CAPABILITY_PD_PROVIDER_PORT                                  0x09u
#define UX_CAPABILITY_SUPERSPEED_PLUS                                   0x0Au
#define UX_CAPABILITY_PRECISION_TIME_MEASUREMENT                        0x0Bu
#define UX_CAPABILITY_WIRELESS_USB_EXT                                  0x0Cu
#define UX_CAPABILITY_BILLBOARD                                         0x0Du
#define UX_CAPABILITY_AUTHENTICATION                                    0x0Eu
#define UX_CAPABILITY_BILLBOARD_EX                                      0x0Fu
#define UX_CAPABILITY_CONFIGURATION_SUMMARY                             0x10u
#endif

#define UX_DEVICE_BUS_POWERED                                           1u
#define UX_DEVICE_SELF_POWERED                                          2u
#define UX_MAX_SELF_POWER                                               (500u/2)
#define UX_MAX_BUS_POWER                                                (100u/2)
#define UX_CONFIGURATION_DEVICE_BUS_POWERED                             0x80u
#define UX_CONFIGURATION_DEVICE_SELF_POWERED                            0x40u
#define UX_STATUS_DEVICE_SELF_POWERED                                   1u

#define UX_OTG_STATUS_SELECTOR                                          0xF000u
#define UX_OTG_HOST_REQUEST_FLAG                                        0x01u

#define UX_ERROR                                                        0xff
#define UX_BUSY                                                         0xfe
#define UX_TIMEOUT                                                      0xfd
#define UX_REENTRY                                                      0xfc
#define UX_INVALID_STATE                                                0xfb
#define UX_INVALID_PARAMETER                                            0xfa
#define UX_ABORTED                                                      0xf9
#define UX_MATH_OVERFLOW                                                0xf8
#define UX_INVALID_BUILD_OPTION                                         0xf7

#define UX_TOO_MANY_DEVICES                                             0x11
#define UX_MEMORY_INSUFFICIENT                                          0x12
#define UX_NO_TD_AVAILABLE                                              0x13
#define UX_NO_ED_AVAILABLE                                              0x14
#define UX_SEMAPHORE_ERROR                                              0x15
#define UX_THREAD_ERROR                                                 0x16
#define UX_MUTEX_ERROR                                                  0x17
#define UX_EVENT_ERROR                                                  0x18
#define UX_MEMORY_CORRUPTED                                             0x19
#define UX_MEMORY_ARRAY_FULL                                            0x1a
#define UX_FATAL_ERROR                                                  0x1b
#define UX_ALREADY_ACTIVATED                                            0x1c

#define UX_TRANSFER_STALLED                                             0x21
#define UX_TRANSFER_NO_ANSWER                                           0x22
#define UX_TRANSFER_ERROR                                               0x23
#define UX_TRANSFER_MISSED_FRAME                                        0x24
#define UX_TRANSFER_NOT_READY                                           0x25
#define UX_TRANSFER_BUS_RESET                                           0x26
#define UX_TRANSFER_BUFFER_OVERFLOW                                     0x27
#define UX_TRANSFER_APPLICATION_RESET                                   0x28
#define UX_TRANSFER_DATA_LESS_THAN_EXPECTED                             0x29

#define UX_PORT_RESET_FAILED                                            0x31
#define UX_CONTROLLER_INIT_FAILED                                       0x32
#define UX_CONTROLLER_DEAD                                              0x33

#define UX_NO_BANDWIDTH_AVAILABLE                                       0x41
#define UX_DESCRIPTOR_CORRUPTED                                         0x42
#define UX_OVER_CURRENT_CONDITION                                       0x43
#define UX_DEVICE_ENUMERATION_FAILURE                                   0x44
#define UX_TOO_MANY_HUB_PORTS                                           0x45

#define UX_DEVICE_HANDLE_UNKNOWN                                        0x50
#define UX_CONFIGURATION_HANDLE_UNKNOWN                                 0x51
#define UX_INTERFACE_HANDLE_UNKNOWN                                     0x52
#define UX_ENDPOINT_HANDLE_UNKNOWN                                      0x53
#define UX_FUNCTION_NOT_SUPPORTED                                       0x54
#define UX_CONTROLLER_UNKNOWN                                           0x55
#define UX_PORT_INDEX_UNKNOWN                                           0x56
#define UX_NO_CLASS_MATCH                                               0x57
#define UX_HOST_CLASS_ALREADY_INSTALLED                                 0x58
#define UX_HOST_CLASS_UNKNOWN                                           0x59
#define UX_CONNECTION_INCOMPATIBLE                                      0x5a
#define UX_HOST_CLASS_INSTANCE_UNKNOWN                                  0x5b
#define UX_TRANSFER_TIMEOUT                                             0x5c
#define UX_BUFFER_OVERFLOW                                              0x5d
#define UX_NO_ALTERNATE_SETTING                                         0x5e
#define UX_NO_DEVICE_CONNECTED                                          0x5f

#define UX_HOST_CLASS_PROTOCOL_ERROR                                    0x60
#define UX_HOST_CLASS_MEMORY_ERROR                                      0x61
#define UX_HOST_CLASS_MEDIA_NOT_SUPPORTED                               0x62
#define UX_CLASS_MALFORMED_PACKET_RECEIVED_ERROR                        0x63

#define UX_HOST_CLASS_HID_REPORT_OVERFLOW                               0x70
#define UX_HOST_CLASS_HID_USAGE_OVERFLOW                                0x71
#define UX_HOST_CLASS_HID_TAG_UNSUPPORTED                               0x72
#define UX_HOST_CLASS_HID_PUSH_OVERFLOW                                 0x73
#define UX_HOST_CLASS_HID_POP_UNDERFLOW                                 0x74
#define UX_HOST_CLASS_HID_COLLECTION_OVERFLOW                           0x75
#define UX_HOST_CLASS_HID_COLLECTION_UNDERFLOW                          0x76
#define UX_HOST_CLASS_HID_MIN_MAX_ERROR                                 0x77
#define UX_HOST_CLASS_HID_DELIMITER_ERROR                               0x78
#define UX_HOST_CLASS_HID_REPORT_ERROR                                  0x79
#define UX_HOST_CLASS_HID_PERIODIC_REPORT_ERROR                         0x7A
#define UX_HOST_CLASS_HID_UNKNOWN                                       0x7B

#define UX_HOST_CLASS_AUDIO_WRONG_TYPE                                  0x80
#define UX_HOST_CLASS_AUDIO_WRONG_INTERFACE                             0x81
#define UX_HOST_CLASS_AUDIO_WRONG_FREQUENCY                             0x82

#define UX_CLASS_CDC_ECM_LINK_STATE_DOWN_ERROR                          0x90
#define UX_CLASS_ETH_LINK_STATE_DOWN_ERROR                              0x90
#define UX_CLASS_ETH_PACKET_POOL_ERROR                                  0x91
#define UX_CLASS_ETH_PACKET_ERROR                                       0x92
#define UX_CLASS_ETH_SIZE_ERROR                                         0x93


/* Define USBX HCD API function constants.  */

#define UX_HCD_DISABLE_CONTROLLER                                       1
#define UX_HCD_GET_PORT_STATUS                                          2
#define UX_HCD_ENABLE_PORT                                              3
#define UX_HCD_DISABLE_PORT                                             4
#define UX_HCD_POWER_ON_PORT                                            5
#define UX_HCD_POWER_DOWN_PORT                                          6
#define UX_HCD_SUSPEND_PORT                                             7
#define UX_HCD_RESUME_PORT                                              8
#define UX_HCD_RESET_PORT                                               9
#define UX_HCD_GET_FRAME_NUMBER                                         10
#define UX_HCD_SET_FRAME_NUMBER                                         11
#define UX_HCD_TRANSFER_REQUEST                                         12
#define UX_HCD_TRANSFER_RUN                                             12
#define UX_HCD_TRANSFER_ABORT                                           13
#define UX_HCD_CREATE_ENDPOINT                                          14
#define UX_HCD_DESTROY_ENDPOINT                                         15
#define UX_HCD_RESET_ENDPOINT                                           16
#define UX_HCD_PROCESS_DONE_QUEUE                                       17
#define UX_HCD_TASKS_RUN                                                17
#define UX_HCD_UNINITIALIZE                                             18

/* Define USBX DCD API function constants.  */

#define UX_DCD_DISABLE_CONTROLLER                                       1
#define UX_DCD_GET_PORT_STATUS                                          2
#define UX_DCD_ENABLE_PORT                                              3
#define UX_DCD_DISABLE_PORT                                             4
#define UX_DCD_POWER_ON_PORT                                            5
#define UX_DCD_POWER_DOWN_PORT                                          6
#define UX_DCD_SUSPEND_PORT                                             7
#define UX_DCD_RESUME_PORT                                              8
#define UX_DCD_RESET_PORT                                               9
#define UX_DCD_GET_FRAME_NUMBER                                         10
#define UX_DCD_SET_FRAME_NUMBER                                         11
#define UX_DCD_TRANSFER_REQUEST                                         12
#define UX_DCD_TRANSFER_RUN                                             12
#define UX_DCD_TRANSFER_ABORT                                           13
#define UX_DCD_CREATE_ENDPOINT                                          14
#define UX_DCD_DESTROY_ENDPOINT                                         15
#define UX_DCD_RESET_ENDPOINT                                           16
#define UX_DCD_SET_DEVICE_ADDRESS                                       17
#define UX_DCD_ISR_PENDING                                              18
#define UX_DCD_TASKS_RUN                                                18
#define UX_DCD_CHANGE_STATE                                             19
#define UX_DCD_STALL_ENDPOINT                                           20
#define UX_DCD_ENDPOINT_STATUS                                          21


/* Define USBX generic host controller constants.  */

#define UX_HCD_STATUS_UNUSED                                            0
#define UX_HCD_STATUS_HALTED                                            1
#define UX_HCD_STATUS_OPERATIONAL                                       2
#define UX_HCD_STATUS_DEAD                                              3

/* Define USBX generic SLAVE controller constants.  */

#define UX_DCD_STATUS_HALTED                                            0
#define UX_DCD_STATUS_OPERATIONAL                                       1
#define UX_DCD_STATUS_DEAD                                              2

/* Define USBX  SLAVE controller VBUS constants.  */

#define UX_DCD_VBUS_RESET                                               0
#define UX_DCD_VBUS_SET                                                 1

/* Define USBX class interface constants.  */

#define UX_HOST_CLASS_COMMAND_QUERY                                     1
#define UX_HOST_CLASS_COMMAND_ACTIVATE                                  2
#define UX_HOST_CLASS_COMMAND_DEACTIVATE                                3
#define UX_HOST_CLASS_COMMAND_DESTROY                                   4
#define UX_HOST_CLASS_COMMAND_ACTIVATE_START                            UX_HOST_CLASS_COMMAND_ACTIVATE
#define UX_HOST_CLASS_COMMAND_ACTIVATE_WAIT                             5

#define UX_SLAVE_CLASS_COMMAND_QUERY                                    1
#define UX_SLAVE_CLASS_COMMAND_ACTIVATE                                 2
#define UX_SLAVE_CLASS_COMMAND_DEACTIVATE                               3
#define UX_SLAVE_CLASS_COMMAND_REQUEST                                  4
#define UX_SLAVE_CLASS_COMMAND_INITIALIZE                               5
#define UX_SLAVE_CLASS_COMMAND_CHANGE                                   6
#define UX_SLAVE_CLASS_COMMAND_UNINITIALIZE                             7

#define UX_HOST_CLASS_COMMAND_USAGE_PIDVID                              1
#define UX_HOST_CLASS_COMMAND_USAGE_CSP                                 2
#define UX_HOST_CLASS_COMMAND_USAGE_DCSP                                3

#define UX_HOST_CLASS_INSTANCE_FREE                                     0
#define UX_HOST_CLASS_INSTANCE_LIVE                                     1
#define UX_HOST_CLASS_INSTANCE_SHUTDOWN                                 2
#define UX_HOST_CLASS_INSTANCE_MOUNTING                                 3


/* Define USBX root HUB constants.  */

#define UX_RH_ENUMERATION_RETRY                                         3
#define UX_RH_ENUMERATION_RETRY_DELAY                                   100


/* Define USBX PCI driver constants.  */

#define UX_PCI_NB_FUNCTIONS                                             7
#define UX_PCI_NB_DEVICE                                                32
#define UX_PCI_NB_BUS                                                   0xff

#define UX_PCI_CMD_IO_ENABLE                                            0x0001u
#define UX_PCI_CMD_MEM_ENABLE                                           0x0002u
#define UX_PCI_CMD_MASTER_ENABLE                                        0x0004u
#define UX_PCI_CMD_MONITOR_ENABLE                                       0x0008u
#define UX_PCI_CMD_MEM_WRITE_INV_ENABLE                                 0x0010u
#define UX_PCI_CMD_SNOOP_PALETTE_ENABLE                                 0x0020u
#define UX_PCI_CMD_PARITY_ERROR_ENABLE                                  0x0040u
#define UX_PCI_CMD_WAIT_CYCLE_CTRL_ENABLE                               0x0080u
#define UX_PCI_CMD_SERR_ENABLE                                          0x0100u
#define UX_PCI_CMD_FBB_ENABLE                                           0x0200u

#define UX_PCI_CFG_CTRL_ADDRESS                                         0x0cf8u
#define UX_PCI_CFG_DATA_ADDRESS                                         0x0cfcu

#define UX_PCI_CFG_VENDOR_ID                                            0x00
#define UX_PCI_CFG_DEVICE_ID                                            0x02
#define UX_PCI_CFG_COMMAND                                              0x04
#define UX_PCI_CFG_STATUS                                               0x06
#define UX_PCI_CFG_REVISION                                             0x08
#define UX_PCI_CFG_PROGRAMMING_IF                                       0x09
#define UX_PCI_CFG_SUBCLASS                                             0x0a
#define UX_PCI_CFG_CLASS                                                0x0b
#define UX_PCI_CFG_CACHE_LINE_SIZE                                      0x0c
#define UX_PCI_CFG_LATENCY_TIMER                                        0x0d
#define UX_PCI_CFG_HEADER_TYPE                                          0x0e
#define UX_PCI_CFG_BIST                                                 0x0f
#define UX_PCI_CFG_BASE_ADDRESS_0                                       0x10
#define UX_PCI_CFG_BASE_ADDRESS_1                                       0x14
#define UX_PCI_CFG_BASE_ADDRESS_2                                       0x18
#define UX_PCI_CFG_BASE_ADDRESS_3                                       0x1c
#define UX_PCI_CFG_BASE_ADDRESS_4                                       0x20
#define UX_PCI_CFG_BASE_ADDRESS_5                                       0x24
#define UX_PCI_CFG_CARDBUS_CIS                                          0x28
#define UX_PCI_CFG_SUB_VENDOR_ID                                        0x2c
#define UX_PCI_CFG_SUB_SYSTEM_ID                                        0x2e
#define UX_PCI_CFG_EXPANSION_ROM_ADDRESS                                0x30
#define UX_PCI_CFG_RESERVED_0                                           0x34
#define UX_PCI_CFG_RESERVED_1                                           0x38
#define UX_PCI_CFG_INT_LINE                                             0x3c
#define UX_PCI_CFG_INT_PIN                                              0x3d
#define UX_PCI_CFG_MIN_GNT                                              0x3e
#define UX_PCI_CFG_MAX_LATENCY                                          0x3f

#define UX_PCI_CFG_SBRN                                                 0x60
#define UX_PCI_CFG_FLADJ                                                0x61

/* Define DFU constants.  */
#define UX_SYSTEM_DFU_STATE_APP_IDLE                                    0
#define UX_SYSTEM_DFU_STATE_APP_DETACH                                  1
#define UX_SYSTEM_DFU_STATE_DFU_IDLE                                    2
#define UX_SYSTEM_DFU_STATE_DFU_DNLOAD_SYNC                             3
#define UX_SYSTEM_DFU_STATE_DFU_DNBUSY                                  4
#define UX_SYSTEM_DFU_STATE_DFU_DNLOAD_IDLE                             5
#define UX_SYSTEM_DFU_STATE_DFU_MANIFEST_SYNC                           6
#define UX_SYSTEM_DFU_STATE_DFU_MANIFEST                                7
#define UX_SYSTEM_DFU_STATE_DFU_MANIFEST_WAIT_RESET                     8
#define UX_SYSTEM_DFU_STATE_DFU_UPLOAD_IDLE                             9
#define UX_SYSTEM_DFU_STATE_DFU_ERROR                                   10

}
