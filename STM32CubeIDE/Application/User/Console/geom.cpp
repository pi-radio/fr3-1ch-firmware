/*
 * geom.cpp
 *
 *  Created on: Feb 2, 2026
 *      Author: zapman
 */
#include <geom.hpp>

const position pzero(0UL,0UL);
const size szero(0UL,0UL);

std::shared_ptr<region> rect::operator +(const rect &r) const
{
  std::shared_ptr<region> retval(new region());

  if (disjoint(r)) {
    retval->add(*this);
    retval->add(r);
    return retval;
  }

  if (contains(r)) {
    retval->add(*this);
    return retval;
  }

  if (r.contains(*this)) {
    retval->add(r);
    return retval;
  }

  rect r1(*this);
  rect r2(r);
  rect r3;

  r3 = r1.trim_top_to(r2.top());

  if (!r3.empty()) {
    retval->add(r3);
  }

  r3 = r2.trim_top_to(r1.top());

  if (!r3.empty()) {
    retval->add(r3);
  }

  r3 = r1.trim_bottom_to(r2.bottom());

  if (!r3.empty()) {
    retval->add(r3);
  }

  r3 = r2.trim_bottom_to(r1.bottom());

  if (!r3.empty()) {
    retval->add(r3);
  }

  r3 = rect(r1.top(), min(r1.left(), r2.left()),
      r1.bottom(), max(r1.right(), r2.right()));

  if (!r3.empty()) {
    retval->add(r3);
  }

  return retval;
}


std::shared_ptr<region> rect::operator -(const rect &r2) const
{
  rect r1(*this);

  if ((r1.bottom() <= r2.top()) ||
      (r1.top() >= r2.bottom()) ||
      (r1.left() >= r2.right()) ||
      (r1.right() <= r2.left())) {
    return std::shared_ptr<region>(new region(r1));
  }

  std::shared_ptr<region> retval(new region());

  if (r1.top() < r2.top()) {
    rect r(r1.top(), r1.left(), r2.top(), r1.right());
    retval->add(r);
    r1.trim_top(r.height());
  }

  if (r1.bottom() > r2.bottom()) {
    rect r(r2.bottom(), r1.left(), r1.bottom(), r1.right());
    retval->add(r);
    r1.trim_bottom(r.height());
  }

  if (r1.left() < r2.left()) {
    rect r(r1.top(), r1.left(), r1.bottom(), r2.left());
    retval->add(r);
    r1.trim_left(r.width());
  }

  if (r2.right() < r1.right()) {
    rect r(r1.top(), r2.right(), r1.bottom(), r1.right());
    retval->add(r);
    r1.trim_right(r.width());
  }

  return retval;
}

region &region::operator +=(const rect &_r)
{
  rect r;
  std::list<rect> new_rects;

  new_rects.push_back(_r);

  while (!new_rects.empty()) {
    bool add = true;

    r = new_rects.front();
    new_rects.pop_front();

    for (auto it = rects.begin(); it != rects.end(); ) {
      if (r.disjoint(*it)) {
        it++;
        continue;
      }

      add = false;

      std::shared_ptr<region> reg = r + *it;

      it = rects.erase(it);

      for (auto r2 : reg->rects) {
        new_rects.push_back(r2);
      }

      break;
    }

    if (add) {
      rects.push_back(r);
    }
  }

  return *this;
}


region &region::operator +=(const region &r)
{
  std::list<rect> new_rects;

  for (auto r2 : r.rects) {
    *this += r2;
  }

  return *this;
}

region &region::operator -=(const rect &_r)
{
  std::list<rect> new_rects;

  for (auto it = rects.begin(); it != rects.end(); ) {
    if (!_r.overlaps(*it)) {
      it++;
      continue;
    }

    std::shared_ptr<region> reg = *it - _r;

    for (auto r2 : reg->rects) {
      new_rects.push_back(r2);
    }

    it = rects.erase(it);
  }

  for (auto r : new_rects) {
    *this += r;
  }

  return *this;
}


region &region::operator -=(const region &r)
{
  std::list<rect> new_rects;

  for (auto r2 : r.rects) {
    *this -= r2;
  }

  return *this;
}

region operator&(const rect &_rect, const region &_region)
{
  region retval;

  for (auto r : _region.rects) {
    auto r2 = _rect & r;

    if (!r2.empty()) {
      retval.add(r2);
    }
  }

  return retval;
}


#if 0
region &operator -=(const region &);
#endif
