#ifndef UX_API_H
#define UX_API_H

/* Determine if a C++ compiler is being used.  If so, ensure that standard
   C is used to process the API information.  */

#ifdef   __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {

#endif



/* Include USBX port specific file.  */

#include "ux_port.h"



/* Define interrupts lockout constructs to protect the memory allocation/release which could happen
   under ISR in the device stack.  */


/* Define thread/timer related things.  */


/* Define basic USBX constants.  */

#if 0
#define UX_FEEDBACK_SIZE_FULL_SPEED                                     3   /* 10.10 format fits into 3 bytes.  */
#define UX_FEEDBACK_SIZE_HIGH_SPEED                                     4   /* 12.13 format fits into 4 bytes.  */
#endif


#if 0
#define UX_DEVICE_RESET                                                 0
#define UX_DEVICE_ATTACHED                                              1
#define UX_DEVICE_ADDRESSED                                             2
#define UX_DEVICE_CONFIGURED                                            3
#define UX_DEVICE_SUSPENDED                                             4
#define UX_DEVICE_RESUMED                                               5
#define UX_DEVICE_SELF_POWERED_STATE                                    6
#define UX_DEVICE_BUS_POWERED_STATE                                     7
#define UX_DEVICE_REMOTE_WAKEUP                                         8
#define UX_DEVICE_BUS_RESET_COMPLETED                                   9
#define UX_DEVICE_REMOVED                                               10
#define UX_DEVICE_FORCE_DISCONNECT                                      11

#define UX_ENDPOINT_RESET                                               0
#define UX_ENDPOINT_RUNNING                                             1
#define UX_ENDPOINT_HALTED                                              2

#define UX_DEVICE_DESCRIPTOR_ITEM                                       1u
#define UX_CONFIGURATION_DESCRIPTOR_ITEM                                2u
#define UX_STRING_DESCRIPTOR_ITEM                                       3u
#define UX_INTERFACE_DESCRIPTOR_ITEM                                    4u
#define UX_ENDPOINT_DESCRIPTOR_ITEM                                     5u
#define UX_DEVICE_QUALIFIER_DESCRIPTOR_ITEM                             6u
#define UX_OTHER_SPEED_DESCRIPTOR_ITEM                                  7u
#define UX_OTG_DESCRIPTOR_ITEM                                          9u
#define UX_INTERFACE_ASSOCIATION_DESCRIPTOR_ITEM                        11u
#define UX_BOS_DESCRIPTOR_ITEM                                          15u
#define UX_DEVICE_CAPABILITY_DESCRIPTOR_ITEM                            16u
#define UX_DFU_FUNCTIONAL_DESCRIPTOR_ITEM                               0x21u
#define UX_HUB_DESCRIPTOR_ITEM                                          0x29u
#endif

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


#ifndef UX_CONTROL_TRANSFER_TIMEOUT
#define UX_CONTROL_TRANSFER_TIMEOUT                                     10000
#endif

#ifndef UX_NON_CONTROL_TRANSFER_TIMEOUT
#define UX_NON_CONTROL_TRANSFER_TIMEOUT                                 50000
#endif
#define UX_PORT_ENABLE_WAIT                                             50
#define UX_DEVICE_ADDRESS_SET_WAIT                                      50
#define UX_HIGH_SPEED_DETECTION_HANDSHAKE_SUSPEND_WAIT                  200
#define UX_ENUMERATION_THREAD_WAIT                                      200


/* USB Billboard constants.  */

#define UX_CLASS_BILLBOARD_CLASS                                        0x11
#define UX_CLASS_BILLBOARD_SUBCLASS                                     0x00
#define UX_CLASS_BILLBOARD_PROTOCOL                                     0x00
#define UX_CLASS_BILLBOARD_DESCRIPTOR_BILLBOARD                         0x0D
#define UX_CLASS_BILLBOARD_DESCRIPTOR_ALTERNATE_MODE                    0x0F
#define UX_CLASS_BILLBOARD_MAX_NUM_ALT_MODE                             0x34

/* USBX 5.8 BACKWARD COMPATIBILITY DEFINITIONS. THESE DEFINITIONS ARE NOW OBSOLETE
   BUT DEFINED HERE FOR COMPATIBILITY REASONS.  */

