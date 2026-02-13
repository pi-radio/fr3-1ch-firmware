#include <string>
#include <queue>
#include <memory>
#include <algorithm>
#include <map>

template<class StringRange>
constexpr StringRange&& mklower(StringRange&& stringview) noexcept {
    std::transform(stringview.begin(), stringview.end(), stringview.begin(),
        [](unsigned char c) -> unsigned char { return std::toupper(c); });
    return std::forward<StringRange>(stringview);
}


namespace parser
{

  struct token {
    enum tok_type {
      STR,
      INT,
      FLOAT,
      EOL,
      ERROR,
      ID,
      KEYWORD
    } t;

    const std::string s;
    const int i;
    const double d;

    token(tok_type _t)  : t(_t), s(""), i(0), d(0.0) {}
    token(tok_type _t, const std::string &_s)  : t(_t), s(_s), i(0), d(0.0) {}
    token(tok_type _t, const int &_i)  : t(_t), s(""), i(_i), d(0.0) {}
    token(tok_type _t, const double &_d)  : t(_t), s(""), i(0), d(_d) {}

    bool isint() { return t == INT; }
    bool isfloat() { return t == FLOAT; }
    bool iseol() { return t == EOL; }

    bool operator == (const token &o) {
      if (t != o.t) return false;
      if (s != o.s) return false;
      if (i != o.i) return false;
      if (d != o.d) return false;
      return true;
    }
  };

  typedef std::shared_ptr<token> token_t;


  struct _ID : public token {
    _ID(std::string _s) : token(ID, _s) {}
  };

  struct _STR : public token {
    _STR(std::string _s) : token(STR, _s) {}
  };


  struct _INT : public token {
    _INT(int _i) : token(INT, _i) {}
  };

  struct _FLOAT : public token {
    _FLOAT(double _d) : token(FLOAT, _d) {}
  };

  struct _EOL : public token {
    _EOL() : token(EOL) {}
  };

  struct _ERROR : public token {
    _ERROR() : token(ERROR) {}
  };

  struct keyword : public token {
    keyword(const std::string &_kw);
  };

#define MAKE_KEYWORD(x)  extern token_t x;

  namespace keywords {
#include "keywords.h"
  };

  class _tokenizer {
    std::queue<token_t> tq;
  public:
    token_t peek_token();
    token_t get_token();
    void set_line(const std::string &s);
  };

  extern _tokenizer tokenizer;
};
