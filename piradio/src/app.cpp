#include <piradio/app.hpp>
#include <piradio/parser.hpp>
#include <piradio/config.hpp>
#include <threadxx/config_data.hpp>
#include <consolexx/cooked.hpp>
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

PiRadioApp::PiRadioApp() : cmd_queue("App command queue"),
    usb_io(usb_serial),
    term(this, &usb_io)
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

void PiRadioApp::setup_tick()
{
  MX_DCACHE1_Init();
  MX_ICACHE_Init();
  MX_DTS_Init();
  MX_LPTIM1_Init();
}

void PiRadioApp::initialize_hardware()
{
  try {
    auto bm = config.get<board_model>();

    if (bm != nullptr) {
      board.model.append((char *)bm->model, bm->length);
      board.revision = bm->revision;
    } else {
      dbg::dbgout << "Board model not found" << std::endl;
    }
  } catch(...) {
    dbg::dbgout << "Board model not found" << std::endl;
  }

  if (board.model == "FR3 1CH") {
    hardware = new piradio::hardware::FR31CHHardware();
  } else if (board.model == "OctoLO") {
    hardware = new piradio::hardware::OctoLOHardware();
  } else if (board.model.size() == 0) {
    std::cout << "Unprovisioned board. Please run 'set board model <model> <revision>' and restart" << std::endl;
    hardware = new piradio::hardware::UnconfiguredHardware();
  } else {
    std::cout << "Unknown board model '" << board.model << "'.  We should never see this." << std::endl;
    hardware = new piradio::hardware::UnconfiguredHardware();
  }

  usb_serial.set_manufacturer("Pi Radio");

  if (hardware->configured()) {
    std::cout << board.model << " starting..." << std::endl;
    usb_serial.set_product(board.model);
  } else {
    usb_serial.set_product("Unprovisioned");
  }



  //initialize_gpios();
  hardware->initialize_gpios();
  
  MX_GPDMA1_Init();
  MX_SPI4_Init();
  MX_UCPD1_Init();
}

void setup_bank(GPIO_TypeDef *gpio, const std::vector<uint32_t> &pins, const std::vector<uint32_t> &set_pins)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  uint32_t pin_mask = 0,
    set_mask = 0;
  
  for(auto &i: pins) {
    pin_mask |= (1 << i);
  }
  
  GPIO_InitStruct.Pin = pin_mask;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  HAL_GPIO_Init(gpio, &GPIO_InitStruct);

  for(auto &i: set_pins) {
    set_mask |= (1 << i);
  }
  
  HAL_GPIO_WritePin(gpio, pin_mask & ~set_mask, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(gpio, pin_mask & set_mask, GPIO_PIN_SET);
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
      
      std::vector<uint32_t> a_pins, b_pins, c_pins, d_pins, e_pins;
      std::vector<uint32_t> a_set, e_set;

      if (board.model == "FR3 1CH") {
        a_pins = { 0, 1, 2, 3 };
        a_set = { 2 };
        
        b_pins = { 4, 6, 7, 8, 9 };
        c_pins = { 7, 8, 9 };
        d_pins = { 6, 7, 8, 9, 10, 11, 12, 13, 14 };
        
        e_pins = { 2, 5, 6, 8, 9, 10, 12, 13, 14 };
        e_set = { 9, 10 };
      } else if (board.model == "OctoLO") {
        a_pins = { 0, 1, 2, 3 };
        
        b_pins = { 7, 8, 9 };
        c_pins = { 8, 9 };
        d_pins = { 6, 7 };
        
        e_pins = { 2, 5, 9, 13 };
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
  //USBPD_PreInitOs();
}

void PiRadioApp::tx_init()
{
  usb_serial.start();
  
  cmd_queue.create();

  term.startup();

  output_win = term.get_cooked().create<consolexx::window>(1, 0, 37, 132);
  status_win = term.get_cooked().create<consolexx::window>(39, 0, 1, 132);

  input_win = term.get_cooked().create<consolexx::text_field>(38, 0, 1, 132);

  term.get_cooked().set_default_output(output_win);
  term.get_cooked().set_focus(input_win);

  fr3_1ch_hw_init();
}

void PiRadioApp::app_main()
{
  parser::Parser p;

  hardware->power_up();

  hardware->restore_settings();

  while (true) {
    auto cmd = cmd_queue.pop();

    p.set_command(cmd);
    
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

void PiRadioApp::on_event(const consolexx::evt_ptr &evt)
{
  using namespace consolexx;
  using namespace parser;

  if (evt->type == input_event::EVENT_TYPE) {
    auto input = event::to<input_event>(evt);

    std::shared_ptr<Command> cmd = std::make_shared<Command>(input->s, Command::CONSOLE);

    output_win->write(input->s + "\n");

    cmd_queue.push(cmd);
  }
}

int PiRadioApp::render(const char *buffer, size_t size)
{
  HAL_UART_Transmit(&huart1, (const uint8_t *)buffer, size, 0xFFFF);
  return 0;
}

int PiRadioApp::writemsg(const char *buffer, size_t size)
{
  return term.output_handler(buffer, size);
}

void PiRadioApp::status_update()
{
  int result;
  int32_t temp;

  result = HAL_DTS_GetTemperature(&hdts, &temp);

  if (result == TX_SUCCESS) {
    status_win->printf(consolexx::position(0,0), "Temp: %dC", temp);
  } else {
    status_win->printf(consolexx::position(0,0), "Temp: ERROR");
  }
}

void PiRadioApp::redraw()
{
  term.get_cooked().redraw();
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

