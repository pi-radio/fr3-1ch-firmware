/*
 * vtparser.hpp
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_VTPARSER_HPP_
#define APPLICATION_USER_CONSOLE_VTPARSER_HPP_

#include <stdlib.h>
#include <memory.h>

class terminal;

typedef void (*terminal_handler_t)(struct terminal *, int);

#define TERMINAL_MAX_CSI_INTERMEDIATES  16
#define TERMINAL_MAX_CSI_PARAMETERS     16


typedef enum {
  TERMINAL_STATE_GROUND,
  TERMINAL_STATE_ESC,
  TERMINAL_STATE_CSI_ENTRY,
  TERMINAL_STATE_CSI_PARAM,
  TERMINAL_STATE_CSI_INTER,

  NUM_TERMINAL_STATES
} terminal_state_t;



class vtparser {
  struct terminal *term;
  terminal_state_t state;
  int csi_count;
  int error_count;
  int error_char;
  terminal_state_t error_state;

  terminal_handler_t handlers[NUM_TERMINAL_STATES];

  struct {
    int command;
    int private_leader;
    int n_intermediates;
    int intermediates[TERMINAL_MAX_CSI_INTERMEDIATES];
    int intermediate_overflow;
    int n_parameters;
    int parameters[TERMINAL_MAX_CSI_PARAMETERS];
    int parameter_overflow;
  } csi;

  void reset_csi() { memset(&csi, 0, sizeof(csi)); };

  void set_state(terminal_state_t _state);
  void handle_vt100_esc(int c);
  void handle_error(int c);
  void handle_csi();
  void handle_ground(int c);
  void handle_esc(int c);
  void handle_csi_inter(int c);
  void handle_csi_param(int c);
  void handle_csi_entry(int c);

public:
  vtparser(terminal *);

  void process(int c);
};

#endif /* APPLICATION_USER_CONSOLE_VTPARSER_HPP_ */
