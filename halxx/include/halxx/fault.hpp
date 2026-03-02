#pragma once

#include <halxx/extern.hpp>

#if defined(__cplusplus)

#include <cstdint>
#include <ostream>

namespace halxx
{
  namespace fault
  {
    class analyzer
    {
    public:
      analyzer(std::ostream &);
    };
  }
}

#endif


EXTERN_C uint32_t __exc_stor[2];
EXTERN_C void save_exception(uint32_t exception_type);
EXTERN_C void __error_handler(const char *file, int line);

#define EXCEPTION_HARD_FAULT    0xAD01FFCB
#define EXCEPTION_DEBUG         0xDEADBEEF
#define EXCEPTION_NMI           0xFEEDF00D
#define EXCEPTION_MEM_MANAGE    0xCAFECAFE
#define EXCEPTION_BUS_FAULT     0x99998888
#define EXCEPTION_USAGE_FAULT   0x45888277
#define EXCEPTION_ERROR_HANDLER 0xF00DA551
