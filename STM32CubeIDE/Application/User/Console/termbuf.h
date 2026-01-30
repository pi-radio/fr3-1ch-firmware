/*
 * termbuf.h
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_TERMBUF_H_
#define APPLICATION_USER_CONSOLE_TERMBUF_H_

#include <stdio.h>

#include <app_threadx.h>
//#include <stdio.h>

#include <tbbase.h>

#define DIRTY_NONE   0xFF


struct termbuf {
  struct tbbase base;
  TX_BYTE_POOL *pool;
  char *buffer;
  char *dirty_start, *dirty_end;
  int rows;
  int cols;

  struct window *top, *bottom;

  struct window *focus;
};

struct termbuf_update {
  struct termbuf *tbuf;
  int line;
  int start;
  int len;
  const char *s;
};




extern void termbuf_create(struct termbuf *tbuf, TX_BYTE_POOL *pool, int rows, int cols);
extern int termbuf_create_window(struct termbuf *tbuf, struct window *win, int start_row, int start_col, int width, int height);

static inline void termbuf_set_focus(struct termbuf *tbuf, struct window *win)
{
  tbuf->focus = win;
}

static inline char *termbuf_getbufat(struct termbuf *tbuf, int row, int col)
{
  if (row >= tbuf->rows) {
    row = tbuf->rows - 1;
  }

  if (col >= tbuf->cols) {
    col = tbuf->cols - 1;
  }

  return tbuf->buffer + row * tbuf->cols + col;
}

static inline void termbuf_validate_line(struct termbuf *tbuf, int line)
{
  tbuf->dirty_start[line] =
      tbuf->dirty_end[line] = DIRTY_NONE;
}

static inline void termbuf_update_start(struct termbuf *tbuf, struct termbuf_update *upd)
{
  upd->tbuf = tbuf;
  upd->line = -1;
  upd->start = DIRTY_NONE;
  upd->len = 0;
}

extern int termbuf_update_next(struct termbuf_update *upd);


#endif /* APPLICATION_USER_CONSOLE_TERMBUF_H_ */
