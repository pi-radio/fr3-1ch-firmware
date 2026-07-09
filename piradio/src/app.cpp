#include <piradio/app.hpp>
#include <piradio/parser.hpp>
#include <piradio/config.hpp>
#include <threadxx/config_data.hpp>
#include <consolexx/terminal.hpp>
#include <halxx/fault.hpp>
#include <stm32h5/flash.hpp>

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


using namespace piradio::config;
using namespace TXX::config_data;

PiRadioApp::PiRadioApp() : lmx(10e6),
    term(usb_serial),
    cmd_queue("App command queue")
{
  TXX::config_data::registry.register_tlv<board_model>();
  TXX::config_data::registry.register_tlv<board_serial>();
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

void PiRadioApp::setup_memory() {
  MPU_Region_InitTypeDef MPU_InitStruct = {0};
  MPU_Attributes_InitTypeDef MPU_AttributesInit = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Attribute 0 and the memory to be protected
   */
  MPU_AttributesInit.Number = MPU_ATTRIBUTES_NUMBER0;
  MPU_AttributesInit.Attributes = 0;

  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);


  /** Initializes and configures the Region 0 and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x09000000;
  MPU_InitStruct.LimitAddress = 0x09017FFF;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Attribute 0 and the memory to be protected
  */
  MPU_AttributesInit.Number = MPU_ATTRIBUTES_NUMBER0;
  MPU_AttributesInit.Attributes = INNER_OUTER(MPU_WRITE_THROUGH|MPU_TRANSIENT
                              |MPU_NO_ALLOCATE);

  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

  flash::Flash::Init();
}

void PiRadioApp::setup_debug()
{
  MX_USART1_UART_Init();

  dbg::add_renderer(this);
}


void PiRadioApp::initialize_hardware() {
  try {
    auto bm = config.get<board_model>();

    board.model.append((char *)bm->model, bm->length);
    board.revision = bm->revision;
  } catch(...) {
    dbg::dbgout << "Board model not found" << std::endl;
  }

  initialize_gpios();

  
  MX_GPDMA1_Init();
  MX_SPI4_Init();
  MX_UCPD1_Init();
  MX_DCACHE1_Init();
  MX_ICACHE_Init();
  MX_DTS_Init();
  MX_LPTIM1_Init();
}

void setup_bank(GPIO_TypeDef *gpio, vector<uint32_t> &pins, vector<uint32_t> &set_pins)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  uint32_t pin_mask = 0,
    set_mask = 0;
  
  for(auto &i: pins) {
    pins_mask |= (1 << i);
  }
  
  GPIO_InitStruct.Pin = pins_mask;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  HAL_GPIO_Init(gpio, &GPIO_InitStruct);

  for(auto &i: set_pins) {
    set_mask |= (1 << i);
  }
  
  HAL_GPIO_WritePin(gpio, pins_mask & ~set_mask, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(gpio, pins_mask & set_mask, GPIO_PIN_SET);
}

/*
  FR3 Single channel GPIOs
  
    // 3. Configure the TX channel
  // a. Set IQ Swap 0: Upper side-band. 1: Lower side-band
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
  
  // b. Set the filter. Bypass. LLXXXX. Configure later through Serial.
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
*/

void PiRadioApp::initialize_gpios()
{
  MX_GPIO_Init();


  if (board.model == "OctoLO" || board.model == "FR3 1CH") {
      __HAL_RCC_GPIOA_CLK_ENABLE();
      __HAL_RCC_GPIOB_CLK_ENABLE();
      __HAL_RCC_GPIOC_CLK_ENABLE();
      __HAL_RCC_GPIOD_CLK_ENABLE();
      __HAL_RCC_GPIOE_CLK_ENABLE();
      
      if (board.model == "FR3 1CH") {
        std::vector<uint32_t> a_pins({ 0, 1, 2, 3 });  
        std::vector<uint32_t> a_set({ 2 });
        
        std::vector<uint32_t> b_pins({ 4, 6, 7, 8, 9 });
        std::vector<uint32_t> c_pins({ 7, 8, 9 });
        std::vector<uint32_t> d_pins({ 6, 7, 8, 9, 10, 11, 12, 13, 14 });
        
        std::vector<uint32_t> e_pins({ 2, 5, 6, 8, 9, 10, 12, 13, 14 });
        std::vector<uint32_t> e_set({ 9. 10 });
      } else if (board.model == "OctoLO") {
        std::vector<uint32_t> a_pins({ 0, 1, 2, 3 });
        std::vector<uint32_t> a_set({ });
        
        std::vector<uint32_t> b_pins({ 7, 8, 9 });
        std::vector<uint32_t> c_pins({ 8, 9 });
        std::vector<uint32_t> d_pins({ 6, 7 });
        
        std::vector<uint32_t> e_pins({ 2, 5, 9, 13 });        
        std::vector<uint32_t> e_set({ }); 
      }
      
      setup_bank(GPIOA, a_pins, a_set);
      setup_bank(GPIOB, b_pins, { });
      setup_bank(GPIOC, c_pins, { });
      setup_bank(GPIOD, d_pins, { });
      setup_bank(GPIOE, e_pins, e_set);
  }
}


void PiRadioApp::pre_kernel()
{
  USBPD_PreInitOs();

  lmx.setup();
}

void PiRadioApp::tx_init()
{
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
  
  if (board.model.size()) {
    std::cout << board.model << " starting..." << std::endl;
  } else {
    std::cout << "Unprovisioned board. Please run 'set board model <model> <revision>' and restart" << std::endl;
  }

  tx_thread_sleep(1000);

  if (board.model == "OctoLO") {
    lmx.set_OSCIN(100e6);
    lmx.dump();
  }

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

void FR31CHPowerTree::power_up()
{
  // 1. Start the power-up sequence
  // a. In-rush Enable
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);
  //tx_thread_sleep(100);
  
  // b. Enable BUCK_5V3
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
  //tx_thread_sleep(100);
  
  // c. Enable BUCK_3V7
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
  //tx_thread_sleep(100);
  
  // 2. Get the LO Working
  // a. LO_CTRL_3V3. 0: Internal LMX. 1: External from SMA
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
  
  // b. Enable the TCXO.
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
  
  // c. Select the Clock source.  0: External. 1: On-board
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
  
}

void FR31CHPowerTree::power_down()
{
}

void OctoLOCHPowerTree::power_up()
{
}

void OctoLOCHPowerTree::power_down()
{
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
