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

#include <dbgstream.hpp>

#include "tx_api.h"

namespace TXX {
  class ThreadBase {
    TX_THREAD _thread;
    const std::string &_name;
    int _priority;
    int _preempt;
    bool _autostart;
    int _timeslice;
    uint8_t *_stack;
    size_t _stack_size;

  protected:
    static void launch(ULONG a) {
      ThreadBase *p = (ThreadBase *)a;
      p->entry();
    }

    void entry() {
      try {
        main();
      } catch (...) {
        dbgout << "Uncaught exception in thread " << _name << std::endl;
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
    void create();
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
};



#endif /* TXXTHREAD_HPP_ */
