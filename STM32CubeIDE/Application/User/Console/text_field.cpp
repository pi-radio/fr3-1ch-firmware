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
      _callbacks->on_nl(bufat(0, 0), cursor.col + 1);
    }
    clearline(0);
    cr();
  } else if (c == '\r') {
    if (_callbacks) {
      _callbacks->on_cr(bufat(0, 0), cursor.col + 1);
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

#if 0
int text_field_on_input(struct window *win, int c)
{
  struct text_field *tf = (struct text_field *)win;

  if (c == '\n') {
    if (tf->on_nl) {
      tf->on_nl(tf, wbufat(win, 0, 0), win->cur_col + 1);
    }

    wlclearline(win, 0);
    wincr(win);
  } else if (c == '\r') {
    if (tf->on_cr) {
      tf->on_cr(tf, wbufat(win, 0, 0), win->cur_col + 1);
    }

    wlclearline(win, 0);
    wincr(win);
  } else if (c == '\b' || c == 0x7F) {
    if (win->cur_col == 0) {
      txchar('\a');
      return;
    }

    win->pcur--;
    *win->pcur = 0;
    win->cur_col--;
    wldirtypt(win);
  } else if (c >= 0x20 && c < 0x7F) {
    wputc(win, c);
    return 0;
  } else {
    printf("Unknown char 0x%2.2x\n", c);
  }

  return -1;
  /*
  if (c == '\n') {
      if (term->echo) {
        lock_tx();

        if (term->onlcr) {
          txchar('\r');
        }
        txchar('\n');

        unlock_tx();
      }

      for (i = 0; i < term->input_len; i++) {
        terminal_send_rxchar(term->input_buf[i]);
      }

      terminal_send_rxchar('\n');

      term->input_len = 0;

      // EOL callback?
    } else   if (c == '\b') {
      if (term->buffer_input && (term->input_len > 0)) {
        term->input_len--;
      }
    } else if (c == 0x7F) {
      // DELETE?
      if (term->buffer_input && (term->input_len > 0)) {
        term->input_len--;
      }
    } else if (c == '\t') {
      // Check for tab callback
      // Absorb tab
    } else if (c == '\r') {
      // Check for tab callback
      if (term->echo) {
        lock_tx();

        txchar('\r');
        if (term->onlcr) {
          txchar('\n');
        }

        unlock_tx();
      }

      for (i = 0; i < term->input_len; i++) {
        terminal_send_rxchar(term->input_buf[i]);
      }

      // Assume translate
      terminal_send_rxchar('\n');

      term->input_len = 0;
    } else if (c >= 0x20 && c <= 0x7F) {
      if (term->echo) {
        lock_tx();
        txchar(c);
        unlock_tx();
      }

      if (!term->buffer_input) {
        tx_queue_send(&terminal_rx_queue, &c, TX_WAIT_FOREVER);
      } else {
        if (term->input_len < sizeof(term->input_buf)) {
          term->input_buf[term->input_len++] = c;
        } else {
          term->input_buf[sizeof(term->input_buf) - 1] = c;
        }
      }
    } else {
      // Invalid char callback
      terminal_handle_error(term, c);
    }
  */
}


int text_field_create(struct text_field *tf, struct termbuf *tbuf, int start_row, int start_col, int width, int height)
{
  int result = window_create(tbuf, &tf->win, start_row, start_col, width, height);

  if (result) {
    return result;
  }

  tf->win.on_input = text_field_on_input;
  tf->on_nl = NULL;

  return 0;
}

#endif
