/*
 * parser.c
 *
 *  Created on: Jan 23, 2026
 *      Author: zapman
 */
//#include <app_azure_rtos.h>

#include <stdio.h>
#include <stdarg.h>

#include <format>

#include <parser.h>
#include <lexer.h>
#include <terminal.hpp>
#include <lmx.h>
#include <ltc2668.h>

extern "C" {
#include <config_data.h>
}

using namespace parser;

char parser_error[128];

const char *parser_get_error_string(void)
{
  return parser_error;
}

void parser_set_error(const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);

  vsnprintf(parser_error, 127, fmt, args);
  parser_error[128] = 0;

  va_end(args);
}


// LL(1) for now -- go to LALR(1) only if needed

static inline void parse_statement_end() {
  if (!tokenizer.get_token()->iseol()) {
    throw SyntaxError();
  }
}

int shift_int()
{
  auto cur_tok = tokenizer.get_token();

  if (!cur_tok->isint()) {
    throw SyntaxError();
  }

  return cur_tok->i;
}

double shift_float()
{
  auto cur_tok = tokenizer.get_token();

  if (!cur_tok->isfloat()) {
    throw SyntaxError();
  }

  return cur_tok->d;
}


static void parse_config_statement()
{
  auto cur_tok = tokenizer.get_token();

  if (cur_tok == keywords::ERASE) {
    parse_statement_end();
    erase_config_data();
    return;
  }

  if (cur_tok == keywords::READ) {
    int reg = shift_int();

    parse_statement_end();

    uint16_t hw;

    hw = config_read_word(reg);

    printf("Config[%d]: %4.4x\n", reg, hw);

    return;
  }

  throw SyntaxError();
}

static void parse_lmx_prog() {
  parse_statement_end();

  lmx.reprogram();
}


static void parse_lmx_read() {
  int reg, result;
  uint16_t val;
  auto cur_tok = tokenizer.get_token();

  if (cur_tok != keywords::REG) {
    throw SyntaxError();
  }

  reg = shift_int();

  if (reg < 0 || reg > 122) {
    // TODO BETTER ERROR REPORTING
    throw GeneralError::fmt("Invalid register {}", reg);
  }

  parse_statement_end();

  result = lmx.read_reg(reg, &val);

  if (result == 0) {
    printf("LMX reg %d: %04x\n", reg, val);
  } else {
    printf("Failed to read reg: %d\n", result);
  }
}


static void parse_lmx_write() {
  int reg, val;
  auto cur_tok = tokenizer.get_token();

  if (cur_tok != keywords::REG) {
    throw SyntaxError();
  }

  cur_tok = tokenizer.get_token();

  reg = shift_int();

  if (reg < 0 || reg > 122) {
    // TODO BETTER ERROR REPORTING
    throw GeneralError::fmt("Invalid register {}", reg);
  }

  val = shift_int();

  parse_statement_end();
}

static void parse_lmx_statement() {
  auto cur_tok = tokenizer.get_token();

  if (cur_tok == keywords::PROG) {
    parse_lmx_prog();
  } else if (cur_tok == keywords::WRITE) {
    parse_lmx_write();
  } else if (cur_tok == keywords::READ) {
    parse_lmx_read();
  } else {
    throw SyntaxError();
  }
}

static void parse_set_statement() {

  auto cur_tok = tokenizer.get_token();

  if (cur_tok == keywords::LO) {
    auto cur_tok = tokenizer.get_token();

    if (cur_tok == keywords::EXT) {
      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
    } else if (cur_tok == keywords::INT) {
      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
    } else {
      throw SyntaxError();
    }

    parse_statement_end();

    return;
  } else if (cur_tok == keywords::CLOCK) {
    auto cur_tok = tokenizer.get_token();

    if (cur_tok == keywords::EXT) {
      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET);
    } else if (cur_tok == keywords::INT) {
      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
    } else {
      throw SyntaxError();
    }

    parse_statement_end();

    return;
  } else if (cur_tok == keywords::I_V) {
    // Parse a float
    double V = shift_float();

    parse_statement_end();
    ltc2668.setV(2, V);
    return;
  } else if (cur_tok == keywords::Q_V) {
    // Parse a float
    double V = shift_float();

    parse_statement_end();

    ltc2668.setV(0, V);
    return;
  } else if (cur_tok == keywords::IQ) {
    auto cur_tok = tokenizer.get_token();

    if (cur_tok == keywords::SWAP) {
      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
    } else if (cur_tok == keywords::THRU) {
      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
    } else {
      throw SyntaxError();
    }

    parse_statement_end();

    return;
  } else {

	  // Invalid SET token
	throw SyntaxError();
	parse_statement_end();
	return;
  }

}

extern struct bootloader_vectable {
  uint32_t msp;
  void (*reset_handler)(void);
} bootloader;

static void parse_bootloader_statement() {
  int i;
  auto cur_tok = tokenizer.get_token();

  parse_statement_end();

  /* Disable all interrupts */
  __disable_irq();

  /* Disable Systick timer */
  SysTick->CTRL = 0;

  /* Set the clock to the default state */
  HAL_RCC_DeInit();

  /* Clear Interrupt Enable Register & Interrupt Pending Register */
  for (i=0;i<5;i++)
  {
    NVIC->ICER[i]=0xFFFFFFFF;
    NVIC->ICPR[i]=0xFFFFFFFF;
  }

  /* Re-enable all interrupts */
  __enable_irq();

  /* Set the main stack pointer to the boot loader stack */
  __set_MSP(bootloader.msp);

  /* Call the function to jump to boot loader location */
  bootloader.reset_handler();


  /* Jump is done successfully */
  Error_Handler();
}

int parser_parse_statement()
{
  int retval;

  parser_set_error("NOT SET");

  try {
    auto cur_tok = tokenizer.get_token();

    if (cur_tok->iseol()) {
      return PARSER_OK;
    }

    if (cur_tok == keywords::CONFIG) {
        parse_config_statement();
    } else if (cur_tok == keywords::LMX) {
        parse_lmx_statement();
    } else if (cur_tok == keywords::REDRAW) {
        parse_statement_end();

        terminal_send_command(TERMINAL_CMD_REDRAW);
    } else if (cur_tok == keywords::CLEAR) {
        parse_statement_end();

        terminal_send_command(TERMINAL_CMD_CLEAR_OUTPUT);
    } else if (cur_tok == keywords::BOOTLOADER){
        parse_bootloader_statement();
    } else if (cur_tok == keywords::SET){
      parse_set_statement();
    } else {
      throw SyntaxError();
    }

    return PARSER_OK;
  } catch (const SyntaxError &e) {
    parser_set_error("Syntax Error");
    return PARSER_SYNTAX_ERROR;
  } catch (const GeneralError &e) {
    parser_set_error(e.s.c_str());
    return PARSER_GENERAL_ERROR;
  } catch (...) {
    parser_set_error("Unknown Exception!!?!?!?!?\n");
    return PARSER_GENERAL_ERROR;
  }
}

void lexer_set_line(const std::string &s)
{
  tokenizer.set_line(s);
}

