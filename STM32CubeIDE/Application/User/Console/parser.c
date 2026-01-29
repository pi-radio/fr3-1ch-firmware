/*
 * parser.c
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */

#include <stdio.h>
#include <stdarg.h>


#include <parser.h>
#include <lexer.h>
#include <terminal.h>
#include <lmx.h>
#include <config_data.h>

char parser_error[128];

const char *parser_get_error_string(void)
{
  return parser_error;
}

void parser_set_error(const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);

  vsnprintf(parser_error, 127, fmt, args);
  parser_error[128] = 0;

  va_end(args);
}

int syntax_error(void)
{
  parser_set_error("Syntax Error");
  return PARSER_SYNTAX_ERROR;
}

// LL(1) for now -- go to LALR(1) only if needed

static int parse_statement_end() {
  token_t cur_tok;

  get_token(&cur_tok);

  if (cur_tok.token_type != TOK_EOL) {
    return syntax_error();
  }

  return PARSER_OK;
}

static int parse_config_read()
{
  token_t cur_tok;

  get_token(&cur_tok);

  switch (cur_tok.token_type) {
  case TOK_INT:
  {
    uint16_t hw;

    if (parse_statement_end() != PARSER_OK) {
      return syntax_error();
    }

    hw = config_read_word(cur_tok.i);

    printf("Config[%d]: %4.4x\n", cur_tok.i, hw);

    return PARSER_OK;
  }
  default:
  }

  return syntax_error();
}

static int parse_config_statement()
{
  token_t cur_tok;

  get_token(&cur_tok);

  switch (cur_tok.token_type) {
  case TOK_ERASE:
    if (parse_statement_end() != PARSER_OK) {
      return PARSER_SYNTAX_ERROR;
    }
    // Erase user config
    erase_config_data();
    return PARSER_OK;

  case TOK_READ:
    return parse_config_read();

  default:
    return syntax_error();
  }
}

static int parse_lmx_read() {
  int reg, result;
  uint16_t val;
  token_t cur_tok;

  get_token(&cur_tok);

  if (cur_tok.token_type != TOK_REG) {
    return syntax_error();
  }

  get_token(&cur_tok);

  if (cur_tok.token_type != TOK_INT) {
    return syntax_error();
  }

  reg = cur_tok.i;

  if (reg < 0 || reg > 122) {
    // TODO BETTER ERROR REPORTING
    parser_set_error("Invalid register %d", reg);
    return PARSER_GENERAL_ERROR;
  }


  if (parse_statement_end() != PARSER_OK) {
    return syntax_error();
  }

  result = lmx_read_reg(reg, &val);

  if (result == 0) {
    printf("LMX reg %d: %04x\n", reg, val);
  } else {
    printf("Failed to read reg: %d\n", result);
  }

  return PARSER_OK;

}


static int parse_lmx_write() {
  int reg, val;
  token_t cur_tok;

  get_token(&cur_tok);

  if (cur_tok.token_type != TOK_REG) {
    return syntax_error();
  }

  get_token(&cur_tok);

  if (cur_tok.token_type != TOK_INT) {
    return syntax_error();
  }

  reg = cur_tok.i;

  if (reg < 0 || reg > 122) {
    // TODO BETTER ERROR REPORTING
    parser_set_error("Invalid register %d", reg);
    return PARSER_GENERAL_ERROR;
  }

  get_token(&cur_tok);

  if (cur_tok.token_type != TOK_INT) {
    return syntax_error();
  }

  val = cur_tok.i;

  if (parse_statement_end() != PARSER_OK) {
    return syntax_error();
  }

  return PARSER_OK;
}

static int parse_lmx_statement() {
  token_t cur_tok;

  get_token(&cur_tok);

  switch (cur_tok.token_type) {
  case TOK_PROG:
    return PARSER_SYNTAX_ERROR;

  case TOK_WRITE:
    return parse_lmx_write();

  case TOK_READ:
    return parse_lmx_read();

  default:
  }

  return syntax_error();
}

int _parser_parse_statement()
{
  token_t cur_tok;

  parser_set_error("NOT SET");

  get_token(&cur_tok);

  while (cur_tok.token_type == TOK_EOL) {
    get_token(&cur_tok);
  }

  switch(cur_tok.token_type) {
    case TOK_CONFIG:
      return parse_config_statement();

    case TOK_LMX:
      return parse_lmx_statement();

    case TOK_REDRAW:
      force_redraw = 1;
      return parse_statement_end();

    default:
  }

  return syntax_error();
}

int parser_parse_statement()
{
  int retval = _parser_parse_statement();

  if (retval != PARSER_OK) {
    token_t cur_tok;

    do {
      get_token(&cur_tok);
    } while (cur_tok.token_type != TOK_EOL);
  }

  return retval;
}
