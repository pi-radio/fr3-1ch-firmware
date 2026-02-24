/*
 * txxeventflags.cpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */
#include <threadxx/eventflags.hpp>


TXX::EventFlagsGroup::EventFlagsGroup(const std::string &name,
    bool create_immediate) : _name(name)
{
  if (create_immediate) create();
}

void TXX::EventFlagsGroup::create()
{
  tx_event_flags_create(&_flags, (char *)_name.c_str());
}

TXX::EventFlag TXX::EventFlagsGroup::operator[](int n)
{
  return EventFlag(*this, n);
}

bool TXX::EventFlag::get()
{
}

bool TXX::EventFlag::get_nowait()
{
}

bool TXX::EventFlag::get_clear()
{
}

void TXX::EventFlag::set(bool)
{
}

#if 0

  };

  class EventFlag
  {
    friend class EventFlagsGroup;
    EventFlagsGroup &_evtflags;
    int _n;

    EventFlag(EventFlagsGroup &evtflags, int n) : _evtflags(evtflags), _n(n) { }

  public:
    bool get();
    bool get_nowait();
    bool get_clear();
    void set(bool);
  };


  class Events
  {
    std::vector<EventFlagsGroup *> _evt_flag_grps;

  public:
    Events();

    EventFlag alloc();
  };
};



#endif
