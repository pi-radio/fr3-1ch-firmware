#include <halxx/fault.hpp>

#include <cortex/core.hpp>

#include <memory>
#include <cstring>

using namespace halxx::fault;

struct state_context_base {
  uint32_t R0;
  uint32_t R1;
  uint32_t R2;
  uint32_t R3;
  uint32_t R12;
  uint32_t LR;
  uint32_t PC;
  uint32_t xPSR;
} __attribute__ ((packed));

struct state_context_fp {
  uint32_t reserved;
  uint32_t FPSCR;
  uint32_t S15;
  uint32_t S14;
  uint32_t S13;
  uint32_t S12;
  uint32_t S11;
  uint32_t S10;
  uint32_t S9;
  uint32_t S8;
  uint32_t S7;
  uint32_t S6;
  uint32_t S5;
  uint32_t S4;
  uint32_t S3;
  uint32_t S2;
  uint32_t S1;
  uint32_t S0;
} __attribute__ ((packed));

#define EXCEPTION_HARD_FAULT    0xAD01FFCB
#define EXCEPTION_DEBUG         0xDEADBEEF
#define EXCEPTION_NMI           0xFEEDF00D
#define EXCEPTION_MEM_MANAGE    0xCAFECAFE
#define EXCEPTION_BUS_FAULT     0x99998888
#define EXCEPTION_USAGE_FAULT   0x45888277
#define EXCEPTION_ERROR_HANDLER 0xF00DA551

typedef struct exception_info {
  uint32_t exception_type;
  uint32_t CFSR;
  uint32_t HFSR;
  uint32_t SHCSR;
  uint32_t VTOR;
  uint32_t excSP;
  uint32_t excLR;
  uint32_t SP;
  uint32_t SP1;
  uint32_t CONTROL;
  uint32_t excbase;
  struct state_context_base base_ctx;
  struct state_context_fp fp_ctx;
} exception_info_t;


exception_info_t __attribute__(( section(".noinitdata") )) _einfo;

template <int w>
struct hex
{
  uint32_t n;

  hex(uint32_t _n) : n(_n) {}
};

template <int w>
std::ostream &operator <<(std::ostream &str, const struct hex<w> &n)
{
  std::ios save(NULL);
  save.copyfmt(str);

  auto sw = str.width(w);
  auto sf = str.fill('0');

  str << std::hex << "0x" << n.n;

  str.width(sw);
  str.fill(sf);
  
  str.copyfmt(save);

  return str;
}

