#include <threadxx/static.hpp>
#include <threadxx/dbgstream.hpp>
#include <threadxx/app.hpp>

static int _counter = 0;

using namespace TXX;

__txx_initializer::__txx_initializer()
{
  if(_counter++ > 0)
    return;

  dbg::initialize_dbgstream();
}

__txx_initializer::~__txx_initializer()
{
  if(--_counter > 0)
    return;

  while(1);
}
