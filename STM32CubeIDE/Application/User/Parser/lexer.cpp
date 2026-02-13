#include "app_threadx.h"

#include "console.h"

#include <ctype.h>
#include <strings.h>

#include <cmath>
#include <map>
#include <span>

#include <algorithm>

#define DEFINE_TOKENS

#include "lexer.h"


using namespace parser;

std::map<const std::string, token_t> __keywords;

#define MAKE_KEYWORD(x)  token_t parser::keywords::x(new keyword(mklower(std::string{#x})))
#include <keywords.h>

token_t EOL(new _EOL());

keyword::keyword(const std::string &_kw) : token(KEYWORD, _kw)
{
  __keywords.emplace(_kw, this);
}


token_t _tokenizer::peek_token()

{
  return tq.front();
}

token_t _tokenizer::get_token()
{
  if (tq.empty()) {
    return EOL;
  }

  token_t retval = tq.front();
  tq.pop();
  return retval;
}

int get_octal_str(std::string::const_iterator &cur)
{
  int i = 0;

  while (*cur >= '0' && *cur <= '7') {
    i = i * 8 + (*cur++ - '0');
  }

  return i;
}

int get_decimal_str(std::string::const_iterator &cur)
{
  int i = 0;

  while (*cur >= '0' && *cur <= '9') {
    i = i * 10 + (*cur++ - '0');
  }

  return i;
}

token_t get_number(std::string::const_iterator &cur)
{
  int neg = 1;
  bool isfloat = false;
  int i;
  double d;

  if (*cur == '-') {
    neg = -1;
    cur++;
  }

  if (*cur == '0') {
    cur++;

    if (*cur == 'x') {
      // Hexadecimal number

    } else if (*cur >= '0' && *cur <= '7') {
      // Octal number
      return token_t(new _INT(get_octal_str(cur)));
    }
  }

  d = i = neg * get_decimal_str(cur);

  if (*cur == '.') {
    cur++;
    isfloat = true;
    auto start = cur;
    d += get_decimal_str(cur) * std::pow(10, start - cur);
  }

  if (std::tolower(*cur) == 'e') {
    cur++;
    isfloat = true;
    int e = get_decimal_str(cur);
    d *= std::pow(10, e);
  }

  if (isfloat) {
    return token_t(new _FLOAT(d));
  }

  return token_t(new _INT(i));

#if 0

  else if (*cur == '.') {
        get_float_fractional(tok, 0);
        return;
      } else if (*cur == 'e') {
        tok->d = 0;
        get_float_exponent(tok);
      } else {
        return new _INT(0);
      }

  else {
    tok->i = *cur++ - '0';

    while (1) {
      if (*cur >= '0' && *cur <= '9') {
        tok->i = tok->i * 10 + (*cur++ - '0');
      } else if (*cur == '.') {
        get_float_fractional(tok, tok->i);
        return;
      } else if (c == 'e') {
        tok->d = tok->i;
        get_float_exponent(tok);
      } else {
        tok->token_type = TOK_INT;
        tok->i = neg * tok->i;
        return;
      }
    }
  }
#endif
}

token_t get_id(std::string::const_iterator &cur)
{
  std::string s;

  s += *cur++;

  // Maybe set an error flag for overflow?  Not the greatest implementation
  while ((isalnum(*cur) || *cur == '_')) {
    s += *cur++;
  }

  auto it = __keywords.find(mklower(s));

  if (it != __keywords.end()) {
    return token_t(it->second);
  }

  return token_t(new _STR(s));
}

void _tokenizer::set_line(const std::string &line)
{
  auto cur = line.begin();

  while (!tq.empty()) tq.pop();

  while (cur != line.end()) {
    if (isspace(*cur)) {
      cur++;
      continue;
    }

    if (isalpha(*cur) || *cur == '_') {
      tq.push(get_id(cur));
    } else if (isdigit(*cur) || *cur == '-') {
      tq.push(get_number(cur));
    } else {
      tq.push(token_t(new _ERROR()));
      return;
    }
  }
}

_tokenizer parser::tokenizer;

extern "C" int getentropy(void *buffer, size_t length)
{
  return -ENOSYS;
}


#if 0



token _peek_token;

int token_pushc(token *tok, char c)
{
  if (tok->s.len >= sizeof(tok->s.str) - 1) {
    return -1;
  }

  tok->s.str[tok->s.len++] = c;

  return 0;
}



void get_float_fractional(token *tok, int ip) {
  char c;

  // c == '.'
  c = rxchar();
}

void get_float_exponent(token *tok) {

}


void get_string_esc(token *tok)
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

void get_string(token *tok)
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

void _get_token(token *tok)
{
  int c = peekchar();

  memset(tok, 0, sizeof(*tok));

  while (isspace(c) && c != '\n' && c != '\r') {
    rxchar();

    c = peekchar();
  }

}

void peek_token(token *tok)
{
  if (_peek_token.token_type != TOK_NONE) {
    _get_token(&_peek_token);
  }

  memcpy(tok, &_peek_token, sizeof(*tok));
}

EXTERN_C void get_token(token *tok)
{
  if (_peek_token.token_type != TOK_NONE) {
    memcpy(tok, &_peek_token, sizeof(*tok));
    _peek_token.token_type = TOK_NONE;
    return;
  }

  _get_token(tok);
}

EXTERN_C void init_lexer(void)
{
  _peek_token.token_type = TOK_NONE;
}
#endif
