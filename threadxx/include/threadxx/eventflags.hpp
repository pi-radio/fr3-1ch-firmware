/*
 * txxeventflags.hpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */

#ifndef TXXEVENTFLAGS_HPP_
#define TXXEVENTFLAGS_HPP_

#include <stdint.h>

#include <vector>
#include <string>

#include <tx_api.h>

namespace TXX {
  class EventFlag;

  class EventFlagsGroup
  {
    friend class EventFlag;
    TX_EVENT_FLAGS_GROUP _flags;
    const std::string &_name;

  public:
    EventFlagsGroup(const std::string &, bool create_immediate=false);

    void create();

    EventFlag operator[](int n);
  };

  class EventFlag
  {
    friend class EventFlagsGroup;
    EventFlagsGroup *_evtflags;
    int _n;

    EventFlag(EventFlagsGroup *evtflags, int n) : _evtflags(evtflags), _n(n) { }

  public:
    EventFlag() : _evtflags(nullptr), _n(-1) {}
    EventFlag(const EventFlag &o) : _evtflags(o._evtflags), _n(o._n) {}

    bool get();
    bool get_nowait();
    bool get_clear();
    void set();
    void clear();
  };


  class Events
  {
    std::vector<EventFlagsGroup *> _evt_flag_grps;
    uint32_t cur_n;

    void alloc_group();

  public:
    Events();

    EventFlag alloc();
  };

  extern Events events;
};




#endif /* TXXEVENTFLAGS_HPP_ */
