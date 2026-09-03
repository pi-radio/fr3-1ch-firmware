#pragma once

#include "stm32h5xx_hal.h"

#include <cassert>

static inline void pcd_set_tx_cnt(uint32_t epno, uint32_t cnt)
{
  (USB_DRD_PMA_BUFF + epno)->TXBD &= USB_PMA_TXBD_COUNTMSK;
  (USB_DRD_PMA_BUFF + epno)->TXBD |= (uint32_t)((uint32_t)(cnt) << 16U);
}

static inline void pcd_set_rx_cnt(uint32_t epno, uint32_t cnt)
{
  volatile uint32_t *reg = &(USB_DRD_PMA_BUFF + epno)->RXBD;

  *reg &= ~(USB_CNTRX_BLSIZE | USB_CNTRX_NBLK_MSK);

  if (cnt == 0U)
  {
    *reg |= USB_CNTRX_BLSIZE;
    return;
  }

  if (cnt <= 62U)
  {
    auto n_blocks = cnt >> 1U;

    if (cnt & 0x1)
      n_blocks++;

    *reg |= n_blocks << 26U;

    return;
  }


  auto n_blocks =(cnt >> 5U);

  if ((cnt & 0x1F) == 0U)
  {
    n_blocks--;
  }

  *reg |= (n_blocks << 26U) | USB_CNTRX_BLSIZE;

}

static inline void pcd_set_tx_address(uint32_t epno, uint32_t addr)
{
  assert(addr != 0);

  (USB_DRD_PMA_BUFF + (epno))->TXBD &= USB_PMA_TXBD_ADDMSK;
  (USB_DRD_PMA_BUFF + (epno))->TXBD |= addr & ~0x3;
}

static inline void pcd_set_rx_address(uint32_t epno, uint32_t addr)
{
  assert(addr != 0);

  (USB_DRD_PMA_BUFF + (epno))->RXBD &= USB_PMA_RXBD_ADDMSK; \
  (USB_DRD_PMA_BUFF + (epno))->RXBD |= addr & ~0x3;
}


static inline void pcd_set_dbuf1_cnt(uint32_t epno, uint32_t bDir, uint32_t wCount)
{
  if (!bDir)
  {
    pcd_set_rx_cnt(epno, wCount);
    return;
  }

  (USB_DRD_PMA_BUFF + (epno))->RXBD &= USB_PMA_TXBD_COUNTMSK;
  (USB_DRD_PMA_BUFF + (epno))->RXBD |= wCount << 16U;
}

