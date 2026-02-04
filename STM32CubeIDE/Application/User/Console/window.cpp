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
  pcur = _bufat(cursor);

  f = fopencookie(this, "w", win_iofunc);

  buf_size = r.area();
  buf = alloc_buffer(buf_size);

  memset(buf, 0, r.area());

  _stride = r.width();
}

uint8_t *window::_bufat(position p)
{
  if (p.row < 0 || p.row >= r.height()) {
    Error_Handler();
  }

  if (p.col < 0 || p.col >= r.width()) {
    Error_Handler();
  }

  return buf + p.row * _stride + p.col;
};

void window::clear(const rect &r)
{
  ord_t i;

  uint8_t *pbuf = _bufat(r.p);

  dirty(r);

  for (i = 0; i < r.height(); i++) {
    memset(pbuf, 0, r.width());
    pbuf += stride();
  }
}

void window::moveto(position p)
{
  if (p.row >= r.height()) {
    p.row = r.height() - 1;
  }

  if (p.col >= r.width()) {
    p.col = r.width() - 1;
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


ssize_t window::write(const char *s, size_t l)
{
  ssize_t retval = l;

  while (l) {
    char c = *s++;
    l--;

    if (c == '\n') {
      if (cursor.row == r.height() - 1) {
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

  if (std::abs(n) >= r.height()) {
    clear();
    return;
  }

  if (n > 0) {
    l = 0;

    for (i = 0; i < r.height() - n; i++) {
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
  if (cursor.col >= r.width()) {
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
  memcpy(_bufatline(dstline), _bufatline(srcline), r.width());
}

void window::validate_self(const rect &_r)
{
  if (_r.contains(r)) {
    _dirty.clear();
    return;
  }

  _dirty -= r;
}

region window::dirty_region()
{
  return _dirty;
}

void window::render(const rect &_r)
{
  rect rr = _r & rlocal();

  render_buffer(rr, bufat(rr.p), _stride);
}



