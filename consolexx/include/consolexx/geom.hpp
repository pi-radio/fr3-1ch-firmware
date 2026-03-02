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
#include <vector>


#include <iterator>



template <typename T> T min(T a, T b)
{
  return (a < b) ? a : b;
}

template <typename T> T max(T a, T b)
{
  return (a > b) ? a : b;
}

typedef int32_t ord_t;

template <typename T>
struct _range {
    // Store the start and end of the range (half-open: [start, end))
    T _start;
    T _end;

    // Iterator for the range (models InputIterator)
    struct _rangeiter {
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::input_iterator_tag;

        T current; // Current value of the iterator

        // Constructor: initialize with the starting value
        explicit _rangeiter(T val) : current(val) {}

        // Dereference: return current value
        T operator*() const { return current; }

        // Prefix increment: move to next value
        _rangeiter& operator++() {
            ++current;
            return *this;
        }

        // Inequality comparison: check if iterators point to different values
        bool operator!=(const _rangeiter& other) const {
            return current != other.current;
        }
    };

    _range(T __start, T __end) : _start(__start), _end(__end) { }

    // Begin iterator: starts at 'start'
    _rangeiter begin() const { return _rangeiter(_start); }

    // End iterator: stops at 'end' (exclusive)
    _rangeiter end() const { return _rangeiter(_end); }
};

typedef _range<int> irange;
typedef _range<ord_t> ordrange;


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

class viewport;

struct rect {
  position p;
  size s;

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

  rect(ord_t top, ord_t left, ord_t bottom, ord_t right) : rect(position(top, left), size(bottom - top, right - left)) { };

  inline bool inside(const position &_p) { return (_p.row >= top()) && (_p.row < bottom()) && (_p.col >= left()) && (_p.col < right()); }

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

  static rect enclosing(const rect &r1, const rect &r2) {
    if (r1.empty()) return r2;
    if (r2.empty()) return r1;

    return rect(min(r1.top(), r2.top()),
        min(r1.left(), r2.left()),
        max(r1.bottom(), r2.bottom()),
        max(r1.right(), r2.right()));
  }

  inline rect operator +(const position &offset) const { return rect(p + offset, s); }
  inline rect operator -(const position &offset) const { return rect(p - offset, s); }

  inline void clear() { p = position(0,0); s = size(0,0); }

  inline bool empty() const { return (s.height == 0) || (s.width == 0); }

  inline ord_t height() const { return s.height; }
  inline ord_t width() const { return s.width; }

  inline ord_t top() const { return p.row; }
  inline ord_t bottom() const { return p.row + s.height; }
  inline ord_t left() const { return p.col; }
  inline ord_t right() const { return p.col + s.width; }

  inline ord_t area() { return s.area(); }

  inline auto lines() const { return ordrange(top(), bottom()); }

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

#endif /* APPLICATION_USER_CONSOLE_GEOM_HPP_ */
