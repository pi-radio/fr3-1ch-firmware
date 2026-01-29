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

#define DIRTY_NONE   0xFF

struct termbuf {
  TX_BYTE_POOL *pool;
  char *buffer;
  char *dirty_start, *dirty_end;
  int rows;
  int cols;

  struct window *focus;
};

struct termbuf_update {
  struct termbuf *tbuf;
  int line;
  int start;
  int len;
  const char *s;
};

struct window {
  struct termbuf *tbuf;
  FILE *f;

  int start_row;
  int start_col;
  int width;
  int height;

  int cur_row;
  int cur_col;

  char *pcur;

  int (*on_input)(struct window *window, int c);
};

struct text_field {
  struct window win;

  int (*on_nl)(struct text_field *, const char *s, int l);
  int (*on_cr)(struct text_field *, const char *s, int l);
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

extern int text_field_on_input(struct window *, int);

extern int text_field_create(struct termbuf *tbuf, struct text_field *tf, int start_row, int start_col, int width, int height);


extern void wprintf(struct window *win, const char *fmt, ...);
extern ssize_t wwrite(struct window *win, const char *str, size_t l);

#endif /* APPLICATION_USER_CONSOLE_TERMBUF_H_ */
