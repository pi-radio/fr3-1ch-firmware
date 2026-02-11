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

void viewport::redraw(std::vector<rect> &occluded)
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


void viewport::refresh(std::vector<rect> &occluded)
{
  for (auto pchild : children) {
    if (!pchild->is_dirty()) {
      continue;
    }

    pchild->refresh(occluded);
  }

  // TODO -- handle occlusion
  refresh_self(dirty_rect());

  occluded.emplace_back(rglobal());

  validate_self(rlocal());
}

void viewport::validate(const rect &_r)
{
  for (auto child : children) {
    child->validate(_r);
  }

  validate_self(_r);
}

void viewport::validate_self(const rect &)
{
}

void viewport::refresh_self(const rect &)
{
}


rect viewport::dirty_rect()
{
  return rect();
}

