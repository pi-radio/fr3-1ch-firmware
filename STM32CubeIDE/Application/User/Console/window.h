/*
 * window.h
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_WINDOW_H_
#define APPLICATION_USER_CONSOLE_WINDOW_H_

#include <stdio.h>
#include <terminal.h>

struct window {
  struct tbbase base;

  // Order list
  struct window *onext, *oprev;

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

static inline char *wbufat(struct window *win, int r, int c)
{
  return termbuf_getbufat(win->tbuf, win->start_row + r, win->start_col + c);
}


extern void wprintf(struct window *win, const char *fmt, ...);
extern void waprintf(struct window *win, int row, int col, const char *fmt, ...);
extern ssize_t wwrite(struct window *win, const char *str, size_t l);

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

  *pstart = *pstart == DIRTY_NONE ? start : min(*pstart, start);
  *pend = *pend == DIRTY_NONE ? end : max(*pend, end);
}


static inline void wldirtypt(struct window *win)
{
  wldirty(win, win->cur_row, win->cur_col, win->cur_col);
}

static inline void wdirtyline(struct window *win, int l)
{
  wldirty(win, l, 0, win->width - 1);
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

static inline void wlclearline(struct window *win, int l)
{
  char *dst = wbufat(win, l, 0);

  memset(dst, 0, win->width);

  wdirtyline(win, l);
}




#endif /* APPLICATION_USER_CONSOLE_WINDOW_H_ */
