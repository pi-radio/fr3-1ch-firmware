/*
 * txxthread.cpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */

#include <threadxx/thread.hpp>
#include <threadxx/dbgstream.hpp>

TXX::ThreadBase::ThreadBase(const std::string &_name,
    uint8_t *stack,
    size_t stack_size,
    int priority,
    int preempt,
    int timeslice,
    bool autostart) :
    object(_name, c),
    _priority(priority),
    _preempt(preempt),
    _autostart(autostart),
    _timeslice(timeslice),
    _stack(stack),
    _stack_size(stack_size)
{
}

void TXX::ThreadBase::create() {
  auto result = tx_thread_create(&_thread, (char *)name.c_str(),
                                  ThreadBase::launch, (ULONG)this,
                                  _stack, _stack_size,
                                  _priority, _preempt,
                                  _timeslice,
                                  _autostart ? TX_AUTO_START : TX_DONT_START);

  if (result != 0) {
    __asm volatile ("BKPT     %0" : : "i"(0));
  }
}
