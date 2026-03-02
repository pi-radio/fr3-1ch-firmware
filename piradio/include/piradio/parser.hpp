#pragma once

#include <string>
#include <format>
#include <vector>
#include <queue>
#include <memory>
#include <algorithm>
#include <map>

#define LIST_OF_KEYWORDS			\
  MAKE_KEYWORD(ERASE);				\
  MAKE_KEYWORD(CONFIG);				\
  MAKE_KEYWORD(READ);				\
  MAKE_KEYWORD(REDRAW);				\
  MAKE_KEYWORD(LMX);				\
  MAKE_KEYWORD(PROG);				\
  MAKE_KEYWORD(WRITE);				\
  MAKE_KEYWORD(REG);				\
  MAKE_KEYWORD(BOOTLOADER);			\
  MAKE_KEYWORD(CLEAR);				\
  MAKE_KEYWORD(SET);				\
  MAKE_KEYWORD(LO);				\
  MAKE_KEYWORD(EXT);				\
  MAKE_KEYWORD(INT);				\
  MAKE_KEYWORD(I_V);				\
  MAKE_KEYWORD(Q_V);				\
  MAKE_KEYWORD(IQ);				\
  MAKE_KEYWORD(SWAP);				\
  MAKE_KEYWORD(THRU);				\
  MAKE_KEYWORD(ON);				\
  MAKE_KEYWORD(OFF);				\
  MAKE_KEYWORD(CLOCK);				\
  MAKE_KEYWORD(TUNE);				\


namespace parser
{
  struct token {
    enum class token_type {
      STR,
      INT,
      FLOAT,
      EOL,
      ERROR,
      ID,
      KEYWORD
    };

    token_type t;
    const std::string s;
    const int i;
    const double d;

    token(token_type _t)  : t(_t), s(""), i(0), d(0.0) {}
    token(token_type _t, const std::string &_s)  : t(_t), s(_s), i(0), d(0.0) {}
    token(token_type _t, const int &_i)  : t(_t), s(""), i(_i), d(0.0) {}
    token(token_type _t, const double &_d)  : t(_t), s(""), i(0), d(_d) {}

    bool isint() const { return t == token_type::INT; }
    bool isfloat() const { return t == token_type::FLOAT; }
    bool iseol() const { return t == token_type::EOL; }

    bool operator == (const token &o) const {
      if (t != o.t) return false;
      if (s != o.s) return false;
      if (i != o.i) return false;
      if (d != o.d) return false;
      return true;
    }
  };

  typedef std::shared_ptr<token> token_t;


  struct _ID : public token {
    _ID(std::string _s) : token(token_type::ID, _s) {}
  };

  struct _STR : public token {
    _STR(std::string _s) : token(token_type::STR, _s) {}
  };


  struct _INT : public token {
    _INT(int _i) : token(token_type::INT, _i) {}
  };

  struct _FLOAT : public token {
    _FLOAT(double _d) : token(token_type::FLOAT, _d) {}
  };

  struct _EOL : public token {
    _EOL() : token(token_type::EOL) {}
  };

  struct _ERROR : public token {
    _ERROR() : token(token_type::ERROR) {}
  };

  struct keyword : public token {
    keyword(const std::string &_kw);
  };
  

#define MAKE_KEYWORD(x) extern const token_t x; 
  namespace keywords {
    LIST_OF_KEYWORDS
  };
#undef MAKE_KEYWORD

  class _tokenizer {
    std::queue<token_t> tq;
    void push_token(token_t);
  public:
    token_t peek_token();
    token_t get_token();
    void set_line(const std::string &s);
  };

  
  
  struct ParserRule
  {
  };

  class SyntaxError
  {

  };

  class GeneralError
  {
  public:
    std::string s;

    template <typename... Args>
    static GeneralError fmt(const std::format_string<Args...> fmt, Args&&...args) {
      return GeneralError(std::vformat(fmt.get(), std::make_format_args(args...)));
    }

    GeneralError(const std::string &_s) : s(_s) {}
  };


  class rule {
    class INT;
    class FLOAT;
    class ID;
  };

  struct ParserState
  {
  };
  
  class Parser
  {
    _tokenizer tokenizer;


    void parse_statement_end();
    int shift_int();
    double shift_float();
    void parse_config_statement();
    void parse_lmx_prog();
    void parse_lmx_read();
    void parse_lmx_write();
    void parse_lmx_statement();

    void parse_set_statement();
    void parse_bootloader_statement();

  public:
    void set_line(const std::string &s);
    void parse();
  };
  
};


#define PARSER_OK            0
#define PARSER_SYNTAX_ERROR  1
#define PARSER_GENERAL_ERROR 2

extern const char *parser_get_error_string(void);

extern "C" int parser_parse_statement(void);

extern void lexer_set_line(const std::string &s);


