/*
 * txxeventflags.cpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */
#include <format>

#include <threadxx/eventflags.hpp>

using namespace TXX;

EventFlagsGroup::EventFlagsGroup(const std::string &name,
    bool create_immediate) : _name(name)
{
  if (create_immediate) create();
}

void EventFlagsGroup::create()
{
  tx_event_flags_create(&_flags, (char *)_name.c_str());
}

EventFlag EventFlagsGroup::operator[](int n)
{
  return EventFlag(this, n);
}

bool EventFlag::get()
{
  ULONG afp;

  ULONG retval = tx_event_flags_get(&_evtflags->_flags, (1 << _n), TX_AND, &afp, TX_WAIT_FOREVER);

  return retval == TX_SUCCESS;
}

bool EventFlag::get_nowait()
{
  ULONG afp;

  ULONG retval = tx_event_flags_get(&_evtflags->_flags, (1 << _n), TX_AND, &afp, TX_NO_WAIT);

  return retval == TX_SUCCESS;
}

bool EventFlag::get_clear()
{
  ULONG afp;

  ULONG retval = tx_event_flags_get(&_evtflags->_flags, (1 << _n), TX_AND_CLEAR, &afp, TX_WAIT_FOREVER);

  return retval == TX_SUCCESS;
}

void EventFlag::clear()
{
  tx_event_flags_set(&_evtflags->_flags, ~(1 << _n), TX_AND);
}

void EventFlag::set()
{
  tx_event_flags_set(&_evtflags->_flags, (1 << _n), TX_OR);
}


Events::Events() : cur_n(32)
{
}


void Events::alloc_group()
{
  _evt_flag_grps.push_back(new EventFlagsGroup(std::format("Event Flag Group {}", _evt_flag_grps.size()), true));
}

TXX::EventFlag TXX::Events::alloc()
{
  if (cur_n == 32) {
    alloc_group();
    cur_n = 0;
  }

  return (*_evt_flag_grps.back())[cur_n++];
}

