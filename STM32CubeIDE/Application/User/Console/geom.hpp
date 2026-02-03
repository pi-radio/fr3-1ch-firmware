/*
 * geom.hpp
 *
 *  Created on: Feb 2, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_GEOM_HPP_
#define APPLICATION_USER_CONSOLE_GEOM_HPP_

#include <stdint.h>
#include <memory.h>
#include <list>
#include <ranges>
#include <numeric>
#include <memory>

class irange
{
private:
  int iter;
  int last;

public:
  irange(int start, int end) : iter(start), last(end) { }

  const irange &begin() const { return *this; }
  const irange &end() const { return *this; }

  bool operator != (const irange &) const {
    return iter < last;
  }

  void operator++() { ++iter; }
  int operator*() { return iter; }
};

template <typename T> T min(T a, T b)
{
  return (a < b) ? a : b;
}

template <typename T> T max(T a, T b)
{
  return (a > b) ? a : b;
}

typedef int32_t ord_t;

struct position {
  ord_t row;
  ord_t col;

  position(ord_t r = 0, ord_t c = 0) : row(r), col(c) { };

  bool operator == (const position &p) const { return (row == p.row) && (col == p.col); }

  position &operator +=(const position &p) {
    row += p.row;
    col += p.col;

    return *this;
  }


};

static inline position operator +(const position &p1, const position &p2)
{
  return position(p1.row + p2.row, p1.col + p2.col);
}

static inline position operator -(const position &p1, const position &p2)
{
  return position(p1.row - p2.row, p1.col - p2.col);
}

struct size {
  ord_t height;
  ord_t width;

  size() : height(0), width(0) { };
  size(ord_t h, ord_t w) : height(h), width(w) { };

  bool operator == (const size &s) const { return (width == s.width) && (height == s.height); }


  inline uint32_t area() { return height * width; };
};

extern const position pzero;
extern const size szero;

struct region;
class viewport;

struct rect {
  position p;
  size s;

  rect(ord_t top, ord_t left, ord_t bottom, ord_t right) : p(top, left), s(bottom - top, right - left) { };

  rect(const position &_p = pzero, const size &_s = szero) : p(_p), s(_s) {
    if (s.height < 0) {
      p.row = p.row + s.height;
      s.height = -s.height;
    }

    if (s.width < 0) {
      p.col = p.col + s.width;
      s.width = -s.width;
    }
  };

  bool disjoint(const rect &r) const {
    if ((bottom() < r.top()) ||
        (top() > r.bottom()) ||
        (left() > r.right()) ||
        (right() < r.left())) {
      return true;
    }

    return false;
  }

  bool overlaps(const rect &r) const {
    return (left() < r.right()) && (right() > r.left()) &&
        (top() < r.bottom()) && (bottom() > r.top());
  }

  bool contains(const rect &r) const {
    return (left() <= r.left()) &&
        (right() >= r.right()) &&
        (top() <= r.top()) &&
        (bottom() >= r.bottom());
  }

  std::shared_ptr<region> operator +(const rect &) const;
  std::shared_ptr<region> operator -(const rect &) const;

  inline bool empty() const { return (s.height == 0) || (s.width == 0); }

  inline ord_t height() const { return s.height; }
  inline ord_t width() const { return s.width; }

  inline ord_t top() const { return p.row; }
  inline ord_t bottom() const { return p.row + s.height; }
  inline ord_t left() const { return p.col; }
  inline ord_t right() const { return p.col + s.width; }

  inline ord_t area() { return s.area(); }

  inline auto lines() const { return irange(top(), bottom()); }

  inline void trim_top(ord_t n) {
    n = min(n, s.height);
    p.row += n;
    s.height -= n;
  }

  inline rect trim_top_to(ord_t row) {
    rect retval;

    if (row > top()) {
      uint32_t h = row - p.row;
      retval = rect(top(), left(), h, right());
      s.height -= h;
      p.row += h;
    }

    return retval;
  }


  inline rect trim_bottom_to(ord_t row) {
    rect retval;

    if (row < bottom()) {
      uint32_t h = bottom() - row;
      retval = rect(bottom(), left(), h, right());
      s.height -= h;
    }

    return retval;
  }


  inline void trim_bottom(ord_t n) {
    n = min(n, s.height);
    s.height -= n;
  }

  inline void trim_left(ord_t n) {
    n = min(n, s.width);
    p.col += n;
    s.width -= n;
  }

  inline void trim_right(ord_t n) {
    n = min(n, s.width);
    s.width -= n;
  }

  inline bool operator==(const rect &r) const {
    return (p == r.p) && (s == r.s);
  }

  inline rect operator&(const rect &r) const {
    return rect(max(top(), r.top()),
        max(left(), r.left()),
        min(bottom(), r.bottom()),
        min(right(), r.right()));
  }
};



struct region {
  std::list<rect> rects;

  region() {};
  region(const rect &r) { rects.emplace_front(r); }
  region(const region &r) : rects(r.rects) {}

  // To add a non-overlapping region
  void add(const rect &r) { if (!r.empty()) rects.emplace_back(r); }

  region &operator +=(const rect &);
  region &operator +=(const region &);

  region &operator -=(const rect &);
  region &operator -=(const region &);

  inline bool empty() { return rects.empty(); }

  inline void clear() { rects.clear(); }
};

static inline rect operator +(const rect &r, const position &_p) { return rect(r.p + _p, r.s); }
static inline rect operator -(const rect &r, const position &_p) { return rect(r.p - _p, r.s); }


static inline region operator-(const rect &_rect, const region &_region)
{
  region retval(_rect);

  retval -= _region;

  return retval;
}

region operator&(const rect &_rect, const region &_region);

#endif /* APPLICATION_USER_CONSOLE_GEOM_HPP_ */