analyzer::analyzer(std::ostream &out)
{
  bool known_type = true;
  
  uint32_t etype = _einfo.exception_type;
  _einfo.exception_type = 0;
  
  if (etype == EXCEPTION_ERROR_HANDLER) {
    out << "Error_Handler called: " << (const char *)_einfo.excSP << ":" << _einfo.excLR << std::endl;
    return;
  }

  switch(etype) {
    case EXCEPTION_HARD_FAULT:
      out << "Hard Fault:" 
	  << ((_einfo.HFSR & (1 << 31)) ? " DEBUGEVT" : "")
	  << ((_einfo.HFSR & (1 << 30)) ? " FORCED" : "")
	  << ((_einfo.HFSR & (1 << 1)) ? " VECTTBL" : "")
	  << std::endl;
      break;
    case EXCEPTION_DEBUG:
      out << "Debug Fault:" << std::endl;
      break;
    case EXCEPTION_NMI:
      out << "NMI:" << std::endl;
      break;
    case EXCEPTION_MEM_MANAGE:
      out << "Memory Management Fault:" << std::endl;
      break;
    case EXCEPTION_BUS_FAULT:
      out << "Bus Fault:" << std::endl;
      break;
    case EXCEPTION_USAGE_FAULT:
      out << "Usage Fault:" << std::endl;
      break;
    default:
      known_type = false;
      break;
  }

  if (!known_type) return;

  std::ios save(NULL);
  save.copyfmt(out);

  out << "CONTROL: " << hex<8>(_einfo.CONTROL)
      << ((_einfo.CONTROL & (1 << 3)) ? " SFPA" : "")
      << ((_einfo.CONTROL & (1 << 2)) ? " FPCA" : "")
      << ((_einfo.CONTROL & (1 << 1)) ? " PSP" : " MSP")
      << ((_einfo.CONTROL & (1 << 0)) ? " UNPRIV" : " PRIV")
      << std::endl;

  out << "CFSR: " << hex<8>(_einfo.CFSR)
      << " | UFSR: " << hex<4>((_einfo.CFSR >> 16) & 0xFFFF)
      << " BFSR: " << hex<2>((_einfo.CFSR >> 8) & 0xFF)
      << " MMFSR: " << hex<2>(_einfo.CFSR & 0xFF)
      << std::endl;

  out << "Exception SP: " << hex<8>(_einfo.excSP)
      << " LR: " << hex<8>(_einfo.excLR)
      << std::endl;

  out << "S(" << ((_einfo.excLR >> 6) & 1) << ") "
      << "DCRS(" << ((_einfo.excLR >> 5) & 1)  << ") "
      << "FType(" << ((_einfo.excLR >> 4) & 1)  << ") "
      << "Mode(" << ((_einfo.excLR >> 3) & 1) << ") "
      << "SPSEL(" << ((_einfo.excLR >> 2) & 1) << ") "
      << "ES(" << ((_einfo.excLR >> 0) & 1) << std::endl;

  out << "SHCSR: " << hex<8>(_einfo.SHCSR)
      << " VTOR: " << hex<8>(_einfo.VTOR)
      << std::endl;

  out << "PC: " << hex<8>(_einfo.base_ctx.PC)
      << " LR: " << hex<8>(_einfo.base_ctx.LR)
      << " SP: " << hex<8>(_einfo.SP)
      << " xPSR: " << hex<8>(_einfo.base_ctx.xPSR)
      << std::endl;

  out << "R0: " << hex<8>(_einfo.base_ctx.R0)
      << " R1: " << hex<8>(_einfo.base_ctx.R1)
      << " R2: " << hex<8>(_einfo.base_ctx.R2)
      << " R3: " << hex<8>(_einfo.base_ctx.R3)
      << " R12: " << hex<8>(_einfo.base_ctx.R12)
      << std::endl;

  out.copyfmt(save);
}


extern "C" {
  uint32_t __exc_stor[2];
};

extern "C" void save_exception(uint32_t exception_type)
{
  _einfo.excSP = __exc_stor[0];
  _einfo.excLR = __exc_stor[1];

  uint32_t *cp = (uint32_t *)_einfo.excSP;

  _einfo.exception_type = exception_type;
  _einfo.CONTROL = __get_CONTROL();
  _einfo.CFSR = SCB->CFSR;
  _einfo.HFSR = SCB->HFSR;
  _einfo.SHCSR = SCB->SHCSR;
  _einfo.VTOR = SCB->VTOR;

  std::memcpy(&_einfo.base_ctx, cp, sizeof(_einfo.base_ctx));
  cp += sizeof(_einfo.base_ctx) / 4;

  _einfo.SP1 = *cp;

  if (!(_einfo.excLR & (1 << 4))) {
    std::memcpy(&_einfo.fp_ctx, cp, sizeof(_einfo.fp_ctx));
    cp += sizeof(_einfo.fp_ctx) / 4;
  }

  _einfo.SP = *cp++;

  _einfo.excbase = (uint32_t)cp;

  for (int i = 0; i < 5000000; i++);
}

void __error_handler(const char *file, int line)
{
  __disable_irq();

  _einfo.exception_type = EXCEPTION_ERROR_HANDLER;

  _einfo.excSP = (uint32_t)file;
  _einfo.excLR = line;

  NVIC_SystemReset();
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

