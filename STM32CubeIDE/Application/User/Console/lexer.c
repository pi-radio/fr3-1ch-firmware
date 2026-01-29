#include "app_threadx.h"

#include "console.h"
#include "lexer.h"

#include <ctype.h>
#include <strings.h>

struct keyword {
  const char *kw;
  int val;
};

struct keyword keywords[] = {
    { "spiw", TOK_SPIW },
    { "erase", TOK_ERASE },
    { "config", TOK_CONFIG },
    { "read", TOK_READ },
    { "redraw", TOK_REDRAW },
    { "lmx", TOK_LMX },
    { "prog", TOK_PROG },
    { "write", TOK_WRITE },
    { "reg", TOK_REG },
    // End marker
    { NULL, 0 }
};

static char cur_line[132];
static int cur_line_len = 0;
static int pos;

static int peekchar(void) {
  if (pos >= cur_line_len) {
    return 0;
  }

  return cur_line[pos];
}

static int rxchar(void) {
  if (pos >= cur_line_len) {
    return 0;
  }

  return cur_line[pos++];
}


void lexer_set_line(const char *s, int len)
{
  // TODO add lock
  memcpy(cur_line, s, len);
  pos = 0;
  cur_line_len = len;
}


token_t _peek_token;

int token_pushc(token_t *tok, char c)
{
  if (tok->s.len >= sizeof(tok->s.str) - 1) {
    return -1;
  }

  tok->s.str[tok->s.len++] = c;

  return 0;
}

void get_id(token_t *tok)
{
  struct keyword *curkw;

  tok->token_type = TOK_ID;

  token_pushc(tok, rxchar());

  // Maybe set an error flag for overflow?  Not the greatest implementation
  while ((isalnum(peekchar()) || peekchar() == '_')) {
    token_pushc(tok, rxchar());
  }

  tok->s.str[tok->s.len] = 0;

  curkw = keywords;

  while (curkw->kw != NULL) {
    if (strcasecmp(tok->s.str, curkw->kw) == 0) {
      tok->token_type = curkw->val;
      return;
    }

    curkw++;
  }
}

void get_float_fractional(token_t *tok, int ip) {
  char c;

  // c == '.'
  c = rxchar();
}

void get_float_exponent(token_t *tok) {

}

void get_number(token_t *tok)
{
  char c;
  int neg = 0;

  c = rxchar();

  if (c == '-') {
    neg = 1;
    c = rxchar();
  }

  if (c == '0') {
    c = peekchar();

    if (c == 'x') {
      // Hexadecimal number

    } else if (c >= '0' && c <= '7') {
      // Octal number

    } else if (c == '.') {
      get_float_fractional(tok, 0);
      return;
    } else if (c == 'e') {
      tok->d = 0;
      get_float_exponent(tok);
    } else {
      tok->token_type = TOK_INT;
      tok->i = 0;
      return;
    }
  } else {
    tok->i = c - '0';

    while (1) {
      int c = peekchar();

      if (c >= '0' && c <= '9') {
        rxchar();
        tok->i = tok->i * 10 + (c - '0');
      } else if (c == '.') {
        get_float_fractional(tok, tok->i);
        return;
      } else if (c == 'e') {
        tok->d = tok->i;
        get_float_exponent(tok);
      } else {
        tok->token_type = TOK_INT;
        return;
      }
    }
  }
}

void get_string_esc(token_t *tok)
{
  char c;

  c = rxchar();

  switch(c) {
  case '"':
  case '\\':
    token_pushc(tok, c);
    return;

  case 'n':
    token_pushc(tok, '\n');
    return;

  case 'r':
    token_pushc(tok, '\r');
    return;

  case 't':
    token_pushc(tok, '\t');
    return;

  case 'x':
  {
    int c1, c2;
    c1 = rxchar();
    c2 = rxchar();

    token_pushc(tok, ((c1 - '0') << 4) + (c2 - '0'));
    return;
  }


  default:
    return;
  }
}

void get_string(token_t *tok)
{
  char c;

  // Get first "
  rxchar();

  while(1) {
    c = rxchar();

    if (c == '"') {
      break;
    } else if (c == '\\') {
      get_string_esc(tok);
    }
  }
}

void _get_token(token_t *tok)
{
  int c = peekchar();

  memset(tok, 0, sizeof(*tok));

  while (isspace(c) && c != '\n' && c != '\r') {
    rxchar();

    c = peekchar();
  }

  if (c == '\n' || c == 0) {
    rxchar();
    tok->token_type = TOK_EOL;
  } else if (c == '\r') {
    rxchar();
    tok->token_type = TOK_EOL;

    if (peekchar() == '\n') {
      rxchar();
    }
  } else if (isalpha(c) || c == '_') {
    get_id(tok);
  } else if (isdigit(c) || c == '-') {
    get_number(tok);
  } else if (c == '"') {
    get_string(tok);
  } else {
    tok->s.str[0] = rxchar();
    tok->s.str[1] = 0;
    tok->s.len = 1;
    tok->token_type = TOK_ERROR;
  }
}

void peek_token(token_t *tok)
{
  if (_peek_token.token_type != TOK_NONE) {
    _get_token(&_peek_token);
  }

  memcpy(tok, &_peek_token, sizeof(*tok));
}

void get_token(token_t *tok)
{
  if (_peek_token.token_type != TOK_NONE) {
    memcpy(tok, &_peek_token, sizeof(*tok));
    _peek_token.token_type = TOK_NONE;
    return;
  }

  _get_token(tok);
}

void init_lexer(void)
{
  _peek_token.token_type = TOK_NONE;
}
