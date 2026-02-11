/*
 * window.c
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */
#include <stdarg.h>
#include <window.hpp>

extern "C" void Error_Handler(void);

static ssize_t win_read(void *cookie, char *data, size_t n)
{
  return n;
}

static ssize_t win_write(void *cookie, const char *data, size_t n)
{
  return ((window *)cookie)->write(data, n);
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


window::window(viewport *_parent, const rect &_r) :
  viewport(_parent, _r),
  cursor(0, 0)
{
  f = fopencookie(this, "w", win_iofunc);

  buf_size = s.area();
  buf = alloc_buffer(buf_size);

  memset(buf, 0, s.area());

  _stride = s.width;
  pcur = _bufat(cursor);
}

uint8_t *window::_bufat(position _p)
{
  if (!rlocal().inside(_p)) {
    Error_Handler();
  }

  return buf + _p.row * _stride + _p.col;
};

void window::clear(const rect &r)
{
  ord_t i;

  uint8_t *pbuf = _bufat(r.p);

  dirty(r);

  for (i = 0; i < r.height(); i++) {
    memset(pbuf, 0, s.width);
    pbuf += stride();
  }
}

void window::moveto(position p)
{
  if (p.row >= s.height) {
    p.row = s.height - 1;
  }

  if (p.col >= s.width) {
    p.col = s.width - 1;
  }

  cursor = p;
  pcur = _bufat(cursor);
}

size_t window::printf(const char *fmt, ...)
{
  size_t retval;

  va_list args;

  va_start(args, fmt);

  retval = vfprintf(f, fmt, args);

  va_end(args);

  fflush(f);

  return retval;
}

size_t window::printf(position p, const char *fmt, ...)
{
  size_t retval;

  va_list args;

  moveto(p);

  va_start(args, fmt);

  retval = vfprintf(f, fmt, args);

  va_end(args);

  fflush(f);

  return retval;
}


ssize_t window::write(const char *str, size_t l)
{
  ssize_t retval = l;

  while (l) {
    char c = *str++;
    l--;

    if (c == '\n') {
      if (cursor.row == s.height - 1) {
        scroll(1);
      } else {
        cursor.row++;
      }
      // for now
      cr();
    } else if (c == '\r') {
      cr();
    } else if (c >= 0x20 && c <= 0x7E) {
      putc(c);
    }
  }

  return retval;
}



void window::scroll(ord_t n)
{
  ord_t i, l;

  if (n == 0) return;

  dirty();

  if (std::abs(n) >= s.height) {
    clear();
    return;
  }

  if (n > 0) {
    l = 0;

    for (i = 0; i < s.height - n; i++) {
      clearline(l);
      copyline(l, l + n);
      l++;
    }

    for (i = 0; i < n; i++) {
      clearline(l);
      l++;
    }
  }
}

void window::cr() {
  cursor.col = 0;
  pcur = _bufat(cursor);
}

int window::putc(char c) {
  if (cursor.col >= s.width) {
    return -1;
  }

  *pcur++ = c;
  dirtypt();
  cursor.col++;

  return 0;
}

void window::copyline(uint32_t dstline, uint32_t srcline)
{
  dirtyline(dstline);
  memcpy(_bufatline(dstline), _bufatline(srcline), s.width);
}

void window::refresh_self(const rect &_r)
{
  rect rr = _r & rlocal();

  render_buffer(rr, bufat(rr.p), _stride);
}

void window::validate_self(const rect &_r)
{
  _dirty.clear();
  return;

  if (_r.contains(rlocal())) {
    _dirty.clear();
    return;
  }
}

rect window::dirty_rect()
{
  return _dirty;
}



