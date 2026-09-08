#include <threadxx/app.hpp>
#include <threadxx/config_data.hpp>
#include <threadxx/object.hpp>

#

extern "C" {
#include "tx_initialize.h"
}

using namespace TXX; 

static std::vector<std::pair<void (*)(void *), void *> > setup_calls;

AppBase *AppBase::running_app = NULL;

extern uint32_t _tx_thread_system_state;
extern "C" void _tx_thread_schedule(void);

AppBase::AppBase()
{
  running_app = this;
}

void AppBase::start()
{
  setup_clocks();

  setup_memory();

  setup_debug();

  setup_tick();

  config_data::config.load();

  initialize_hardware();

  object::on_enter_kernel();

  pre_kernel();

  for (auto cbp : setup_calls) {
    cbp.first(cbp.second);
  }

  app_create_main();
  
  /* Optional processing extension.  */
  TX_INITIALIZE_KERNEL_ENTER_EXTENSION

  /* Ensure that the system state variable is set to indicate
     initialization is in progress.  Note that this variable is
     later used to represent interrupt nesting.  */
  _tx_thread_system_state =  TX_INITIALIZE_IN_PROGRESS;

  /* Optional random number generator initialization.  */
  TX_INITIALIZE_RANDOM_GENERATOR_INITIALIZATION

  /* Call the application provided initialization function.  Pass the
     first available memory address to it.  */
  tx_application_define(_tx_initialize_unused_memory);

  /* Set the system state in preparation for entering the thread
     scheduler.  */
  _tx_thread_system_state =  TX_INITIALIZE_IS_FINISHED;

  /* Call any port specific pre-scheduler processing.  */
  TX_PORT_SPECIFIC_PRE_SCHEDULER_INITIALIZATION

#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
  /* Initialize Execution Profile Kit.  */
  _tx_execution_initialize();
#endif

  /* Enter the scheduling loop to start executing threads!  */
  _tx_thread_schedule();

#ifdef TX_SAFETY_CRITICAL

  /* If we ever get here, raise safety critical exception.  */
  TX_SAFETY_CRITICAL_EXCEPTION(__FILE__, __LINE__, 0);
#endif


  //tx_kernel_enter();
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
