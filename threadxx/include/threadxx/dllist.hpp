#pragma once

namespace TXX
{
  namespace list
  {
    template <class T> class DLList;

    template <class T>
    class DLListEntry
    {
      friend class DLList<T>;

      T *prev;
      T *next;
    };

  }
}