#ifndef UX_CONTROL_TRANSFER_TIMEOUT_IN_MS
#define UX_CONTROL_TRANSFER_TIMEOUT_IN_MS                               10000
#endif

#ifndef UX_NON_CONTROL_TRANSFER_TIMEOUT_IN_MS
#define UX_NON_CONTROL_TRANSFER_TIMEOUT_IN_MS                           50000
#endif

#ifndef UX_PORT_ENABLE_WAIT_IN_MS
#define UX_PORT_ENABLE_WAIT_IN_MS                                       500
#endif

#ifndef UX_DEVICE_ADDRESS_SET_WAIT_IN_MS
#define UX_DEVICE_ADDRESS_SET_WAIT_IN_MS                                500
#endif

#ifndef UX_HIGH_SPEED_DETECTION_HANDSHAKE_SUSPEND_WAIT_IN_MS
#define UX_HIGH_SPEED_DETECTION_HANDSHAKE_SUSPEND_WAIT_IN_MS            2000
#endif

/* END OF 5.8 BACKWARD COMPATIBILITY DEFINITIONS. */




/* Host change callback events : _callback(event, *class, *instance)  */

#define UX_DEVICE_INSERTION                                             0x01u
#define UX_DEVICE_REMOVAL                                               0x02u
#define UX_HID_CLIENT_INSERTION                                         0x03u
#define UX_HID_CLIENT_REMOVAL                                           0x04u
#define UX_STORAGE_MEDIA_INSERTION                                      0x05u
#define UX_STORAGE_MEDIA_REMOVAL                                        0x06u

/* Host change callback events : _callback(event, NULL, *device_instance)  */

#define UX_DEVICE_CONNECTION                                            0x81u
#define UX_DEVICE_DISCONNECTION                                         0x82u

/* Host change callback events : _callback(event, NULL, NULL)  */

#define UX_STANDALONE_WAIT_BACKGROUND_TASK                              0x00u


/* Define USBX transfer request status constants.  */



/* Define USBX device power constants.  */

#define UX_DEVICE_BUS_POWERED                                           1u
#define UX_DEVICE_SELF_POWERED                                          2u
#define UX_MAX_SELF_POWER                                               (500u/2)
#define UX_MAX_BUS_POWER                                                (100u/2)
#define UX_CONFIGURATION_DEVICE_BUS_POWERED                             0x80u
#define UX_CONFIGURATION_DEVICE_SELF_POWERED                            0x40u
#define UX_STATUS_DEVICE_SELF_POWERED                                   1u

/* Define USBX OTG constants.  */

#define UX_OTG_BM_ATTRIBUTES                                            2u
#define UX_OTG_SRP_SUPPORT                                              1u
#define UX_OTG_HNP_SUPPORT                                              2u
#define UX_HCD_OTG_CAPABLE                                              1u
#define UX_DCD_OTG_CAPABLE                                              1u

#define UX_OTG_FEATURE_B_HNP_ENABLE                                     3u
#define UX_OTG_FEATURE_A_HNP_SUPPORT                                    4u
#define UX_OTG_FEATURE_A_ALT_HNP_SUPPORT                                5u
#define UX_OTG_STATUS_SELECTOR                                          0xF000u
#define UX_OTG_HOST_REQUEST_FLAG                                        0x01u

#define UX_OTG_IDLE                                                     0u
#define UX_OTG_IDLE_TO_HOST                                             1u
#define UX_OTG_IDLE_TO_SLAVE                                            2u
#define UX_OTG_HOST_TO_IDLE                                             3u
#define UX_OTG_HOST_TO_SLAVE                                            4u
#define UX_OTG_SLAVE_TO_IDLE                                            5u
#define UX_OTG_SLAVE_TO_HOST                                            6u
#define UX_OTG_SLAVE_SRP                                                7u

#define UX_OTG_MODE_IDLE                                                0u
#define UX_OTG_MODE_SLAVE                                               1u
#define UX_OTG_MODE_HOST                                                2u

#define UX_OTG_DEVICE_IDLE                                              0u
#define UX_OTG_DEVICE_A                                                 1u
#define UX_OTG_DEVICE_B                                                 2u

