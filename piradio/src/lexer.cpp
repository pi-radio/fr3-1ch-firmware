#include "app_threadx.h"

#include "console.h"

#include <ctype.h>
#include <strings.h>

#include <cmath>
#include <map>
#include <span>

#include <algorithm>

#define DEFINE_TOKENS

#include <threadxx/dbgstream.hpp>
#include <piradio/parser.hpp>

using namespace parser;


std::map<const std::string, token_t> __keywords;

#define MAKE_KEYWORD(x)  const token_t keywords::x(new keyword(#x));
LIST_OF_KEYWORDS
#undef MAKE_KEYWORD

token_t EOL(new _EOL());

std::string mklower(const std::string &a)
{
  std::string retval;

  retval.resize(a.size());

  std::transform(a.begin(), a.end(), retval.begin(),
      [](unsigned char c){ return std::tolower(c); });

  return retval;
}

keyword::keyword(const std::string &_kw) : token(token_type::KEYWORD, mklower(_kw))
{
  __keywords.emplace(s, this);
}


token_t _tokenizer::peek_token()

{
  return tq.front();
}

token_t _tokenizer::get_token()
{
  dbg::dbgout << "tq size 1: " << tq.size() << std::endl;

  if (tq.empty()) {
    return EOL;
  }

  token_t retval = tq.front();
  tq.pop();

  dbg::dbgout << "tq size 2: " << tq.size() << std::endl;

  return retval;
}

void _tokenizer::push_token(token_t tok)
{
  tq.push(tok);
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

  return token_t(new _ID(s));
}

void _tokenizer::set_line(const std::string &line)
{
  auto cur = line.begin();

  dbg::dbgout << "Command: " << line << std::endl;

  while (!tq.empty()) tq.pop();

  while (cur != line.end()) {
    if (isspace(*cur)) {
      cur++;
      continue;
    }

    if (isalpha(*cur) || *cur == '_') {
      push_token(get_id(cur));
    } else if (isdigit(*cur) || *cur == '-') {
      push_token(get_number(cur));
    } else {
      push_token(token_t(new _ERROR()));
      return;
    }
  }
}

extern "C" int getentropy(void *buffer, size_t length)
{
  return -ENOSYS;
}

