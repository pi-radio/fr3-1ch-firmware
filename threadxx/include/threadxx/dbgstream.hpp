#pragma once

#if defined(__cplusplus)
#include <iostream>
#include <vector>
#include <stdint.h>

namespace dbg
{
  struct renderer
  {
    virtual int render(const char *buffer, size_t size) = 0;
  };

  extern void add_renderer(renderer *r);
  
  extern std::ostream &dbgout;

  static struct _dbginitializer {
    _dbginitializer();
    ~_dbginitializer();
  } dbginitializer;
};

extern "C" int dbgprint(const char *fmt, ...);

#else

extern int dbgprint(const char *fmt, ...);

#endif
