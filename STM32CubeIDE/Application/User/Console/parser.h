/*
 * parser.h
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_PARSER_H_
#define APPLICATION_USER_CONSOLE_PARSER_H_

#define PARSER_OK            0
#define PARSER_SYNTAX_ERROR  1
#define PARSER_GENERAL_ERROR 2

extern const char *parser_get_error_string(void);

extern int parser_parse_statement(void);

#endif /* APPLICATION_USER_CONSOLE_PARSER_H_ */
