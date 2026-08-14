/*
 * termbuf.c
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */
#include <main.h>
#include <stdio.h>
#include <stdarg.h>
#include <consolexx/termbuf.hpp>
#include <consolexx/cooked.hpp>

using namespace consolexx;

termbuf::termbuf(cooked_terminal *term, uint32_t rows, uint32_t cols) : viewport(nullptr, rect(0,0,rows,cols)), engine(term)
{
}

uint8_t *termbuf::alloc_buffer(size_t s)
{
  return new uint8_t[s];
};

void termbuf::render_buffer(const rect &_r, const uint8_t *buf, const size_t &stride)
{
  if (engine == NULL) {
    return;
  }

  for (auto line : _r.lines()) {
    engine->draw(position(line, _r.left()), buf, _r.width());
    buf += stride;
  }

}

void termbuf::on_event(const std::shared_ptr<event> &evt)
{
  engine->on_event(evt);
}
