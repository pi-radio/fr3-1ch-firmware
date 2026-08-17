#include <halxx/fault.hpp>
#include <consolexx/cooked.hpp>
#include <stdio.h>
#include <stdarg.h>

#include <algorithm>
#include <format>

#include <threadxx/config_data.hpp>

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

  if (cur_tok->isint()) {
    return cur_tok->i;
  }
  
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
  main_app.get_hardware()->set_lmx_powerdown(true);
}

void Parser::parse_lmx_powerup() {
  main_app.get_hardware()->set_lmx_powerdown(false);
}

void Parser::parse_lmx_prog() {
  parse_statement_end();

  main_app.get_hardware()->reprogram_lmx();
}


void Parser::parse_lmx_read() {
  int reg, result;
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

  auto val = main_app.get_hardware()->lmx_read_reg(reg);

  std::cout << std::format("LMX reg {}: {:04x}", reg, val) << std::endl;
}

void Parser::parse_lmx_drive() {
	int val = shift_int();
	uint16_t blob;

	if (val < 0 || val > 7) {
    throw GeneralError::fmt("Invalid LMX drive {}", val);
	}

	main_app.get_hardware()->set_lmx_drive(val);
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

  main_app.get_hardware()->tune_lmx(f);
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
          return;
        }

        std::string serial;

        serial.append((const char *)ser->serno, std::min((unsigned int)ser->length, sizeof(ser->serno)));

        std::cout << "Board serial '" << serial << "'" << std::endl;
        return;
      }
  } else if (cur_tok == keywords::I_V) {
    parse_statement_end();
    std::cout << main_app.get_hardware()->get_I_voltage() << std::endl;
    return;
  } else if (cur_tok == keywords::Q_V) {
    parse_statement_end();
    std::cout << main_app.get_hardware()->get_Q_voltage() << std::endl;
    return;
  } else if (cur_tok == keywords::FAULT) {
    parse_statement_end();
    halxx::fault::analyzer analyzer(std::cout);
    return;
  } else if (cur_tok == keywords::PROCINFO) {
    parse_statement_end();
    uint32_t cpuid = LL_DBGMCU_GetDeviceID();
    uint32_t revid = LL_DBGMCU_GetRevisionID();

    std::cout << std::format("CPUID: {:04x} Rev: {:04x}", cpuid, revid) << std::endl;

    //std::cout << std::format("CIDR0: {:08x}", DBGMCU->CIDR0) << std::endl;
    //std::cout << std::format("CIDR1: {:08x}", DBGMCU->CIDR1) << std::endl;
    //std::cout << std::format("CIDR2: {:08x}", DBGMCU->CIDR2) << std::endl;
    //std::cout << std::format("CIDR3: {:08x}", DBGMCU->CIDR3) << std::endl;
    return;
  } else {
    throw SyntaxError();
  }
}

void Parser::parse_set_statement() {
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

    if (V < -0.4 || V > 0.4) {
      throw GeneralError::fmt("Invalid IQ voltage {}", V);
    }

    main_app.get_hardware()->set_I_voltage(V);

    return;
  } else if (cur_tok == keywords::Q_V) {
    // Parse a float
    double V = shift_float();

    parse_statement_end();

    if (V < -0.4 || V > 0.4) {
      throw GeneralError::fmt("Invalid IQ voltage {}", V);
    }

    main_app.get_hardware()->set_Q_voltage(V);

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
  } else if (cur_tok == keywords::RXFILTER) {
    uint32_t v = shift_int();

    parse_statement_end();

    main_app.get_hardware()->set_rx_filter(v);
    
    return;
  } else if (cur_tok == keywords::TXFILTER) {
    uint32_t v = shift_int();

    parse_statement_end();

    main_app.get_hardware()->set_tx_filter(v);

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
  } else if (cur_tok == keywords::RAW){
    main_app.set_console_mode(consolexx::terminal_adapter::RAW);
  } else if (cur_tok == keywords::COOKED){
    main_app.set_console_mode(consolexx::terminal_adapter::COOKED);
  } else if (cur_tok == keywords::CRASH) {
    *(int *)0 = 0;
  } else {
    throw SyntaxError();
  }
}