#define UX_OTG_VBUS_IDLE                                                0u
#define UX_OTG_VBUS_ON                                                  1u
#define UX_OTG_VBUS_OFF                                                 2u


#define UX_OTG_HNP_THREAD_SLEEP_TIME                                    (2 * UX_PERIODIC_RATE)

/* Define USBX device speed constants.  */

#define UX_DEFAULT_HS_MPS                                               64
#define UX_DEFAULT_MPS                                                  8

#define UX_LOW_SPEED_DEVICE                                             0
#define UX_FULL_SPEED_DEVICE                                            1
#define UX_HIGH_SPEED_DEVICE                                            2


/* Define USBX generic port status constants.  */

#define UX_PS_CCS                                                       0x01u
#define UX_PS_CPE                                                       0x01u
#define UX_PS_PES                                                       0x02u
#define UX_PS_PSS                                                       0x04u
#define UX_PS_POCI                                                      0x08u
#define UX_PS_PRS                                                       0x10u
#define UX_PS_PPS                                                       0x20u
#define UX_PS_DS_LS                                                     0x00u
#define UX_PS_DS_FS                                                     0x40u
#define UX_PS_DS_HS                                                     0x80u

#define UX_PS_DS                                                        6u


/* Define USBX generic state machine constants.  */

#define UX_STATE_RESET                                                  0u
#define UX_STATE_EXIT                                                   1u
#define UX_STATE_IDLE                                                   2u
#define UX_STATE_ERROR                                                  3u
#define UX_STATE_NEXT                                                   4u
#define UX_STATE_WAIT                                                   5u
#define UX_STATE_LOCK                                                   6u
#define UX_STATE_BUSY                                                   6u
#define UX_STATE_STEP                                                   0x20u
#define UX_STATE_STACK_STEP                                             0x20u
#define UX_STATE_DRIVER_STEP                                            0x40u
#define UX_STATE_CLASS_STEP                                             0x60u
#define UX_STATE_APP_STEP                                               0x80u

#define UX_STATE_IS_RESET(s)                            (!(s))                          /* Reset state  */
#define UX_STATE_IS_FATAL(s)                            ((s) && (s) < UX_STATE_ERROR)   /* Fatal, break all pending loops.  */
#define UX_STATE_IS_ERROR(s)                            ((s) == UX_STATE_ERROR)         /* General error  */
#define UX_STATE_IS_BUSY(s)                             ((s) >= UX_STATE_WAIT)          /* Locked or pending  */
#define UX_STATE_IS_LOCKED(s)                           ((s) >= UX_STATE_LOCK)          /* Locked but not pendint  */


/* Define USBX Error Code constants. The following format describes
   their meaning:

        0x00    : Success
        0x0x    : State machine return codes
        0x1x    : Configuration errors
        0x2x    : USB transport errors
        0x3x    : USB controller errors
        0x4x    : USB topology errors
        0x5x    : USB API errors
        0x6x    : USB Generic Class errors
        0x7x    : USB HID Class errors
        0x8x    : USB Audio Class errors
        0x9x    : USB CDC-ECM Class errors
        ...
        0xfx    : General errors
*/

#define UX_SUCCESS                                                      0

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

/* Define basic class constants.  */

#define UX_HOST_CLASS_PRINTER_NAME_LENGTH                               64


#if 0

struct UX_ENDPOINT;


/* Define USBX 2.0 TT Instance structure.  */

typedef struct UX_HUB_TT_STRUCT
{

    ULONG           ux_hub_tt_port_mapping;
    ULONG           ux_hub_tt_max_bandwidth;
} UX_HUB_TT;


/* Define USBX Class calling command structure.  */

struct UX_HOST_CLASS_COMMAND
{

    UINT            ux_host_class_command_request;
    VOID            *ux_host_class_command_container;
    VOID            *ux_host_class_command_instance;
    UINT            ux_host_class_command_usage;
    UINT            ux_host_class_command_pid;
    UINT            ux_host_class_command_vid;
    UINT            ux_host_class_command_class;
    UINT            ux_host_class_command_subclass;
    UINT            ux_host_class_command_protocol;
    UINT            ux_host_class_command_iad_class;
    UINT            ux_host_class_command_iad_subclass;
    UINT            ux_host_class_command_iad_protocol;

