#pragma once

#include <threadxx/dllist.hpp>
#include <threadxx/thread.hpp>
#include <threadxx/mutex.hpp>
#include <threadxx/semaphore.hpp>

#include <piradio/lmx2820.hpp>
#include <piradio/ltc2668.hpp>

extern "C" {
#include "gpio.h"
}


namespace piradio
{
  namespace hardware
  {
    struct unsupported_error : public std::exception
    {

    };

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

      virtual void tune_lmx(float freq) { throw unsupported_error(); }
      virtual void set_lmx_drive(uint8_t v) { throw unsupported_error(); }
      virtual void set_lmx_powerdown(bool) { throw unsupported_error(); }
      virtual void reprogram_lmx() { throw unsupported_error(); }
      virtual uint16_t lmx_read_reg(uint16_t) { throw unsupported_error(); }
      virtual void lmx_write_reg(uint16_t, uint16_t) { throw unsupported_error(); }

      virtual void set_I_voltage(float v) { throw unsupported_error(); }
      virtual void set_Q_voltage(float v) { throw unsupported_error(); }
      virtual void set_rx_filter(uint8_t v) { throw unsupported_error(); }
      virtual void set_tx_filter(uint8_t v) { throw unsupported_error(); }

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

      void tune_lmx(float freq) override { lmx.tune(freq); };
      void set_lmx_drive(uint8_t v) override { };
      void set_lmx_powerdown(bool b) override { lmx.set_powerdown(b); }
      void reprogram_lmx() override { lmx.reprogram(); }
      uint16_t lmx_read_reg(uint16_t r) override { uint16_t retval; return lmx.read_reg(r, &retval); return retval; }
      void lmx_write_reg(uint16_t r, uint16_t v) override { lmx.write_reg(r, v); }

      virtual bool configured() { return true; }

      virtual void initialize_gpios();

      virtual void setup_gpios() = 0;

      void setup_bank(GPIO_TypeDef *gpio, const std::vector<uint32_t> &pins, const std::vector<uint32_t> &set_pins);
    };

    class FR31CHHardware : public Gen1Hardware
    {
      LTC2668 ltc2668;

    public:
      FR31CHHardware() : Gen1Hardware(10e6) {}

      virtual void power_up();
      virtual void power_down();

      virtual void setup_gpios();

      virtual void restore_settings();

      void set_I_voltage(float v) override;
      void set_Q_voltage(float v) override;
      void set_rx_filter(uint8_t v) override;
      void set_tx_filter(uint8_t v) override;
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
