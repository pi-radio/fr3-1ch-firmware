#pragma once

#include <consolexx/event.hpp>

namespace consolexx
{
  struct termobj : public event_handler
  {
    termobj *_parent;

    termobj(termobj *parent = nullptr) : _parent(parent) {}

    void on_event(const evt_ptr &evt) override {
      if (_parent != nullptr) {
        _parent->on_event(evt);
      }
    }

    template <typename T, typename... Rest>
    void emit(Rest... args) {
      if (_parent == nullptr) return;

      auto evt = std::make_shared<T>(args...);

      _parent->on_event(evt);
    }

    template <typename T>
    T *parent() const { return (T *)_parent; }

  };
}
