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

uint32_t get_hex_str(std::string::const_iterator &cur)
{
	uint32_t v = 0;
	while(1)
	{
		if (*cur >= '0' && *cur <= '9')
		{
			v = v * 16 + (*cur++ - '0');
		}
		else if (toupper(*cur) >= 'A' && toupper(*cur) <= 'F')
		{
			v = v * 16 + (*cur++ - 'A' + 10);
		}
		else
			break;
	}

	return v;
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

token_t get_hex(std::string::const_iterator &cur)
{
	if (*cur == '0') {
		cur++;
	}

	if (*cur == 'x') {
		// Hexadecimal number
	    cur++;
	    return token_t(new _HEX(get_hex_str(cur)));
    }

	return token_t(new _HEX(0));
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
    	cur++;
    	return token_t(new _HEX(get_hex_str(cur)));

    } else if (*cur >= '0' && *cur <= '7') {
      // Octal number
      return token_t(new _INT(get_octal_str(cur)));
    }
  }

  //d = i = neg * get_decimal_str(cur);
  i = neg * get_decimal_str(cur);
  d = get_decimal_str(cur);

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

  d = d * neg;

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
    } else if (*cur == '0' && *(cur+1) == 'x') {
    	// Hexadecimal number
    	tq.push(get_hex(cur));
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

