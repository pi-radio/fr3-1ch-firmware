#pragma once

#include <string>

#include <threadxx/static.hpp>

namespace TXX
{
  class object_manager;
  class AppBase;

  class object
  {
    friend class object_manager;
    friend class __txx_initializer;
    friend class AppBase;

    std::string name;

    static void initialize();
    static void on_enter_kernel();

    virtual void create() = 0;

  public:
    object(const std::string &_name);
  };
}
