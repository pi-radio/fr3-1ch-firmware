#include <halxx/stm32h563.hpp>
#include <threadxx/queue.hpp>
#include <threadxx/app.hpp>
#include <consolexx/adapter.hpp>
#include <consolexx/window.hpp>
#include <consolexx/text_field.hpp>
#include <usbxx/usbxx.hpp>
#include <threadxx/usbpdxx.hpp>
#include <piradio/hardware.hpp>
#include <piradio/parser.hpp>

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
       public consolexx::termobj
{
  TXX::objqueue<parser::Command> cmd_queue;
  USBSerial usb_serial;

  consolexx::usb_io usb_io;

  USBPD usbpd;
  consolexx::terminal_adapter term;

  //consolexx::cooked_terminal term;

  consolexx::window *output_win = NULL;
  consolexx::window *status_win = NULL;
  consolexx::text_field *input_win = NULL;

  TX_TIMER status_timer;

  piradio::hardware::PiRadioHardware *hardware = NULL;

  struct {
    std::string model;
    int revision;
    std::string serial;
  } board;

public:
  PiRadioApp();

  void initialize_gpios();

  void setup_clocks() override;
  void setup_memory() override;
  void setup_tick() override;
  void setup_debug() override;
  void initialize_hardware() override;
  void pre_kernel() override;
  void tx_init() override;
  void app_main() override;
  
  piradio::hardware::PiRadioHardware *get_hardware() { return hardware; }

  int render(const char *buffer, size_t size) override;

  void redraw();
  void clear_output();
  
  int writemsg(const char *buffer, size_t size);

  //void beep() override { term.beep(); }

  void on_event(const consolexx::evt_ptr &evt) override;

  void status_update();

  void set_console_mode(consolexx::terminal_adapter::TerminalMode mode) { term.set_mode(mode); }
};

extern PiRadioApp main_app;
