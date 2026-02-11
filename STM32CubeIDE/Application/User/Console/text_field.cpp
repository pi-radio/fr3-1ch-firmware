/*
 * text_field.c
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */
#include <text_field.hpp>

text_field::text_field(viewport *parent, const rect &r) :
  window(parent, r),
  _callbacks(NULL)
{

}

int text_field::on_input(int c)
{
  if (c == '\n') {
    if (_callbacks) {
      _callbacks->on_nl(bufat(0, 0), cursor.col);
    }
    clearline(0);
    cr();
  } else if (c == '\r') {
    if (_callbacks) {
      _callbacks->on_cr(bufat(0, 0), cursor.col);
    }

    clearline(0);
    cr();
  } else if (c == '\b' || c == 0x7F) {
    if (cursor.col == 0) {
      if (_callbacks) {
        _callbacks->beep();
      }
      return 0;
    }

    pcur--;
    *pcur = 0;
    cursor.col--;
    dirtypt();
  } else if (c >= 0x20 && c < 0x7F) {
    putc(c);
    return 0;
  } else {
    printf("Unknown char 0x%2.2x\n", c);
    return -1;
  }

  return 0;
}
