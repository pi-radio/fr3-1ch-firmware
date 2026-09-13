#include <piradio/hardware.hpp>

using namespace piradio::hardware;

extern "C" {
#include "spi.h"
}

SPIRequest::SPIRequest(SPITarget _target, size_t _len, uint32_t _v) :
        target(_target),
        len(_len),
        v(_v)
{

}

int SPIRequest::process()
{
  uint32_t pin;
  uint8_t tx_buf[4], rx_buf[4];
  int i;

  switch(target) {
  case SPITarget::LMX:
    pin = GPIO_PIN_9;
    break;
  case SPITarget::LTC2668:
    pin = GPIO_PIN_10;
    break;
  }

  auto pbyte = &tx_buf[len - 1];

  for (i = 0; i < len; i++) {
    *pbyte-- = v & 0xFF;
    v >>= 8;
  }

  HAL_GPIO_WritePin(GPIOE, pin, GPIO_PIN_RESET);

  result = HAL_SPI_TransmitReceive(&hspi4, tx_buf, rx_buf, len, 0xFFFF);

  HAL_GPIO_WritePin(GPIOE, pin, GPIO_PIN_SET);

  v = 0;
  pbyte = rx_buf;

  for (i = 0; i < len; i++)
  {
    v = (v << 8) | *pbyte++;
  }

  return result;
}