    UX_HOST_CLASS
                    *ux_host_class_command_class_ptr;
};


/* Define USBX Class container structure.  */

struct UX_HOST_CLASS
{

#if defined(UX_NAME_REFERENCED_BY_POINTER)
    const UCHAR     *ux_host_class_name;
#else
    UCHAR           ux_host_class_name[UX_MAX_CLASS_NAME_LENGTH + 1]; /* "+1" for string null-terminator */
#endif

#if defined(UX_HOST_STANDALONE)
    UINT            (*ux_host_class_task_function)(UX_HOST_CLASS *);
#endif

    UINT            ux_host_class_status;
    UINT            (*ux_host_class_entry_function) (UX_HOST_CLASS_COMMAND *);
    VOID            *ux_host_class_first_instance;
    VOID            *ux_host_class_client;
    VOID            *ux_host_class_media;
    VOID            *ux_host_class_ext;

};


/* Define USBX transfer request structure.  */

#if 0
struct UX_TRANSFER
{

    ULONG           ux_transfer_request_status;
    UX_ENDPOINT     *ux_transfer_request_endpoint;
    UCHAR *         ux_transfer_request_data_pointer;
    ULONG           ux_transfer_request_requested_length;
    ULONG           ux_transfer_request_actual_length;
    UINT            ux_transfer_request_type;
    UINT            ux_transfer_request_function;
    UINT            ux_transfer_request_value;
    UINT            ux_transfer_request_index;
    VOID            (*ux_transfer_request_completion_function) (UX_TRANSFER *);
    VOID            *ux_transfer_request_class_instance;
    ULONG           ux_transfer_request_maximum_length;
    ULONG           ux_transfer_request_timeout_value;
    UINT            ux_transfer_request_completion_code;
    ULONG           ux_transfer_request_packet_length;
    struct UX_TRANSFER_STRUCT
                    *ux_transfer_request_next_transfer_request;
    VOID            *ux_transfer_request_user_specific;
    UX_SEMAPHORE    ux_transfer_request_semaphore;
    UX_THREAD       *ux_transfer_request_thread_pending;
};
#endif

/* Define USBX Endpoint Descriptor structure.  */

struct UX_ENDPOINT_DESCRIPTOR
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bEndpointAddress;
    UCHAR           bmAttributes;
    USHORT          wMaxPacketSize;
    UCHAR           bInterval;
    UCHAR           _align_size[1];
} ;

#define UX_ENDPOINT_DESCRIPTOR_ENTRIES                                  6
#define UX_ENDPOINT_DESCRIPTOR_LENGTH                                   7


/* Define USBX Device Descriptor structure.  */

typedef struct UX_DEVICE_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    USHORT          bcdUSB;
    UCHAR           bDeviceClass;
    UCHAR           bDeviceSubClass;
    UCHAR           bDeviceProtocol;
    UCHAR           bMaxPacketSize0;
    USHORT          idVendor;
    USHORT          idProduct;
    USHORT          bcdDevice;
    UCHAR           iManufacturer;
    UCHAR           iProduct;
    UCHAR           iSerialNumber;
    UCHAR           bNumConfigurations;
    UCHAR           _align_size[2];
} UX_DEVICE_DESCRIPTOR;

#define UX_DEVICE_DESCRIPTOR_ENTRIES                                    14
#define UX_DEVICE_DESCRIPTOR_LENGTH                                     18


/* Define USBX Device Qualifier Descriptor structure.  */

typedef struct UX_DEVICE_QUALIFIER_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    USHORT          bcdUSB;
    UCHAR           bDeviceClass;
    UCHAR           bDeviceSubClass;
    UCHAR           bDeviceProtocol;
    UCHAR           bMaxPacketSize0;
    UCHAR           bNumConfigurations;
    UCHAR           bReserved;
    UCHAR           _align_size[2];
} UX_DEVICE_QUALIFIER_DESCRIPTOR;

#define UX_DEVICE_QUALIFIER_DESCRIPTOR_ENTRIES                          9
#define UX_DEVICE_QUALIFIER_DESCRIPTOR_LENGTH                           10


