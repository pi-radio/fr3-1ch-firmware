/*
 * termbuf.c
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */
#include <termbuf.h>
#include <stdio.h>
#include <stdarg.h>

int termbuf_update_next(struct termbuf_update *upd)
{
  while (++upd->line < upd->tbuf->rows) {
    if (upd->tbuf->dirty_start[upd->line] != -1) {
      upd->start = upd->tbuf->dirty_start[upd->line];
      upd->len = upd->tbuf->dirty_end[upd->line] - upd->start;
      upd->s = termbuf_getbufat(upd->tbuf, upd->line, upd->start);
      return 1;
    }
  }

  return 0;
}

static inline char *wbufat(struct window *win, int r, int c)
{
  return termbuf_getbufat(win->tbuf, win->start_row + r, win->start_col + c);
}

#if 0
 static inline char *termbuf_getcurpos(struct termbuf *tbuf)
 {
   return termbuf_getbufpos(tbuf, tbuf->cur_row, tbuf->cur_col);
 }

 static inline char *termbuf_getline(struct termbuf *tbuf, int row)
 {
   return termbuf_getbufpos(tbuf, row, 0);
 }

 static inline void termbuf_clear_line(struct termbuf *tbuf, int row)
 {
   memset(termbuf_getline(tbuf, row), 0, tbuf->cols);
 }

 static inline void termbuf_copy_line(struct termbuf *tbuf, int dstrow, int srcrow)
 {
   memcpy(termbuf_getline(tbuf, dstrow), termbuf_getline(tbuf, srcrow), tbuf->cols);
 }

 static inline void termbuf_scroll(struct termbuf *tbuf, int n)
 {
   int i;

   if (n >= tbuf->rows) {
     for (i = 0; i < tbuf->rows; i++) {
       termbuf_clear_line(tbuf, i);
     }
     tbuf->cur_row = 0;
     return;
   }

   tbuf->cur_row -= n;

   for (i = 0; i < tbuf->rows - n; i++) {
     termbuf_copy_line(tbuf, i, i+n);
   }

   for (i = 0; i < n; i++) {
     termbuf_clear_line(tbuf, tbuf->rows - n + i);
   }
 }

 static inline void termbuf_clear(struct termbuf *tbuf)
 {
   termbuf_scroll(tbuf, tbuf->rows);
 }

 static inline void termbuf_putc(struct termbuf *tbuf, int c)
 {
   if (c == '\n') {
     tbuf->cur_row++;

     if (tbuf->cur_row == tbuf->rows) {
       termbuf_scroll(tbuf, 1);
     }

     tbuf->cur_col = 0;
   } else if (c == '\r') {
     tbuf->cur_row++;

     if (tbuf->cur_row == tbuf->rows) {
       termbuf_scroll(tbuf, 1);
     }

     tbuf->cur_col = 0;
   } else if (c >= 0x20 && c <= 0x7E &&  tbuf->cur_col < tbuf->cols) {
     *termbuf_getcurpos(tbuf) = c;
     tbuf->cur_col++;
   }
 }

#endif

void termbuf_create(struct termbuf *tbuf, TX_BYTE_POOL *pool, int rows, int cols)
{
  tbuf->rows = rows;
  tbuf->cols = cols;

  tx_byte_allocate(pool, (VOID **)&tbuf->buffer, rows * cols, TX_NO_WAIT);
  tx_byte_allocate(pool, (VOID **)&tbuf->dirty_start, rows, TX_NO_WAIT);
  tx_byte_allocate(pool, (VOID **)&tbuf->dirty_end, rows, TX_NO_WAIT);

  memset(tbuf->buffer, 0, rows * cols);
  memset(tbuf->dirty_start, -1, rows);
  memset(tbuf->dirty_end, -1, rows);
}

static ssize_t win_write(void *cookie, const char *data, size_t n)
{
  return wwrite((struct window *)cookie, data, n);
}


cookie_io_functions_t win_iofunc = {
    .read = NULL,
    .write = win_write,
    .seek = NULL,
    .close = NULL
};

int termbuf_create_window(struct termbuf *tbuf, struct window *win, int start_row, int start_col, int height,  int width)
{
  if (start_row < 0 || start_col < 0 ||
      height < 0 || width < 0 ||
      start_row + height > tbuf->rows ||
      start_col + width > tbuf->cols) {
    Error_Handler();
  }

  win->tbuf = tbuf;
  win->start_row = start_row;
  win->start_col = start_col;
  win->width = width;
  win->height = height;

  win->cur_row = 0;
  win->cur_col = 0;

  win->pcur = wbufat(win, 0, 0);

  win->f = fopencookie(win, "w", win_iofunc);

  return 0;
}

extern void wprintf(struct window *win, const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);

  vfprintf(win->f, fmt, args);

  va_end(args);
}

static inline char *wpdstart(struct window *win, int l)
{
  return &win->tbuf->dirty_start[win->start_row + l];
}

static inline char *wpdend(struct window *win, int l)
{
  return &win->tbuf->dirty_end[win->start_row + l];
}

static inline int wdstart(struct window *win, int l)
{
  return *wpdstart(win, l);
}

static inline int wdend(struct window *win, int l)
{
  return *wpdend(win, l);
}

static inline int min(int a, int b)
{
  return (a < b) ? a : b;
}

static inline int max(int a, int b)
{
  return (a > b) ? a : b;
}

static inline void wldirty(struct window *win, int l, int start, int end)
{
  char *pstart, *pend;

  pstart = wpdstart(win, l);
  pend = wpdend(win, l);

  *pstart = *pstart == -1 ? start : min(*pstart, start);
  *pend = *pend == -1 ? end : max(*pend, end);
}

static inline void wdirtyline(struct window *win, int l)
{
  wldirty(win, l, 0, win->width);
}

static inline void wlcopy(struct window *win, int dstline, int srcline)
{
  char *dst = termbuf_getbufat(win->tbuf, win->start_row + dstline, win->start_col);
  char *src = termbuf_getbufat(win->tbuf, win->start_row + srcline, win->start_col);

  memcpy(dst, src, win->width);

  wdirtyline(win, dstline);
}



static inline char *wpcur(struct window *win)
{
  return termbuf_getbufat(win->tbuf, win->start_row + win->cur_row, win->start_col + win->cur_col);
}

static inline void wlclear(struct window *win, int l)
{
  char *dst = wbufat(win, l, 0);

  memset(dst, 0, win->width);

  wdirtyline(win, l);
}


void wscroll(struct window *win, int n)
{
  int i;

  if (n >= win->height) {
    for (i = 0; i < win->height - n; i++) {
      wlclear(win, i);
    }
  }



  for (i = 0; i < win->height - n; i++) {
    wldirty(win, i, 0, win->width);
    wlcopy(win, i, i + n);
  }
}

ssize_t wwrite(struct window *win, const char *s, size_t l)
{
  ssize_t retval = l;

  while (l) {
    char c = *s++;
    l--;

    if (c == '\n') {
      win->cur_row++;

      if (win->cur_row == win->height) {
        wscroll(win, 1);
      }

      // for now
      win->cur_col = 0;

      win->pcur = wpcur(win);
    } else if (c == '\r') {
      win->cur_col = 0;
      win->pcur = wpcur(win);
    } else if (c >= 0x20 && c <= 0x7E) {
      if (win->cur_col < win->width) {
        *win->pcur++ = c;
        wldirty(win, win->cur_row, win->cur_col, win->cur_col);
        win->cur_col++;
      }
    }
  }

  return retval;
}


