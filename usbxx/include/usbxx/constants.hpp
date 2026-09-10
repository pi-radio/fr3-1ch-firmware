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
}