/* Define USBX Other Speed Descriptor structure.  */

typedef struct UX_OTHER_SPEED_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    USHORT          wTotalLength;
    UCHAR           bNumInterfaces;
    UCHAR           bConfigurationValue;
    UCHAR           iConfiguration;
    UCHAR           bmAttributes;
    ULONG           MaxPower;
} UX_OTHER_SPEED_DESCRIPTOR;

#define UX_OTHER_SPEED_DESCRIPTOR_ENTRIES                               8
#define UX_OTHER_SPEED_DESCRIPTOR_LENGTH                                9


/* Define USBX OTG Descriptor structure.  */

typedef struct UX_OTG_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bmAttributes;
    UCHAR           _align_bcdOTG[1];
    USHORT          bcdOTG;
    UCHAR           _algin_size[2];
} UX_OTG_DESCRIPTOR;

#define UX_OTG_DESCRIPTOR_ENTRIES                          4
#define UX_OTG_DESCRIPTOR_LENGTH                           5


/* Define USBX Interface Association Descriptor structure.  */

typedef struct UX_INTERFACE_ASSOCIATION_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bFirstInterface;
    UCHAR           bInterfaceCount;
    UCHAR           bFunctionClass;
    UCHAR           bFunctionSubClass;
    UCHAR           bFunctionProtocol;
    UCHAR           iFunction;
} UX_INTERFACE_ASSOCIATION_DESCRIPTOR;

#define UX_INTERFACE_ASSOCIATION_DESCRIPTOR_ENTRIES         8
#define UX_INTERFACE_ASSOCIATION_DESCRIPTOR_LENGTH          8


/* Define USBX Device Container structure.  */


#if defined(UX_HOST_STANDALONE)
#define UX_DEVICE_FLAG_LOCK                     0x01u
#define UX_DEVICE_FLAG_RESET                    0x02u
#define UX_DEVICE_FLAG_ENUM                     0x04u
#define UX_DEVICE_FLAG_PROTECT                  0x08u
#endif

#if UX_MAX_HCD > 1
#define UX_DEVICE_HCD_GET(d)                    ((d)->ux_device_hcd)
#define UX_DEVICE_HCD_SET(d,h)                  do { (d)->ux_device_hcd = (h); } while(0)
#define UX_DEVICE_HCD_MATCH(d,h)                ((d)->ux_device_hcd == (h))
#else
#define UX_DEVICE_HCD_GET(d)                    (_ux_system_host->ux_system_host_hcd_array)
#define UX_DEVICE_HCD_SET(d,h)
#define UX_DEVICE_HCD_MATCH(d,h)                (_ux_system_host->ux_system_host_hcd_array == (h))
#endif


#if UX_MAX_DEVICES > 1
#define UX_DEVICE_PARENT_GET(d)                 ((d)->ux_device_parent)
#define UX_DEVICE_PARENT_SET(d,p)               do { (d)->ux_device_parent = (p); } while(0)
#define UX_DEVICE_PARENT_MATCH(d,p)             ((d)->ux_device_parent == (p))
#define UX_DEVICE_PARENT_IS_HUB(d)              ((d)->ux_device_parent != UX_NULL)
#define UX_DEVICE_PARENT_IS_ROOTHUB(d)          ((d)->ux_device_parent == UX_NULL)
#define UX_DEVICE_MAX_POWER_GET(d)              ((d)->ux_device_max_power)
#define UX_DEVICE_MAX_POWER_SET(d,p)            do {(d)->ux_device_max_power = (p); } while(0)
#else
#define UX_DEVICE_PARENT_GET(d)                 (UX_NULL)
#define UX_DEVICE_PARENT_SET(d,p)               UX_PARAMETER_NOT_USED(p)
#define UX_DEVICE_PARENT_MATCH(d,p)             ((p) == UX_NULL)
#define UX_DEVICE_PARENT_IS_HUB(d)              (UX_FALSE)
#define UX_DEVICE_PARENT_IS_ROOTHUB(d)          (UX_TRUE)
#define UX_DEVICE_MAX_POWER_GET(d)              (UX_MAX_SELF_POWER)
#define UX_DEVICE_MAX_POWER_SET(d,p)            UX_PARAMETER_NOT_USED(p)
#endif
#define UX_DEVICE_PORT_LOCATION_GET(d)          ((d)->ux_device_port_location)
#define UX_DEVICE_PORT_LOCATION_SET(d,l)        do { (d)->ux_device_port_location = (l); } while(0)
#define UX_DEVICE_PORT_LOCATION_MATCH(d,l)      ((d)->ux_device_port_location == (l))


