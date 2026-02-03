/*
 * vtparser.cpp
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */
#include <vtparser.hpp>
#include <terminal.hpp>

vtparser::vtparser(terminal *_term) :
  term(_term),
  state(TERMINAL_STATE_GROUND),
  csi_count(0),
  error_count(0)
{

}

void vtparser::set_state(terminal_state_t _state)
{
  state = _state;

  switch (state) {
  case TERMINAL_STATE_CSI_ENTRY:
    reset_csi();
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

  if (csi.n_parameters == 0) {
    csi.n_parameters = 1; // Default 0 parameter
  }

  switch (csi.command) {
  case 'c':
    printf("Terminal type: %d %d (%d %d)\n",
        csi.parameters[0],
        csi.parameters[1],
        csi.n_parameters,
        csi.n_intermediates);
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
    printf("CSI complete: %c n_p: %d\n", csi.command, csi.n_parameters);
  }
}

void vtparser::handle_ground(int c)
{
  if (c == 0x1B) {
    set_state(TERMINAL_STATE_ESC);
  } else if (c == 0x9B) {
    set_state(TERMINAL_STATE_CSI_ENTRY);
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
    if (!csi.intermediate_overflow) {
      if (csi.n_intermediates <= TERMINAL_MAX_CSI_INTERMEDIATES) {
        csi.intermediates[csi.n_intermediates-1] = c;
        csi.n_intermediates++;
      } else {
        csi.intermediate_overflow = 1;
      }
    }
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
    if (!csi.parameter_overflow) {
      csi.parameters[csi.n_parameters - 1] *= 10;
      csi.parameters[csi.n_parameters - 1] += c - '0';
    }
  } else if (c == ';') {
    if (csi.n_parameters < TERMINAL_MAX_CSI_PARAMETERS) {
      csi.n_parameters++;
    } else {
      csi.parameter_overflow = 1;
    }
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
    vtparser::set_state(TERMINAL_STATE_CSI_PARAM);
  } else if (c >= '0' && c <= '9') {
    vtparser::set_state(TERMINAL_STATE_CSI_PARAM);
    csi.n_parameters = 1;
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
  default:
    handle_error(c);
  }

}
