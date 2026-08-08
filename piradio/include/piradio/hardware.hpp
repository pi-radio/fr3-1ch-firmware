#pragma once

#include <threadxx/dllist.hpp>
#include <threadxx/thread.hpp>
#include <threadxx/mutex.hpp>
#include <threadxx/semaphore.hpp>

#include <piradio/lmx2820.hpp>

extern "C" {
#include "gpio.h"
}


namespace piradio
{
  namespace hardware
  {
    class Request
    {
      TXX::list::DLListEntry<Request> list_entry;

      virtual int process() = 0;

    public:
      Request() {}
    };

    class DeviceThread : public TXX::Thread<8192>
    {
      TXX::Mutex request_mutex;
      TXX::Semaphore request_sema;
      std::vector<Request *> requests;

    public:
      DeviceThread();

      virtual void main();
    };



    class PiRadioHardware
    {
    public:
      virtual bool configured() { return false; }
      virtual void initialize_gpios() = 0;

      virtual void power_up() = 0;
      virtual void power_down() = 0;

      virtual void restore_settings() = 0;

      virtual LMX::LMX2820 *get_lmx() { return nullptr; }
    };

    class UnconfiguredHardware : public PiRadioHardware
    {
    public:
      virtual void initialize_gpios() {};

      virtual void power_up() {};
      virtual void power_down() {};

      virtual void restore_settings() {};
    };

    class Gen1Hardware : public PiRadioHardware
    {
      DeviceThread device_thread;

    protected:
      LMX::LMX2820 lmx;

    public:
      Gen1Hardware(float osc_in) : lmx(osc_in) {
      }

      virtual bool configured() { return true; }

      virtual void initialize_gpios();

      virtual void setup_gpios() = 0;

      void setup_bank(GPIO_TypeDef *gpio, const std::vector<uint32_t> &pins, const std::vector<uint32_t> &set_pins);

      virtual LMX::LMX2820 *get_lmx() { return &lmx; }
    };

    class FR31CHHardware : public Gen1Hardware
    {
    public:
      FR31CHHardware() : Gen1Hardware(10e6) {}

      virtual void power_up();
      virtual void power_down();

      virtual void setup_gpios();

      virtual void restore_settings();
    };

    class OctoLOHardware : public Gen1Hardware
    {
    public:
      OctoLOHardware() : Gen1Hardware(100e6) {}

      virtual void power_up();
      virtual void power_down();

      virtual void setup_gpios();

      virtual void restore_settings();
    };
  }
}
