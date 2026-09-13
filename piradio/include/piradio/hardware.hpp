#pragma once

#include <memory>

#include <threadxx/dllist.hpp>
#include <threadxx/thread.hpp>
#include <threadxx/mutex.hpp>
#include <threadxx/semaphore.hpp>
#include <threadxx/queue.hpp>
#include <threadxx/config_data.hpp>


#include <piradio/config.hpp>
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
      friend class DeviceThread;

    protected:
      TXX::Semaphore sema;
      int result;

      virtual int process() = 0;

      void do_process()
      {
        result = process();

        sema.put();
      }

    public:
      using ptr = std::shared_ptr<Request>;

      Request() : sema("request sem"), result(0) {}
    };

    class DeviceThread : public TXX::Thread<8192>
    {
      TXX::queue<Request::ptr> request_queue;

    public:
      DeviceThread();

      virtual void main();

      int process(Request::ptr);

      static DeviceThread hw_thread;
    };

    enum class SPITarget
    {
      LMX,
      LTC2668
    };

    class SPIRequest : public Request
    {
      SPITarget target;
      int len;
      uint32_t v;

      int process() override;

    public:
      using ptr = std::shared_ptr<SPIRequest>;

      SPIRequest(SPITarget _target, size_t _len, uint32_t _v);

      static int transfer(SPITarget _target, size_t _len, uint32_t &_v)
      {
        auto r = std::make_shared<SPIRequest>(_target, _len, _v);

        auto retval = DeviceThread::hw_thread.process(r);

        _v = r->v;

        return retval;
      }
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

      virtual double get_LO() { throw unsupported_error(); }

      virtual void set_lmx_drive(uint8_t v) { throw unsupported_error(); }
      virtual void set_lmx_powerdown(bool) { throw unsupported_error(); }
      virtual void setup_lmx() { throw unsupported_error(); }
      virtual void reprogram_lmx() { throw unsupported_error(); }
      virtual uint16_t lmx_read_reg(uint16_t) { throw unsupported_error(); }
      virtual void lmx_write_reg(uint16_t, uint16_t) { throw unsupported_error(); }

      virtual float get_I_voltage() { throw unsupported_error(); }
      virtual float get_Q_voltage() { throw unsupported_error(); }


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

      double get_LO() override { return lmx.get_fOUTA(); }
      void tune_lmx(float freq) override { lmx.tune(freq); };
      void set_lmx_drive(uint8_t v) override { lmx.set_drive(v); };
      void set_lmx_powerdown(bool b) override { lmx.set_powerdown(b); }
      void setup_lmx() override { lmx.setup(); }
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
      float i_voltage;
      float q_voltage;
      LTC2668 ltc2668;

      void save_IQV();

      void _do_set_i_voltage();
      void _do_set_q_voltage();

    public:
      FR31CHHardware() : Gen1Hardware(10e6), i_voltage(0), q_voltage(0) {}

      virtual void power_up();
      virtual void power_down();

      virtual void setup_gpios();

      virtual void restore_settings();

      float get_I_voltage() override { return i_voltage; }
      float get_Q_voltage() override { return q_voltage; }

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
