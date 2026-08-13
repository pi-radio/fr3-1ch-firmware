#include <consolexx/cooked.hpp>
#include <stdio.h>
#include <stdarg.h>

#include <format>

#include <threadxx/config_data.hpp>

#include <piradio/lmx2820.hpp>
#include <piradio/lmx2820.hpp>
#include <ltc2668.h>

#include <piradio/parser.hpp>
#include <piradio/app.hpp>
#include <piradio/config.hpp>


using namespace parser;


// LL(1) for now -- go to LALR(1) only if needed

void Parser::parse_statement_end() {
  if (!tokenizer.get_token()->iseol()) {
    throw SyntaxError();
  }
}

int Parser::shift_int()
{
  auto cur_tok = tokenizer.get_token();

  if (!cur_tok->isint()) {
    throw SyntaxError();
  }

  return cur_tok->i;
}

double Parser::shift_float()
{
  auto cur_tok = tokenizer.get_token();

  if (!cur_tok->isfloat()) {
    throw SyntaxError();
  }

  return cur_tok->d;
}

std::string Parser::shift_string()
{
	auto cur_tok = tokenizer.get_token();

	if (!cur_tok->isstring() ) {
		throw SyntaxError();
	}

	return cur_tok->s;
}

void Parser::parse_config_statement()
{
  auto cur_tok = tokenizer.get_token();

  if (cur_tok == keywords::ERASE) {
    parse_statement_end();
    TXX::config_data::config.erase();
    return;
  }

  if (cur_tok == keywords::READ) {
    int reg = shift_int();

    parse_statement_end();

    uint16_t hw;

    hw = TXX::config_data::config.read_word(reg);

    std::cout << std::format("config[{}]: {:04x}", reg, hw) << std::endl;

    return;
  }

  throw SyntaxError();
}

void Parser::parse_lmx_powerdown() {
	main_app.get_lmx()->write_reg(0, 0x4071);
}

void Parser::parse_lmx_powerup() {
  main_app.get_lmx()->write_reg(0, 0x4070);
}

void Parser::parse_lmx_prog() {
  parse_statement_end();

  main_app.get_lmx()->reprogram();
}


void Parser::parse_lmx_read() {
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

  result = main_app.get_lmx()->read_reg(reg, &val);

  if (result == 0) {
    std::cout << std::format("LMX reg {}: {:04x}", reg, val) << std::endl;
  } else {
    std::cout << std::format("LMX reg {}: FAILURE", reg, val) << std::endl;
  }
}

void Parser::parse_lmx_drive() {
	int val = shift_int();
	uint16_t blob;

	if (val == 7)
		blob = 0x011E;
	else if (val == 6)
		blob = 0x011C;
	else if (val == 5)
		blob = 0x011A;
	else if (val == 4)
		blob = 0x0118;
	else if (val == 3)
		blob = 0x0116;
	else if (val == 2)
		blob = 0x0114;
	else if (val == 1)
		blob = 0x0112;
	else if (val == 0)
		blob = 0x0110;
	else {
		// Error
		throw GeneralError::fmt("Invalid LMX drive {}", val);
	}

	main_app.get_lmx()->write_reg(79, blob);
}

