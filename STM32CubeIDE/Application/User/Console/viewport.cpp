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
  rect rr = _r;

  if (rr.empty()) {
    return;
  }

  parent()->render_buffer(local2parent(rr), buf, stride);
}

void viewport::render(const rect &r)
{

}

void viewport::redraw(region &occluded)
{
  dirty_all();

  refresh(occluded);
}

void viewport::dirty()
{

}

void viewport::dirty_all()
{
  for (auto pchild : children) {
    pchild->dirty_all();
  }

  dirty();
}


void viewport::refresh(region &occluded)
{
  for (auto pchild : children) {
    pchild->refresh(occluded);
  }

  region draw_rects = local2global(rlocal()) - occluded;

  occluded += r;

  for (auto r : draw_rects.rects) {
    render(global2local(r));
  }

  validate_self(r);
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

