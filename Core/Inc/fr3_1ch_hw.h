#ifndef __FR3_1CH_HW_H__
#define __FR3_1CH_HW_H__

#include <stdint.h>

#include <app_threadx.h>


#define SPI_DEVICE_LMX  0
#define SPI_DEVICE_LTC  1

void fr3_1ch_hw_init(void);

int spi_transmit(int device, int lenb, uint32_t w);
int spi_transfer(int device, int lenb, uint32_t *v);


#endif  /* __FR3_1CH_HW_H__ */
