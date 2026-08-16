#include <piradio/hardware.hpp>

#include "fr3_1ch_hw.h"

#include "main.h"

extern "C" {
#if 0
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
#endif
}

using namespace piradio::hardware;

void Gen1Hardware::initialize_gpios()
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  setup_gpios();
}


void Gen1Hardware::setup_bank(GPIO_TypeDef *gpio,
                              const std::vector<uint32_t> &pins,
                              const std::vector<uint32_t> &set_pins)
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


/**
 * Power up sequence
 *
 * 1 - enable the inrush limiter
 * 2 - start the 5v3 buck
 * 3 - start the 3V7 buck
 */
void FR31CHHardware::power_up()
{
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);
  tx_thread_sleep(10);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
  tx_thread_sleep(10);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
  tx_thread_sleep(10);

}

void FR31CHHardware::power_down()
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
}

void FR31CHHardware::setup_gpios()
{
  std::vector<uint32_t> a_pins, b_pins, c_pins, d_pins, e_pins;
  std::vector<uint32_t> a_set, e_set;

  a_pins = { 0, 1, 2, 3 };
  a_set = { 2 };

  b_pins = { 4, 6, 7, 8, 9 };
  c_pins = { 7, 8, 9 };
  d_pins = { 6, 7, 8, 9, 10, 11, 12, 13, 14 };

  e_pins = { 2, 5, 6, 8, 9, 10, 12, 13, 14 };
  e_set = { 9, 10 };

  setup_bank(GPIOA, a_pins, a_set);
  setup_bank(GPIOB, b_pins, { });
  setup_bank(GPIOC, c_pins, { });
  setup_bank(GPIOD, d_pins, { });
  setup_bank(GPIOE, e_pins, e_set);
}

void FR31CHHardware::restore_settings()
{
  using namespace piradio::config;

  auto iqv = TXX::config_data::config.get<iq_voltages>();

  if (iqv) {
    i_voltage = iqv->I_V;
    q_voltage = iqv->Q_V;
  }

  // 2. Get the LO Working
  // a. LO_CTRL_3V3. 0: Internal LMX. 1: External from SMA
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);

  // b. Enable the TCXO.
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

  // c. Select the Clock source.  0: External. 1: On-board
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);


  _do_set_i_voltage();
  _do_set_q_voltage();


  printf("Programming LMX...\r\n");
  lmx.setup();

  lmx.program();
}

void FR31CHHardware::_do_set_i_voltage()
{
  ltc2668.setV(2, i_voltage);
}

void FR31CHHardware::_do_set_q_voltage()
{
  ltc2668.setV(0, q_voltage);
}

void FR31CHHardware::save_IQV()
{
  using namespace piradio::config;

  iq_voltages voltages(i_voltage, q_voltage);

  TXX::config_data::config.save(voltages);
}

void FR31CHHardware::set_I_voltage(float v)
{
  i_voltage = v;

  _do_set_i_voltage();

  save_IQV();
}

void FR31CHHardware::set_Q_voltage(float v)
{
  q_voltage = v;

  _do_set_q_voltage();

  save_IQV();
}

GPIO_PinState pin_value(uint32_t v)
{
  if (v == 0)
    return GPIO_PIN_RESET;
  else
    return GPIO_PIN_SET;
}

void FR31CHHardware::set_rx_filter(uint8_t v)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, pin_value(v & 0x20));
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, pin_value(v & 0x10));
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, pin_value(v & 0x08));
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, pin_value(v & 0x04));
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, pin_value(v & 0x02));
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, pin_value(v & 0x01));
}

void FR31CHHardware::set_tx_filter(uint8_t v)
{
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, pin_value(v & 0x20));
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, pin_value(v & 0x10));
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9,  pin_value(v & 0x08));
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, pin_value(v & 0x04));
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, pin_value(v & 0x02));
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, pin_value(v & 0x01));
}


void OctoLOHardware::power_up()
{
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
  tx_thread_sleep(10);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
  tx_thread_sleep(10);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
  tx_thread_sleep(10);

  // Start up the OCXO -- give time for initial warm up
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
  tx_thread_sleep(100);

  // Enable the LMX
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
  tx_thread_sleep(10);

  // Enable the LMX
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
  tx_thread_sleep(10);

  // Enable the LMX
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  tx_thread_sleep(10);
}

void OctoLOHardware::power_down()
{

}

void OctoLOHardware::setup_gpios()
{
  std::vector<uint32_t> a_pins, b_pins, c_pins, d_pins, e_pins;
  std::vector<uint32_t> a_set, e_set;

  a_pins = { 0, 1, 2, 3 };

  b_pins = { 7, 8, 9 };
  c_pins = { 8, 9 };
  d_pins = { 6, 7 };

  e_pins = { 2, 5, 9, 13 };

  setup_bank(GPIOA, a_pins, a_set);
  setup_bank(GPIOB, b_pins, { });
  setup_bank(GPIOC, c_pins, { });
  setup_bank(GPIOD, d_pins, { });
  setup_bank(GPIOE, e_pins, e_set);
}

void OctoLOHardware::restore_settings()
{
  printf("Programming LMX...\r\n");
  lmx.setup();

  lmx.program();
}

