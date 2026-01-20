#include "app_threadx.h"

#include "console.h"
#include "lexer.h"

#include <ctype.h>

void get_id(token_t *tok)
{
  int len = 0;

  tok->token_type = TOK_ID;

  tok->str[len] = rxchar();
  len++;

  // Maybe set an error flag for overflow?  Not the greatest implementation
  while ((isalnum(peekchar()) || peekchar() == '_') && (len < sizeof(tok->str) - 1)) {
    tok->str[len] = rxchar();
    len++;
  }
}

void get_number(token_t *tok)
{
}

void get_string(token_t *tok)
{

}


void get_token(token_t *tok)
{
  while (isspace(peekchar()) && peekchar() != '\n') {
    rxchar();
  }

  if (peekchar() == '\n') {
    rxchar();
    tok->token_type = TOK_EOL;
  }
  else if (isalpha(peekchar()) || peekchar() == '_') {
    get_id(tok);
  /*
  } else if (isdigit(peekchar()) || peekchar() == '-') {
    get_number(tok);
  } else if (peekchar() == '"') {
    get_string(tok);
  */
  } else {
    tok->str[0] = rxchar();
    tok->token_type = TOK_ERROR;
  }
}