/* Define USBX Configuration Descriptor structure.  */

typedef struct UX_CONFIGURATION_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    USHORT          wTotalLength;
    UCHAR           bNumInterfaces;
    UCHAR           bConfigurationValue;
    UCHAR           iConfiguration;
    UCHAR           bmAttributes;
    ULONG           MaxPower;
} UX_CONFIGURATION_DESCRIPTOR;


#define UX_CONFIGURATION_DESCRIPTOR_ENTRIES                             8
#define UX_CONFIGURATION_DESCRIPTOR_LENGTH                              9


/* Define USBX Configuration Container structure.  */

struct UX_CONFIGURATION
{

    ULONG           ux_configuration_handle;
    ULONG           ux_configuration_state;
    ULONG           ux_configuration_otg_capabilities;
    struct UX_CONFIGURATION_DESCRIPTOR_STRUCT
                    ux_configuration_descriptor;
    UX_INTERFACE
                    *ux_configuration_first_interface;
    UX_CONFIGURATION
                    *ux_configuration_next_configuration;
    struct UX_DEVICE_STRUCT
                    *ux_configuration_device;
    ULONG           ux_configuration_iad_class;
    ULONG           ux_configuration_iad_subclass;
    ULONG           ux_configuration_iad_protocol;
};

#define UX_HOST_STACK_CONFIGURATION_INSTANCE_CREATE_ALL     0 /* Default: all things created.  */
#define UX_HOST_STACK_CONFIGURATION_INSTANCE_CREATE_OWNED   1 /* Owned: class owned things created.  */
#ifndef UX_HOST_STACK_CONFIGURATION_INSTANCE_CREATE_CONTROL
#define UX_HOST_STACK_CONFIGURATION_INSTANCE_CREATE_CONTROL UX_HOST_STACK_CONFIGURATION_INSTANCE_CREATE_ALL
#endif


/* Define USBX Interface Descriptor structure.  */

struct UX_INTERFACE_DESCRIPTOR
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bInterfaceNumber;
    UCHAR           bAlternateSetting;
    UCHAR           bNumEndpoints;
    UCHAR           bInterfaceClass;
    UCHAR           bInterfaceSubClass;
    UCHAR           bInterfaceProtocol;
    UCHAR           iInterface;
    UCHAR           _align_size[3];
};

#define UX_INTERFACE_DESCRIPTOR_ENTRIES                                 9
#define UX_INTERFACE_DESCRIPTOR_LENGTH                                  9


/* Define USBX Interface Container structure.  */

struct UX_INTERFACE
{

    ULONG           ux_interface_handle;
    ULONG           ux_interface_state;
    UINT            ux_interface_current_alternate_setting;
    UX_INTERFACE_DESCRIPTOR
                    ux_interface_descriptor;
    UX_HOST_CLASS
                    *ux_interface_class;
    VOID            *ux_interface_class_instance;
    UX_ENDPOINT
                    *ux_interface_first_endpoint;
    UX_INTERFACE
                    *ux_interface_next_interface;
    UX_CONFIGURATION
                    *ux_interface_configuration;
    ULONG           ux_interface_iad_class;
    ULONG           ux_interface_iad_subclass;
    ULONG           ux_interface_iad_protocol;

};


/* Define USBX String Descriptor structure.  */

typedef struct UX_STRING_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bString[2];
} UX_STRING_DESCRIPTOR;

#define UX_STRING_DESCRIPTOR_ENTRIES                                    3
#define UX_STRING_DESCRIPTOR_LENGTH                                     4


/* Define USBX BOS Descriptor structure.  */

typedef struct UX_BOS_DESCRIPTOR_STRUCT
{
    UCHAR           bLength;
    UCHAR           bDescriptorType;
    USHORT          wTotalLength;
    UCHAR           bNumDeviceCaps;
    UCHAR           _align_size[3];
} UX_BOS_DESCRIPTOR;

