/*
 * ltc2668.cpp
 *
 *  Created on: Feb 12, 2026
 *      Author: zapman
 */
#include <fr3_1ch_hw.h>
#include <ltc2668.h>

LTC2668 ltc2668;

LTC2668::LTC2668()
{

}

void LTC2668::initialize()
{
  spi_transmit(SPI_DEVICE_LTC, 3, 0xE00004);

  spi_transmit(SPI_DEVICE_LTC, 3, 0x800000 | V2code(0));
}

void LTC2668::setV(int n, double V)
{
  spi_transmit(SPI_DEVICE_LTC, 3, 0xE00004);
  tx_thread_sleep(100);
  spi_transmit(SPI_DEVICE_LTC, 3, (n << 16) | V2code(V) | 0x300000);
}
