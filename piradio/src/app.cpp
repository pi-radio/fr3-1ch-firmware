#include <piradio/app.hpp>
#include <piradio/parser.hpp>
#include <consolexx/terminal.hpp>

#include "fr3_1ch_hw.h"

#include "main.h"

extern "C" {
#include "dcache.h"
#include "dts.h"
#include "flash.h"
#include "gpdma.h"
#include "gpio.h"
#include "icache.h"
#include "lptim.h"
#include "spi.h"
#include "ucpd.h"
#include "usbpd.h"
#include "usart.h"
}


#ifdef OCTOLO
static const int LMX_OSC_IN = 100e6;
#else
static const int LMX_OSC_IN = 10e6;
#endif


PiRadioApp::PiRadioApp() : lmx(LMX_OSC_IN),
    term(usb_serial),
    cmd_queue("App command queue")
{
}

void PiRadioApp::setup_clocks() {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV2;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_0);
}

void PiRadioApp::initialize_hardware() {
  MX_FLASH_Init();
  MX_GPIO_Init();
  MX_USART1_UART_Init();

  dbg::add_renderer(this);
  
  MX_GPDMA1_Init();
  MX_SPI4_Init();
  MX_UCPD1_Init();
  MX_DCACHE1_Init();
  MX_ICACHE_Init();
  MX_FLASH_Init();
  MX_DTS_Init();
  MX_LPTIM1_Init();
}

void PiRadioApp::pre_kernel()
{
  USBPD_PreInitOs();

  lmx.setup();
}

void PiRadioApp::tx_init()
{
  printf("FR3 1ch starting...\r\n");

  usb_serial.start();
  
  cmd_queue.create();

  term.startup();

  output_win = term.create<window>(8, 0, 8, 132);
  status_win = term.create<window>(20, 0, 1, 132);

  input_win = term.create<text_field>(17, 0, 1, 132);

  term.set_focus(input_win);

  input_win->set_callbacks(this);

  /*
  tx_timer_create(&status_timer,
      (char *)"Status Timer",
      status_update,
      (ULONG)status_win,
      1000,
      1000,
      TX_AUTO_ACTIVATE);
      */
  

  fr3_1ch_hw_init();
}

void PiRadioApp::app_main()
{
  parser::Parser p;
  
  tx_thread_sleep(1000);

  printf("Programming LMX...\r\n");
  lmx.program();

  while (true) {
    std::string *ps = (std::string *)cmd_queue.recv();

    p.set_line(*ps);
    
    delete ps;

    try {
      p.parse();
    } catch (const parser::SyntaxError &e) {
      printf("Syntax Error\n");
    } catch (const parser::GeneralError &e) {
      printf("%s\n", e.s.c_str());
    } catch (...) {
      printf("Caught unknown exception\n");             
    }
  }
}

void PiRadioApp::on_cr(const uint8_t *s, size_t l)
{
  uint32_t cmd = (uint32_t)new std::string((const char *)s, l);
  
  output_win->printf("%.*s\n", l, s);

  cmd_queue.send(cmd);
}

int PiRadioApp::render(const char *buffer, size_t size)
{
  HAL_UART_Transmit(&huart1, (const uint8_t *)buffer, size, 0xFFFF);
  return 0;
}

int PiRadioApp::writemsg(const char *buffer, size_t size)
{
  render(buffer, size);

  if (output_win) output_win->write(buffer, size);

  return size;
}

void PiRadioApp::status_update()
{
  int result;
  int32_t temp;

  result = HAL_DTS_GetTemperature(&hdts, &temp);

  if (result == TX_SUCCESS) {
    status_win->printf(position(0,0), "Temp: %dC", temp);
  } else {
    status_win->printf(position(0,0), "Temp: ERROR");
  }
}

void PiRadioApp::redraw()
{
  term.redraw();
}

void PiRadioApp::clear_output()
{
  output_win->clear();
}

PiRadioApp main_app;

USBSerial::USBSerial()
{

}

uint32_t USBSerial::on_attached()
{
  dbg::dbgout << "USB Attached" << std::endl;
  return UX_SUCCESS;
}

uint32_t USBSerial::on_removed()
{
  dbg::dbgout << "USB Removed" << std::endl;
  return UX_SUCCESS;
}

uint32_t USBSerial::on_connected()
{
  dbg::dbgout << "USB Connected" << std::endl;
  return UX_SUCCESS;
}

uint32_t USBSerial::on_disconnected()
{
  dbg::dbgout << "USB Disconnected" << std::endl;
  return UX_SUCCESS;
}

EXTERN_C int _write(int file, char *ptr, int len)
{
  return main_app.writemsg(ptr, len);
}

#if 0

EXTERN_C int terminal_send_command(int cmd)
{
  switch (cmd) {
  case TERMINAL_CMD_NOOP:
    return 0;
  case TERMINAL_CMD_REDRAW:
    term->enqueue_cmd(cmd);
    return 0;
  case TERMINAL_CMD_FLUSH:
    // has to go through the character stream to be sure
    // we flush to this point
    term->flush();
    return 0;
  case TERMINAL_CMD_CLEAR_OUTPUT:
    output_win->clear();
    return 0;
  default:
    return -1;
  }
}
#endif
