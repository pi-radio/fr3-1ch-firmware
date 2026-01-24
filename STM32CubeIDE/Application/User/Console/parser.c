/*
 * parser.c
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */
#include "parser.h"
#include "lexer.h"
#include "terminal.h"

#include <config_data.h>

// LL(1) for now -- go to LALR(1) only if needed

static int parse_statement_end() {
  token_t cur_tok;

  get_token(&cur_tok);

  if (cur_tok.token_type != TOK_EOL) {
    return PARSER_SYNTAX_ERROR;
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
      return PARSER_SYNTAX_ERROR;
    }

    hw = config_read_word(cur_tok.i);

    printf("Config[%d]: %4.4x\n", cur_tok.i, hw);

    return PARSER_OK;
  }
  default:
  }

  return PARSER_SYNTAX_ERROR;
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
    return PARSER_SYNTAX_ERROR;
  }
}


int parser_parse_statement()
{
  token_t cur_tok;

  get_token(&cur_tok);

  while (cur_tok.token_type == TOK_EOL) {
    get_token(&cur_tok);
  }

  switch(cur_tok.token_type) {
    case TOK_CONFIG:
      return parse_config_statement();
    case TOK_REDRAW:
      force_redraw = 1;
      return parse_statement_end();
    default:
      return PARSER_SYNTAX_ERROR;
  }


}
