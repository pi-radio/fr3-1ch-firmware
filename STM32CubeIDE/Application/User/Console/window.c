/*
 * window.c
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */
#include <stdarg.h>
#include <window.h>






static ssize_t win_read(void *cookie, char *data, size_t n)
{
  return n;
}

static ssize_t win_write(void *cookie, const char *data, size_t n)
{
  return wwrite((struct window *)cookie, data, n);
}

static int win_close(void *cookie)
{
  return 0;
}

cookie_io_functions_t win_iofunc = {
    .read = win_read,
    .write = win_write,
    .seek = NULL,
    .close = win_close
};


int window_create(struct window *win, struct termbuf *tbuf, int start_row, int start_col, int height,  int width)
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

  win->on_input = NULL;

  return 0;
}

extern void wprintf(struct window *win, const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);

  vfprintf(win->f, fmt, args);

  va_end(args);

  fflush(win->f);
}



void wscroll(struct window *win, int n)
{
  int i, l;

  if (n >= win->height) {
    for (i = 0; i < win->height; i++) {
      wlclearline(win, i);
    }
    return;
  }

  l = 0;

  for (i = 0; i < win->height - n; i++) {
    wlclearline(win, l);
    wlcopy(win, l, l + n);
    l++;
  }

  for (i = 0; i < n; i++) {
    wlclearline(win, l);
    l++;
  }
}

int wincr(struct window *win) {
  win->cur_col = 0;
  win->pcur = wbufat(win, win->cur_row, win->cur_col);
}

int wputc(struct window *win, int c) {
  if (win->cur_col > win->width) {
    return -1;
  }

  *win->pcur++ = c;
  wldirtypt(win);
  win->cur_col++;

  return 0;
}

ssize_t wwrite(struct window *win, const char *s, size_t l)
{
  ssize_t retval = l;

  while (l) {
    char c = *s++;
    l--;

    if (c == '\n') {
      if (win->cur_row == win->height - 1) {
        wscroll(win, 1);
      } else {
        win->cur_row++;
      }
      // for now
      wincr(win);
    } else if (c == '\r') {
      wincr(win);
    } else if (c >= 0x20 && c <= 0x7E) {
      wputc(win, c);
    }
  }

  return retval;
}



