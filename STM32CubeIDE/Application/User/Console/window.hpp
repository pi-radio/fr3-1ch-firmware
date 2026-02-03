/*
 * window.h
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_WINDOW_HPP_
#define APPLICATION_USER_CONSOLE_WINDOW_HPP_

#include <stdio.h>
#include <termbuf.hpp>



class window : public viewport {
  // Order list
  window *onext, *oprev;

  FILE *f;

  uint8_t *buf;
  size_t buf_size;


protected:
  uint8_t *_bufat(position p) override { return buf + p.row * _stride + p.col; };
  position cursor;
  position pos;
  uint8_t *pcur;
  size_t _stride;
  region _dirty;

public:
  window(viewport *parent, const rect &_r);

  void moveto(position p);
  inline void moveto(uint32_t row, uint32_t col) {
    moveto(position(row, col));
  };

  size_t printf(const char *fmt, ...);
  size_t printf(int row, int col, const char *fmt, ...);
  size_t printf(position p, const char *fmt, ...);
  ssize_t write(const char *str, size_t l);
  ssize_t write(position p, const char *str, size_t l);

  inline rect rlocal() const { return rect(pzero, r.s); }

  void clear(const rect &r);
  inline void clear() { clear(rlocal()); }
  inline void clearline(int l) { clear(rect(l, 0, 1, r.width())); };


  void scroll(ord_t n);
  void cr();
  int putc(char c);

  virtual region dirty_region();

  void dirty(const rect &r) { _dirty += r; };
  void dirty() { dirty(rlocal()); }

  virtual void render(const rect &r) override;

  void validate_self(const rect &r) override;

  void dirtyline(uint32_t l, uint32_t start, uint32_t end) {
    dirty(rect(l, start, 1, end - start));
  }

  void dirtyline(uint32_t l) {
    dirtyline(l, 0, r.width());
  }

  inline void dirtypt() { dirtyline(cursor.row, cursor.col, cursor.col); }

  void copyline(uint32_t dstline, uint32_t srcline);


  virtual uint8_t *alloc_buffer(size_t s) { return parent()->alloc_buffer(s); };

  virtual size_t stride() const { return _stride; };
};



#endif /* APPLICATION_USER_CONSOLE_WINDOW_HPP_ */
