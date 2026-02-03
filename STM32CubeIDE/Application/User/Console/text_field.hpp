/*
 * text_field.h
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_TEXT_FIELD_HPP_
#define APPLICATION_USER_CONSOLE_TEXT_FIELD_HPP_

#include <window.hpp>

struct text_field_callbacks {
  virtual void beep() {};
  virtual void on_nl(const uint8_t *, size_t) {};
  virtual void on_cr(const uint8_t *, size_t) {};
};

class text_field : public window {
  text_field_callbacks *_callbacks;

public:
  text_field(viewport *parent, const rect &_R);

  void set_callbacks(text_field_callbacks *callbacks)  { _callbacks = callbacks; }

  int on_input(int c) override;
};

#endif /* APPLICATION_USER_CONSOLE_TEXT_FIELD_HPP_ */
