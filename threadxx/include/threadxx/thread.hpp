/*
 * txxthread.hpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */

#ifndef TXXTHREAD_HPP_
#define TXXTHREAD_HPP_

#include <stdint.h>
#include <string>

#include <threadxx/object.hpp>
#include <threadxx/dbgstream.hpp>

#include "tx_api.h"

namespace TXX {
  struct thread {
  };
  

  class ThreadBase : public object {
  protected:
    TX_THREAD _thread;
    int _priority;
    int _preempt;
    bool _autostart;
    int _timeslice;
    uint8_t *_stack;
    size_t _stack_size;

    friend class creator<ThreadBase>;
    static constexpr creator<ThreadBase> c = {};

    void create();

  protected:
    static void launch(ULONG a) {
      try {
        ThreadBase *p = (ThreadBase *)a;
        p->entry();
      } catch(...) {
        __asm volatile ("BKPT     %0" : : "i"(0));
      }
    }

    void entry() {
      try {
        main();
      } catch (const std::exception &e) {
        const char *what = e.what();
        dbg::dbgout << "Uncaught exception in thread " << name << std::endl;
        __asm volatile ("BKPT     %0" : : "i"(0));
      }
    }

    ThreadBase(const std::string &name,
                uint8_t *stack,
                size_t stack_size,
                int priority = 20,
                int preempt = 20,
                int timeslice = TX_NO_TIME_SLICE,
                bool autostart=true
                );

    virtual void main() = 0;

  public:
    virtual void resume() { tx_thread_resume(&_thread); }
  };

  template <int stack_size>
  class Thread : public ThreadBase {
    uint8_t __stack[stack_size] __attribute__((aligned(4)));




  public:
    Thread(const std::string &name,
        int priority = 20,
        int preempt = 20,
        int timeslice = TX_NO_TIME_SLICE,
        bool autostart=true) : ThreadBase(name, __stack, stack_size,
            priority, preempt, timeslice, autostart) { }
  };

  template <class C, size_t stack_size, int priority=20, int preempt=20>
  class MemberThread : public Thread<stack_size> {
    C *_p;
    void (C::*_mf)();

    static void s_entry(ULONG _p) {
      MemberThread *t = (MemberThread *)_p;
      t->m_entry();
    }
    
    void main() override {
      (_p->*_mf)();
    }

  public:
    MemberThread(const std::string &name, C *x, void (C::*mf)()) : Thread<stack_size>(name,
										      priority,
										      preempt)
    {
      _p = x;
      _mf = mf;
    }
  };
};



#endif /* TXXTHREAD_HPP_ */
