/*
 * termbuf.c
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */
#include <main.h>
#include <stdio.h>
#include <stdarg.h>
#include <termbuf.hpp>

termbuf::termbuf(TX_BYTE_POOL *_pool, uint32_t rows, uint32_t cols) : viewport(NULL, rect(0,0,rows,cols)),
    pool(_pool)
{
}

uint8_t *termbuf::alloc_buffer(size_t s)
{
  int result;
  void *p = NULL;

  result = tx_byte_allocate(pool, &p, s, TX_NO_WAIT);

  if ((result != TX_SUCCESS) || (p == NULL)) {
    Error_Handler();
  }

  return (uint8_t *)p;
};

void termbuf::render_buffer(const rect &_r, const uint8_t *buf, const size_t &stride)
{
  for (auto line : _r.lines()) {
    engine->draw(position(line, _r.left()), buf, _r.width());
    buf += stride;
  }

}