void Parser::parse_lmx_write() {
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

void Parser::parse_lmx_tune() {
  double f = shift_float() * 1e9;

  if (f < 6e9 || f > 24e9) {
    throw GeneralError::fmt("Invalid frequency {}", f);
  }

  main_app.get_lmx()->tune(f);
}

void Parser::parse_lmx_statement() {
  auto cur_tok = tokenizer.get_token();

  if (cur_tok == keywords::PROG) {
    parse_lmx_prog();
  } else if (cur_tok == keywords::WRITE) {
    parse_lmx_write();
  } else if (cur_tok == keywords::READ) {
    parse_lmx_read();
  } else if (cur_tok == keywords::POWERUP) {
    parse_lmx_powerup();
  } else if (cur_tok == keywords::POWERDOWN) {
    parse_lmx_powerdown();
  } else if (cur_tok == keywords::DRIVE) {
    parse_lmx_drive();
  } else if (cur_tok == keywords::TUNE) {
    parse_lmx_tune();
  } else {
    throw SyntaxError();
  }
}

GPIO_PinState pin_value(uint32_t v)
{
	if (v == 0)
		return GPIO_PIN_RESET;
	else
		return GPIO_PIN_SET;
}

using namespace piradio::config;
using namespace TXX::config_data;

void Parser::parse_get_statement() {
  auto cur_tok = tokenizer.get_token();

  if (cur_tok == keywords::BOARD) {
      auto cur_tok = tokenizer.get_token();

      if (cur_tok == keywords::MODEL) {
        auto cur_tok = tokenizer.get_token();

        parse_statement_end();

        auto bm = config.get<board_model>();

        if (bm != nullptr) {
          std::string model;

          model.append((char *)bm->model, bm->length);

          std::cout << "Board model '" << model << "' revision " << bm->revision << std::endl;
          return;
        } else {
          std::cout << "Board model not set" << std::endl;
          return;
        }
      } else if(cur_tok == keywords::SERIAL) {
        parse_statement_end();

        auto ser = config.get<board_serial>();

        if (ser == nullptr) {
          std::cout << "Board serial not set" << std::endl;
        }

        std::string serial;

        serial.append((const char *)ser->serno, ser->length);

        std::cout << "Board model '" << serial << "'" << std::endl;
        return;
      }
  }

  throw SyntaxError();
}

void Parser::parse_set_statement() {
  auto cur_tok = tokenizer.get_token();

  if (cur_tok == keywords::LO) {
    auto cur_tok = tokenizer.get_token();

    if (cur_tok == keywords::EXT) {

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
  } else if (cur_tok == keywords::TXFILTER) {
	  uint32_t v = shift_int();

	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, pin_value(v & 0x20));
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, pin_value(v & 0x10));
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9,  pin_value(v & 0x08));
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, pin_value(v & 0x04));
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, pin_value(v & 0x02));
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, pin_value(v & 0x01));
          
	  return;
  } else if (cur_tok == keywords::BOARD) {
    auto cur_tok = tokenizer.get_token();

    if (cur_tok == keywords::MODEL) {
      auto cur_tok = tokenizer.get_token();

      if (cur_tok->t != token::token_type::STR) {
        throw SyntaxError();
      }

      std::string board_model = cur_tok->s;

      cur_tok = tokenizer.get_token();

      if (cur_tok->t != token::token_type::INT) {
        throw SyntaxError();
      }

      parse_statement_end();

      int rev = cur_tok->i;

      bool found = false;

      for (auto s : piradio::config::board_models) {
        if (s == board_model) {
          found = true;
          break;
        }
      }

      if (!found) {
        throw GeneralError::fmt("Invalid model '{}'", board_model);
      }

      if (board_model.size() > 32) {
        throw GeneralError::fmt("Board model '{}' is too long", board_model);
      }

      std::cout << "Setting board model to " << board_model << " revision " << rev << std::endl;


      piradio::config::board_model mdl(board_model, rev);

      TXX::config_data::config.save(mdl);

      return;
    } else if (cur_tok == keywords::SERIAL) {
      auto cur_tok = tokenizer.get_token();

      if (cur_tok->t != token::token_type::STR) {
        throw SyntaxError();
      }

      std::string board_serial = cur_tok->s;

      parse_statement_end();

      std::cout << "Setting board serial to '" << board_serial << "'" << std::endl;

      piradio::config::board_serial ser(board_serial);

      TXX::config_data::config.save(ser);

      return;
    }
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

void Parser::parse_bootloader_statement() {
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
  dbg::dbgout << "Failed to reset device" << std::endl;

  while(1);
}

void Parser::set_command(std::shared_ptr<Command> &cmd)
{
  current_command = cmd;

  tokenizer.set_line(cmd->str);
}

void parse_lmx(int a, int b, int c)
{

}

Parser::Parser()
{
  //rule<typeof(parse_lmx), ID<"LMX">, ID<"PROGRAM"> > r(parse_lmx);
}

void Parser::parse()
{
  auto cur_tok = tokenizer.get_token();

  if (cur_tok->iseol()) {
    return;
  }

  if (cur_tok == keywords::CONFIG) {
    parse_config_statement();
  } else if (cur_tok == keywords::LMX) {
    parse_lmx_statement();
  } else if (cur_tok == keywords::REDRAW) {
    parse_statement_end();

    main_app.redraw();
  } else if (cur_tok == keywords::CLEAR) {
    parse_statement_end();

    main_app.clear_output();
  } else if (cur_tok == keywords::BOOTLOADER){
    parse_bootloader_statement();
  } else if (cur_tok == keywords::GET){
    parse_get_statement();
  } else if (cur_tok == keywords::SET){
    parse_set_statement();
  } else {
    throw SyntaxError();
  }
}

