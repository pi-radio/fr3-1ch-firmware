#include <halxx/stm32h563.hpp>
#include <threadxx/queue.hpp>
#include <threadxx/app.hpp>
#include <consolexx/terminal.hpp>
#include <usbxx/usbxx.hpp>
#include <threadxx/usbpdxx.hpp>
#include "lmx.h"

class USBSerial : public USBXX::CDCACM
{
public:
  USBSerial();

  uint32_t on_attached() override;
  uint32_t on_removed() override;

  uint32_t on_connected() override;
  uint32_t on_disconnected() override;
};


class PiRadioApp : public TXX::App<halxx::STM32H563>,
		   public dbg::renderer,
                   public text_field_callbacks
{
  LMX::LMX2820 lmx;
  TXX::Queue<1, 16> cmd_queue;
  USBSerial usb_serial;
  USBPD usbpd;
  terminal term;

  window *output_win = NULL;
  window *status_win = NULL;
  text_field *input_win = NULL;

  TX_TIMER status_timer;
  
public:
  PiRadioApp();

  void setup_clocks() override;
  void initialize_hardware() override;
  void pre_kernel() override;
  void tx_init() override;
  void app_main() override;
  
  LMX::LMX2820 &get_lmx() { return lmx; }

  int render(const char *buffer, size_t size) override;

  void redraw();
  void clear_output();
  
  int writemsg(const char *buffer, size_t size);

  void beep() override { term.beep(); }

  void on_cr(const uint8_t *s, size_t l) override;

  void status_update();
};

extern PiRadioApp main_app;
