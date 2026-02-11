/*
 * tbbase.h
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_VIEWPORT_HPP_
#define APPLICATION_USER_CONSOLE_VIEWPORT_HPP_

#include <geom.hpp>

class viewport {
  viewport *_parent;
  std::list<viewport *> children;
  viewport *_focus;

protected:
  position p;
  size s;

  viewport(viewport *parent, position _p, size _s) : _parent(parent),
      _focus(NULL), p(_p), s(_s) { }

  viewport(viewport *parent, rect _r) : viewport(parent, _r.p, _r.s) {}

public:
  inline rect local2global(const rect &_r) const { if (_parent) return _parent->local2global(_r + p); return _r; }
  inline rect global2local(const rect &_r) const { if (_parent) return _parent->global2local(_r) - p; return _r; }
  inline rect local2parent(const rect &_r) const { return _r + p; }
  inline rect parent2local(const rect &_r) const { return _r - p; }

  inline rect rparent() const { return rect(p, s); }
  inline rect rlocal() const { return rect(pzero, s); }
  inline rect rglobal() const { return local2global(rlocal()); }

  virtual uint8_t *_bufat(position) = 0;
  uint8_t *_bufat(uint32_t row, uint32_t col) { return _bufat(position(row, col)); };
  uint8_t *_bufatline(uint32_t row) { return _bufat(row, (uint32_t)0); }

  inline size get_size() { return s; }
  inline uint32_t rows() { return s.height; }
  inline uint32_t cols() { return s.width; }

  viewport *parent() const { return _parent; }

  inline void set_focus(viewport *win) {
    _focus = win;
  }

  virtual int on_input(int c);

  const uint8_t *bufat(uint32_t r, uint32_t c) { return _bufat(position(r, c)); };
  const uint8_t *bufat(position p) { return _bufat(p); };

  template <class T> T *create(ord_t start_row, ord_t start_col, ord_t height, ord_t width) {
      T *retval = new T(this, rect(position(start_row, start_col), size(height, width)));

      children.push_back(retval);

      return retval;
  }

  virtual uint8_t *alloc_buffer(size_t s);

  virtual size_t stride() const = 0;

  virtual rect dirty_rect();


  virtual void render_buffer(const rect &, const uint8_t *, const size_t &);

  virtual void validate_self(const rect &);
  virtual void refresh_self(const rect &);

  void validate(const rect &);
  void redraw(std::vector<rect> &occluded);
  void refresh(std::vector<rect> &occluded);

  inline bool is_dirty() { return !dirty_rect().empty(); }
  virtual void dirty();
  virtual void dirty_all();

  void redraw() { std::vector<rect> occluded; redraw(occluded); }
  void refresh() { std::vector<rect> occluded; refresh(occluded); }
};

#endif /* APPLICATION_USER_CONSOLE_VIEWPORT_HPP_ */
