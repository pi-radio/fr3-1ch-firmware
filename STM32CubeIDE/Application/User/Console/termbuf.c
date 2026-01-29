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

static inline char *wbufat(struct window *win, int r, int c)
{
  return termbuf_getbufat(win->tbuf, win->start_row + r, win->start_col + c);
}


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

int text_field_on_input(struct window *win, int c)
{
  struct text_field *tf = (struct text_field *)win;

  if (c == '\n') {
    if (tf->on_nl) {
      tf->on_nl(tf, wbufat(win, 0, 0), win->cur_col + 1);
    }

    wlclearline(win, 0);
    wincr(win);
  } else if (c == '\r') {
    if (tf->on_cr) {
      tf->on_cr(tf, wbufat(win, 0, 0), win->cur_col + 1);
    }

    wlclearline(win, 0);
    wincr(win);
  } else if (c == '\b' || c == 0x7F) {
    if (win->cur_col == 0) {
      txchar('\a');
      return;
    }

    win->pcur--;
    *win->pcur = 0;
    win->cur_col--;
    wldirtypt(win);
  } else if (c >= 0x20 && c < 0x7F) {
    wputc(win, c);
    return 0;
  } else {
    printf("Unknown char 0x%2.2x\n", c);
  }

  return -1;
  /*
  if (c == '\n') {
      if (term->echo) {
        lock_tx();

        if (term->onlcr) {
          txchar('\r');
        }
        txchar('\n');

        unlock_tx();
      }

      for (i = 0; i < term->input_len; i++) {
        terminal_send_rxchar(term->input_buf[i]);
      }

      terminal_send_rxchar('\n');

      term->input_len = 0;

      // EOL callback?
    } else   if (c == '\b') {
      if (term->buffer_input && (term->input_len > 0)) {
        term->input_len--;
      }
    } else if (c == 0x7F) {
      // DELETE?
      if (term->buffer_input && (term->input_len > 0)) {
        term->input_len--;
      }
    } else if (c == '\t') {
      // Check for tab callback
      // Absorb tab
    } else if (c == '\r') {
      // Check for tab callback
      if (term->echo) {
        lock_tx();

        txchar('\r');
        if (term->onlcr) {
          txchar('\n');
        }

        unlock_tx();
      }

      for (i = 0; i < term->input_len; i++) {
        terminal_send_rxchar(term->input_buf[i]);
      }

      // Assume translate
      terminal_send_rxchar('\n');

      term->input_len = 0;
    } else if (c >= 0x20 && c <= 0x7F) {
      if (term->echo) {
        lock_tx();
        txchar(c);
        unlock_tx();
      }

      if (!term->buffer_input) {
        tx_queue_send(&terminal_rx_queue, &c, TX_WAIT_FOREVER);
      } else {
        if (term->input_len < sizeof(term->input_buf)) {
          term->input_buf[term->input_len++] = c;
        } else {
          term->input_buf[sizeof(term->input_buf) - 1] = c;
        }
      }
    } else {
      // Invalid char callback
      terminal_handle_error(term, c);
    }
  */
}

int text_field_create(struct termbuf *tbuf, struct text_field *tf, int start_row, int start_col, int width, int height)
{
  termbuf_create_window(tbuf, &tf->win, start_row, start_col, width, height);

  tf->win.on_input = text_field_on_input;
  tf->on_nl = NULL;

  return 0;
}

