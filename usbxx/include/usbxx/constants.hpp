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


}
