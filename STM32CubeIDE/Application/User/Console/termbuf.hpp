/*
 * termbuf.h
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_TERMBUF_HPP_
#define APPLICATION_USER_CONSOLE_TERMBUF_HPP_

#include <stdio.h>

#include <app_threadx.h>
//#include <stdio.h>

#include <viewport.hpp>

#define DIRTY_NONE   0xFF

class window;

struct termbuf_render_engine {
  virtual void draw(position p, const uint8_t *buf, size_t len) = 0;
};

class termbuf : public viewport {
  TX_BYTE_POOL *pool;
  uint8_t *buffer;
  window *focus;
  termbuf_render_engine *engine;

protected:
  uint8_t *_bufat(position p) override {
    if (p.row >= get_size().height) return NULL;
    if (p.col >= get_size().width) return NULL;

    return buffer + p.row * cols() + p.col;
  };

public:
  termbuf(TX_BYTE_POOL *_pool, uint32_t rows, uint32_t cols);

  inline void set_render_engine(termbuf_render_engine *_engine) { engine =_engine; }

  virtual uint8_t *alloc_buffer(size_t s);

  virtual size_t stride() const { return s.width; };

  virtual void render_buffer(const rect &, const uint8_t *, const size_t &) override;
};

#endif /* APPLICATION_USER_CONSOLE_TERMBUF_HPP_ */
