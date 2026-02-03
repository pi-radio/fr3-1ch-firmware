/*
 * tbbase.cpp
 *
 *  Created on: Feb 2, 2026
 *      Author: zapman
 */

#include <main.h>
#include <stdio.h>
#include <stdarg.h>
#include <termbuf.hpp>

int viewport::on_input(int c)
{
  if (_focus != NULL)
    return _focus->on_input(c);
  return 0;
};

uint8_t *viewport::alloc_buffer(size_t s)
{
  return parent()->alloc_buffer(s);
}


void viewport::render_buffer(const rect &_r, const uint8_t *buf, const size_t &stride)
{
  rect rr = _r + r.p;

  if (rr.top() < 0) {
    buf += -rr.top() * stride;
  }

  if (rr.left() < 0) {
    buf += -rr.left();
  }

  rr = rr & r;

  if (rr.empty()) {
    return;
  }

  parent()->render_buffer(rr, buf, stride);
}

void viewport::render(const rect &r)
{

}

void viewport::redraw(region &occluded)
{
  dirty();

  for (auto pchild : children) {
    pchild->redraw(occluded);
  }

  region draw_rects = local2global(r) - occluded;

  occluded += r;

  for (auto r : draw_rects.rects) {
    render(global2local(r));
  }

  validate_self(r);
}

void viewport::refresh(region &occluded)
{
  redraw(occluded);
}

void viewport::validate(const rect &_r)
{
  for (auto child : children) {
    child->validate(r);
  }

  validate_self(r);
}

void viewport::validate_self(const rect &)
{
}

void viewport::refresh_self(const rect &)
{
}


region viewport::dirty_region()
{
  return region();
}

