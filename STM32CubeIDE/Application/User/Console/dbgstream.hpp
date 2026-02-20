/*
 * dbgstream.hpp
 *
 *  Created on: Feb 18, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_DBGSTREAM_HPP_
#define APPLICATION_USER_CONSOLE_DBGSTREAM_HPP_

#if defined(__cplusplus)

#include <iostream>
#include <stdint.h>

extern std::ostream dbgout;

extern "C" void set_uart_ready();

#endif


#endif /* APPLICATION_USER_CONSOLE_DBGSTREAM_HPP_ */
