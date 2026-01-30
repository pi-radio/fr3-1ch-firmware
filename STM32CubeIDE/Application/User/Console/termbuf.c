/*
 * termbuf.c
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */
#include <termbuf.h>
#include <stdio.h>
#include <stdarg.h>


void termbuf_create(struct termbuf *tbuf, TX_BYTE_POOL *pool, int rows, int cols)
{
  tbuf->rows = rows;
  tbuf->cols = cols;
  tbuf->focus = NULL;

  tx_byte_allocate(pool, (VOID **)&tbuf->buffer, rows * cols, TX_NO_WAIT);
  tx_byte_allocate(pool, (VOID **)&tbuf->dirty_start, rows, TX_NO_WAIT);
  tx_byte_allocate(pool, (VOID **)&tbuf->dirty_end, rows, TX_NO_WAIT);

  memset(tbuf->buffer, 0, rows * cols);
  memset(tbuf->dirty_start, -1, rows);
  memset(tbuf->dirty_end, -1, rows);
}

int termbuf_update_next(struct termbuf_update *upd)
{
  while (++upd->line < upd->tbuf->rows) {
    if (upd->tbuf->dirty_start[upd->line] != DIRTY_NONE) {
      upd->start = upd->tbuf->dirty_start[upd->line];
      upd->len = upd->tbuf->dirty_end[upd->line] - upd->start + 1;
      upd->s = termbuf_getbufat(upd->tbuf, upd->line, upd->start);
      return 1;
    }
  }

  return 0;
}





