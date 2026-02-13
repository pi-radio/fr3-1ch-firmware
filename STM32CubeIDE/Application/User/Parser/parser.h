/*
 * parser.h
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_PARSER_H_
#define APPLICATION_USER_CONSOLE_PARSER_H_

#if defined(__cplusplus)

#include <string>
#include <format>

namespace parser
{
  class ParserRule
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

};


#define PARSER_OK            0
#define PARSER_SYNTAX_ERROR  1
#define PARSER_GENERAL_ERROR 2

extern const char *parser_get_error_string(void);

extern "C" int parser_parse_statement(void);

extern void lexer_set_line(const std::string &s);

#else // __cplusplus

extern int parser_parse_statement(void);
#endif

#endif /* APPLICATION_USER_CONSOLE_PARSER_H_ */
