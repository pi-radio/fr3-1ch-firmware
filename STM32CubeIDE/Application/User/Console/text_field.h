/*
 * text_field.h
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_TEXT_FIELD_H_
#define APPLICATION_USER_CONSOLE_TEXT_FIELD_H_

#include <window.h>

struct text_field {
  struct window win;

  int (*on_nl)(struct text_field *, const char *s, int l);
  int (*on_cr)(struct text_field *, const char *s, int l);
};


extern int text_field_on_input(struct window *, int);

extern int create_text_field(struct text_field *tf, struct termbuf *tbuf, int start_row, int start_col, int width, int height);

#endif /* APPLICATION_USER_CONSOLE_TEXT_FIELD_H_ */
