#pragma once

#include <string>
#include <format>
#include <vector>
#include <queue>
#include <memory>
#include <algorithm>
#include <map>
#include <concepts>
#include <functional>

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
  MAKE_KEYWORD(POWERUP);                        \
  MAKE_KEYWORD(POWERDOWN);                      \
  MAKE_KEYWORD(DRIVE);                          \
  MAKE_KEYWORD(RXFILTER);                       \
  MAKE_KEYWORD(TXFILTER);                       \
  MAKE_KEYWORD(BOARD);                          \
  MAKE_KEYWORD(MODEL);                          \
  MAKE_KEYWORD(SERIAL);                         \
  MAKE_KEYWORD(GET);        \
  MAKE_KEYWORD(RAW);        \
  MAKE_KEYWORD(COOKED);     \
  MAKE_KEYWORD(FAULT);      \
  MAKE_KEYWORD(CRASH);      \
  MAKE_KEYWORD(PROCINFO);   \


namespace parser
{
  struct token_base {

  };

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
    bool isstring() const { return t == token_type::STR; }


    bool operator == (const token &o) const {
      if (t != o.t) return false;
      if (s != o.s) return false;
      if (i != o.i) return false;
      if (d != o.d) return false;
      return true;
    }
  };



  typedef std::shared_ptr<token> token_t;


  struct ID : public token {
    ID(std::string _s) : token(token_type::ID, _s) {}
  };

  struct STR : public token {
    STR(std::string _s) : token(token_type::STR, _s) {}
  };


  struct _INT : public token {
    _INT(int _i) : token(token_type::INT, _i) {}
  };

  struct FLOAT : public token {
    FLOAT(double _d) : token(token_type::FLOAT, _d) {}
  };

  struct _EOL : public token {
    _EOL() : token(token_type::EOL) {}
  };

  struct ERROR : public token {
    ERROR() : token(token_type::ERROR) {}
  };

  struct keyword : public token {
    keyword(const std::string &_kw);
  };
  




  template <typename t, typename... rest>
  struct rule_fragment {
    rule_fragment<rest...> remainder;
  };

  template <typename t>
  struct rule_fragment<t> {
  };

  template <typename t, typename... rest>
  struct rule {
    rule_fragment<t, rest...> body;
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


  struct ParserHandler
  {

  };

  struct Nonterminal : public ParserHandler
  {
    std::map<token::token_type, ParserHandler *> children;

    void add_rule();
  };

  struct Action : public ParserHandler
  {

  };

  struct Command
  {
    enum CommandOrigin {
      CONSOLE,
      APPLICATION
    } origin;
    std::string str;

    Command(const std::string &_str, CommandOrigin _origin) :
      origin(_origin), str(_str)  {

    }
  };

  class Parser
  {
    _tokenizer tokenizer;


    void parse_statement_end();
    int shift_int();
    double shift_float();
    std::string shift_string();


    void parse_config_statement();
    void parse_lmx_prog();
    void parse_lmx_read();
    void parse_lmx_write();
    void parse_lmx_tune();
    void parse_lmx_statement();
    void parse_lmx_powerup();
    void parse_lmx_powerdown();
    void parse_lmx_drive();
    void parse_set_statement();
    void parse_get_statement();
    void parse_bootloader_statement();

    std::shared_ptr<Command> current_command;

  public:
    Parser();

    void set_command(std::shared_ptr<Command> &cmd);
    void parse();
  };
  
};


#define PARSER_OK            0
#define PARSER_SYNTAX_ERROR  1
#define PARSER_GENERAL_ERROR 2

extern const char *parser_get_error_string(void);

extern "C" int parser_parse_statement(void);

extern void lexer_set_line(const std::string &s);


