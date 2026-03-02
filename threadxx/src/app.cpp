#include <threadxx/app.hpp>

extern "C" {
#include "tx_initialize.h"
}

using namespace TXX; 

static std::vector<std::pair<void (*)(void *), void *> > setup_calls;

AppBase *AppBase::running_app = NULL;

AppBase::AppBase()
{
  running_app = this;
}

void AppBase::start()
{
  setup_clocks();
  initialize_hardware();

  pre_kernel();

  _tx_initialize_kernel_setup();

  for (auto cbp : setup_calls) {
    cbp.first(cbp.second);
  }

  app_create_main();
  
  tx_kernel_enter();
}

void AppBase::run(void *p)
{
  // We're now in the ThreadX kernel
  try {
    tx_init();
  } catch(...) {
    dbg::dbgout << "Unhandled exception in tx_init" << std::endl;

    while(1);
  }
}

int main(void)
{
  if (AppBase::running_app != NULL)
    AppBase::running_app->start();
  
  return 0;
}

extern "C" void tx_application_define(void *p)
{
  AppBase::running_app->run(p);
}
