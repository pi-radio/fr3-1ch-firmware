#pragma once

namespace consolexx
{
  struct event
  {
    uint32_t type;

    event(uint32_t _type) : type(_type) {

    }

    template <typename T>
    static std::shared_ptr<T> to(std::shared_ptr<event> &evt) {
      return std::static_pointer_cast<T>(evt);
    }

    template <typename T>
    static const std::shared_ptr<T> to(const std::shared_ptr<event> &evt) {
      return std::static_pointer_cast<T>(evt);
    }

  };

  typedef std::shared_ptr<event> evt_ptr;

  struct input_event : public event
  {
    static const uint32_t EVENT_TYPE = 1;

    std::string s;

    input_event(const std::string &_s) : event(EVENT_TYPE), s(_s) {}
  };

  struct error_event : public event
  {
    static const uint32_t EVENT_TYPE = 2;

    std::string s;

    error_event() : event(EVENT_TYPE) {}
  };

  struct event_handler
  {
    virtual void on_event(const evt_ptr &evt) = 0;
  };

  template <typename T, typename... Rest>
  std::shared_ptr<T> create_event(Rest... args) {
    return new T(args...);
  }

};
