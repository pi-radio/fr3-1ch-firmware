/*
 * vtparser.cpp
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */
#include <consolexx/cooked.hpp>
#include <consolexx/vtparser.hpp>

using namespace consolexx;

vtparser::vtparser(cooked_terminal *_term) :
  term(_term),
  state(TERMINAL_STATE_GROUND),
  saved_state(TERMINAL_STATE_GROUND),
  csi_count(0),
  error_count(0)
{

}

void vtparser::set_state(terminal_state_t _state)
{
  state = _state;

  switch (state) {
  case TERMINAL_STATE_CSI_ENTRY:
    csi.reset();
    break;
  default:
    break;
  }
}

void vtparser::handle_vt100_esc(int c)
{
  set_state(TERMINAL_STATE_GROUND);
}

void vtparser::handle_error(int c) {
  error_count++;
  error_char = c;
  error_state = state;

  set_state(TERMINAL_STATE_GROUND);
}

void vtparser::handle_csi() {
  set_state(TERMINAL_STATE_GROUND);
  csi_count++;

  if (csi.parameters.size() == 0) {
    // Default 0 parameter
    csi.parameters.push_back(0);
  }

  switch (csi.command) {
  case 'c':
    printf("Terminal type: %d %d (%d %d)\n",
        csi.parameters[0],
        csi.parameters[1],
        csi.parameters.size(),
        csi.intermediates.size());
    break;

  case 'R':
    printf("Cursor is at %d, %d\n", csi.parameters[0], csi.parameters[1]);
    break;

  case 'A':
    term->on_input(UP);
    break;
  case 'B':
    term->on_input(DOWN);
    break;
  case 'C':
    term->on_input(RIGHT);
    break;
  case 'D':
    term->on_input(LEFT);
    break;
  default:
    printf("CSI complete: %c n_p: %d n_i: %d\n",
        csi.command,
        csi.parameters.size(),
        csi.intermediates.size());
  }
}

void vtparser::handle_ground(int c)
{
  if (c == 0x1B) {
    set_state(TERMINAL_STATE_ESC);
  } else if (c == 0x9B) {
    set_state(TERMINAL_STATE_CSI_ENTRY);
  } else if (c == 0x10) {
    saved_state = TERMINAL_STATE_GROUND;
    set_state(TERMINAL_STATE_DLE);
  } else {
    term->on_input(c);
  }
}


void vtparser::handle_esc(int c)
{
  if (c == '[') {
    set_state(TERMINAL_STATE_CSI_ENTRY);
  } else if (c >= 0x30 && c <= 0x7F) {
    handle_vt100_esc(c);
    set_state(TERMINAL_STATE_GROUND);
  } else {
    // Invalid char callback
    handle_error(c);
  }
}

void vtparser::handle_csi_inter(int c)
{
  if (c >= 0x20 && c <= 0x2F) {
    csi.intermediates.push_back(c);
  } else if (c >= 0x40 && c <= 0x7E) {
    csi.command = c;
    vtparser::handle_csi();
  } else {
    vtparser::handle_error(c);
  }
}

void vtparser::handle_csi_param(int c)
{
  if (c >= '0' && c <= '9') {
    int v = *(--csi.parameters.end());

    *(--csi.parameters.end()) = 10 * v + (c - '0');
  } else if (c == ';') {
    csi.parameters.push_back(0);
  } else if (c >= 0x20 && c <= 0x2F) {
    vtparser::set_state(TERMINAL_STATE_CSI_INTER);
    vtparser::handle_csi_inter(c);
  } else if (c >= 0x40 && c <= 0x7E) {
    csi.command = c;
    vtparser::handle_csi();
  } else {
    vtparser::handle_error(c);
  }
}

void vtparser::handle_csi_entry(int c)
{
  if (c >= 0x3C && c <= 0x3F) {
    csi.private_leader = c;
    csi.parameters.push_back(0);
    vtparser::set_state(TERMINAL_STATE_CSI_PARAM);
  } else if (c >= '0' && c <= '9') {
    vtparser::set_state(TERMINAL_STATE_CSI_PARAM);
    csi.parameters.push_back(0);
    vtparser::handle_csi_param(c);
  } else if (c >= 0x20 && c <= 0x2F) {
    vtparser::set_state(TERMINAL_STATE_CSI_INTER);
    vtparser::handle_csi_inter(c);
  } else if (c >= 0x40 && c <= 0x7F) {
    csi.command = c;
    vtparser::handle_csi();
  } else {
    vtparser::handle_error(c);
  }
}

void vtparser::handle_dle(int c)
{
  if ((c == 0x02) && (saved_state == TERMINAL_STATE_GROUND)) {
    command = "";
    set_state(TERMINAL_STATE_TEXT);
  } else if ((c == 0x03) && (saved_state == TERMINAL_STATE_TEXT)) {
    set_state(TERMINAL_STATE_GROUND);

    term->emit<input_event>(command);
  } else {
    handle_error(c);
  }
}

void vtparser::handle_text(int c)
{
  if (c == 0x10) {
      saved_state = TERMINAL_STATE_TEXT;
      set_state(TERMINAL_STATE_DLE);
  }

  command.push_back(c);
}

void vtparser::process(int c)
{
  switch(state) {
  case TERMINAL_STATE_GROUND:
    handle_ground(c);
    break;
  case TERMINAL_STATE_ESC:
    handle_esc(c);
    break;
  case TERMINAL_STATE_CSI_INTER:
    handle_csi_inter(c);
    break;
  case TERMINAL_STATE_CSI_PARAM:
    handle_csi_param(c);
    break;
  case TERMINAL_STATE_CSI_ENTRY:
    handle_csi_entry(c);
    break;
  case TERMINAL_STATE_DLE:
    handle_dle(c);
    break;
  case TERMINAL_STATE_TEXT:
    handle_text(c);
    break;
  default:
    handle_error(c);
  }

}
