#pragma once

#include <threadxx/dllist.hpp>

namespace piradio
{
  namespace hardware
  {
    class Request
    {
      TXX::list::DLListEntry<Request> list_entry;

      virtual int process() = 0;

    public:
      Request() {}
    };

    class DeviceThread
    {

    };
  }
}