#define UX_BOS_DESCRIPTOR_ENTRIES                                       4
#define UX_BOS_DESCRIPTOR_LENGTH                                        5


/* Define USBX USB 2.0 Descriptor structure.  */

typedef struct UX_USB_2_0_EXTENSION_DESCRIPTOR_STRUCT
{
    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDevCapabilityType;
    UCHAR           _align_bmAttributes[1];
    ULONG           bmAttributes;
} UX_USB_2_0_EXTENSION_DESCRIPTOR;

#define UX_USB_2_0_EXTENSION_DESCRIPTOR_ENTRIES                         4
#define UX_USB_2_0_EXTENSION_DESCRIPTOR_LENGTH                          7


/* Define USBX Container ID Descriptor structure.  */

typedef struct UX_CONTAINER_ID_DESCRIPTOR_STRUCT
{
    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDevCapabilityType;
    UCHAR           bReserved;
    ULONG           ContainerID[4];
} UX_CONTAINER_ID_DESCRIPTOR;

#define UX_CONTAINER_ID_DESCRIPTOR_ENTRIES                              8
#define UX_CONTAINER_ID_DESCRIPTOR_LENGTH                               20


/* Define USBX DFU functional descriptor.  */

typedef struct UX_DFU_FUNCTIONAL_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bmAttributes;
    UCHAR           _align_wDetachTimeOut[1];
    USHORT          wDetachTimeOut;
    USHORT          wTransferSize;
    USHORT          bcdDFUVersion;
    UCHAR           _align_size[2];
} UX_DFU_FUNCTIONAL_DESCRIPTOR;

#define UX_DFU_FUNCTIONAL_DESCRIPTOR_ENTRIES                            6
#define UX_DFU_FUNCTIONAL_DESCRIPTOR_LENGTH                             9


/* Define USBX Host Controller structure.  */
struct UX_HCD
{

#if defined(UX_NAME_REFERENCED_BY_POINTER)
    const UCHAR     *ux_hcd_name;
#else
    UCHAR           ux_hcd_name[UX_MAX_HCD_NAME_LENGTH + 1]; /* "+1" for string null-terminator */
#endif

    UINT            ux_hcd_status;
    UINT            ux_hcd_controller_type;
    UINT            ux_hcd_irq;
    UINT            ux_hcd_nb_root_hubs;
    UINT            ux_hcd_root_hub_signal[UX_MAX_ROOTHUB_PORT];
    UINT            ux_hcd_nb_devices;
    UINT            ux_hcd_thread_signal;
    ULONG           ux_hcd_rh_device_connection;
    ULONG           ux_hcd_io;
    UINT            (*ux_hcd_entry_function) (UX_HCD *, UINT, VOID *);
    void            *ux_hcd_controller_hardware;

#if defined(UX_OTG_SUPPORT)
    UINT            ux_hcd_otg_capabilities;
#endif

#if UX_MAX_DEVICES > 1
    UCHAR           ux_hcd_address[16];
    UINT            ux_hcd_power_switch;
    ULONG           ux_hcd_available_bandwidth;
    ULONG           ux_hcd_version;
#endif

#if defined(UX_HOST_STANDALONE)
    ULONG           ux_hcd_flags;
#endif
};


/* Define USBX Device Transfer Request structure.  */









/* Define USBX Device Controller structure.  */



/* Define USBX Device Class Command container structure.  */






