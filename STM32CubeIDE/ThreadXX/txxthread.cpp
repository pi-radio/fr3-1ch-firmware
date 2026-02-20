/*
 * txxthread.cpp
 *
 *  Created on: Feb 19, 2026
 *      Author: zapman
 */

#include <txxthread.hpp>
#include <dbgstream.hpp>

TXX::ThreadBase::ThreadBase(const std::string &name,
    uint8_t *stack,
    size_t stack_size,
    int priority,
    int preempt,
    int timeslice,
    bool autostart) : _name(name),
        _stack(stack),
        _stack_size(stack_size),
        _priority(priority),
        _preempt(preempt),
        _timeslice(timeslice),
        _autostart(autostart)
{
}

void TXX::ThreadBase::create() {
  tx_thread_create(&_thread, (char *)_name.c_str(),
            ThreadBase::launch, (ULONG)this,
            _stack, _stack_size,
            _priority, _preempt,
            _timeslice,
            _autostart ? TX_AUTO_START : TX_DONT_START);
}
