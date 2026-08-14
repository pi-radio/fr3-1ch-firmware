/*
 * termbuf.h
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_TERMBUF_HPP_
#define APPLICATION_USER_CONSOLE_TERMBUF_HPP_

#include "tx_api.h"

#include <consolexx/viewport.hpp>

#define DIRTY_NONE   0xFF


namespace consolexx
{
  class window;
  class cooked_terminal;

  class termbuf : public viewport {
    uint8_t *buffer;
    window *focus;
    cooked_terminal *engine;

  protected:
    uint8_t *_bufat(position p) override {
      if (p.row >= get_size().height) return NULL;
      if (p.col >= get_size().width) return NULL;

      return buffer + p.row * cols() + p.col;
    };

  public:
    termbuf(cooked_terminal *term, uint32_t rows, uint32_t cols);

    virtual uint8_t *alloc_buffer(size_t s);

    virtual size_t stride() const { return s.width; };

    virtual void render_buffer(const rect &, const uint8_t *, const size_t &) override;

    virtual void on_event(const std::shared_ptr<event> &evt);
  };
}

#endif /* APPLICATION_USER_CONSOLE_TERMBUF_HPP_ */