#define UX_UCHAR_POINTER_ADD(a,b)                       (((UCHAR *) (a)) + ((UINT) (b)))
#define UX_UCHAR_POINTER_SUB(a,b)                       (((UCHAR *) (a)) - ((UINT) (b)))
#define UX_UCHAR_POINTER_DIF(a,b)                       ((ULONG)(((UCHAR *) (a)) - ((UCHAR *) (b))))
#define UX_ULONG_POINTER_ADD(a,b)                       (((ULONG *) (a)) + ((UINT) (b)))
#define UX_ULONG_POINTER_SUB(a,b)                       (((ULONG *) (a)) - ((UINT) (b)))
#define UX_ULONG_POINTER_DIF(a,b)                       ((ULONG)(((ULONG *) (a)) - ((ULONG *) (b))))
#define UX_POINTER_TO_ULONG_CONVERT(a)                  ((ULONG) ((VOID *) (a)))
#define UX_ULONG_TO_POINTER_CONVERT(a)                  ((VOID *) ((ULONG) (a)))
#define UX_POINTER_TO_ALIGN_TYPE_CONVERT(a)             ((ALIGN_TYPE) ((VOID *) (a)))
#define UX_ALIGN_TYPE_TO_POINTER_CONVERT(a)             ((VOID *) ((ALIGN_TYPE) (a)))
#define UX_LOOP_FOREVER                                 ((UINT) 1)
#define UX_INDIRECT_VOID_TO_UCHAR_POINTER_CONVERT(a)    ((UCHAR **) ((VOID *) (a)))
#define UX_UCHAR_TO_INDIRECT_UCHAR_POINTER_CONVERT(a)   ((UCHAR **) ((VOID *) (a)))
#define UX_VOID_TO_UCHAR_POINTER_CONVERT(a)             ((UCHAR *) ((VOID *) (a)))
#define UX_VOID_TO_INDIRECT_UCHAR_POINTER_CONVERT(a)    ((UCHAR **) ((VOID *) (a)))
#define UX_VOID_TO_BYTE_POOL_POINTER_CONVERT(a)         ((UX_MEMORY_BYTE_POOL *) ((VOID *) (a)))
#define UX_BYTE_POOL_TO_UCHAR_POINTER_CONVERT(a)        ((UCHAR *) ((VOID *) (a)))
#ifndef UX_UCHAR_TO_ALIGN_TYPE_POINTER_CONVERT
#define UX_UCHAR_TO_ALIGN_TYPE_POINTER_CONVERT(a)       ((ALIGN_TYPE *) ((VOID *) (a)))
#endif
#define UX_UCHAR_TO_INDIRECT_BYTE_POOL_POINTER(a)       ((UX_MEMORY_BYTE_POOL **) ((VOID *) (a)))
#define UX_MEMORY_BLOCK_HEADER_SIZE                     (sizeof(UCHAR *) + sizeof(ALIGN_TYPE))

#ifndef UX_BYTE_BLOCK_FREE
#define UX_BYTE_BLOCK_FREE                              ((ULONG) 0xFFFFEEEEUL)
#endif

#ifndef UX_BYTE_BLOCK_MIN
#define UX_BYTE_BLOCK_MIN                               ((ULONG) 20)
#endif

/* Define USBX Memory Management structure.  */

typedef struct UX_MEMORY_BYTE_POOL_STRUCT
{

    /* Define the number of available bytes in the pool.  */
    ULONG           ux_byte_pool_available;

    /* Define the number of fragments in the pool.  */
    UINT            ux_byte_pool_fragments;

    /* Define the search pointer used for initial searching for memory in a byte pool.  */
    UCHAR           *ux_byte_pool_search;

    /* Save the start address of the byte pool's memory area.  */
    UCHAR           *ux_byte_pool_start;

    /* Save the byte pool's size in bytes.  */
    ULONG           ux_byte_pool_size;

#ifdef UX_ENABLE_MEMORY_STATISTICS
    ALIGN_TYPE      ux_byte_pool_min_free;
    ULONG           ux_byte_pool_alloc_count;
    ULONG           ux_byte_pool_alloc_total;
    ULONG           ux_byte_pool_alloc_max_count;
    ULONG           ux_byte_pool_alloc_max_total;
#endif
} UX_MEMORY_BYTE_POOL;

#define UX_MEMORY_BYTE_POOL_REGULAR 0
#define UX_MEMORY_BYTE_POOL_CACHE_SAFE 1
#define UX_MEMORY_BYTE_POOL_NUM 2


/* Define the system API mappings based on the error checking
   selected by the user.  Note: this section is only applicable to
   application source code, hence the conditional that turns off this
   stuff when the include file is processed by the ThreadX source. */

/* Include USBX utility and system file.  */


#endif

/* Determine if a C++ compiler is being used.  If so, complete the standard
   C conditional started above.  */
#ifdef   __cplusplus
        }
#endif


#endif


