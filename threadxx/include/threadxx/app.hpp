#pragma once

#include <threadxx/dbgstream.hpp>
#include <threadxx/thread.hpp>

extern int main(void);

namespace TXX
{
  class AppBase
  {
  protected:
    static AppBase *running_app;

    AppBase();
    
    void start();
    void run(void *);

    friend int ::main(void);

    friend void ::tx_application_define(void *);
    
    virtual void setup_clocks() = 0;
    virtual void initialize_hardware() = 0;    
    virtual void pre_kernel() = 0;
    virtual void tx_init() = 0;
    virtual void app_create_main() = 0;
    virtual void app_main() = 0;
  };

  
  
  
  template <class cpu_type, size_t stack_size=4096>
  class App : public AppBase
  {
    class AppThread : public Thread<stack_size>
    {
      App *app;

      void main() override { app->app_main(); };
      
    public:
      AppThread(App *_app) : Thread<stack_size>("Application Thread"), app(_app) {}
    };
    
    AppThread app_thread;
    
  public:
    cpu_type cpu;
    
    App() : app_thread(this) { };

    void app_create_main() override { app_thread.create(); }
  };

  void register_setup_func(void (*)(void *), void *);
};
