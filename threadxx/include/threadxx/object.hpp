#pragma once

#include <string>

#include <threadxx/static.hpp>

namespace TXX
{
  class object;
  class object_manager;
  class AppBase;

#if 0
  class creator_base
  {
  public:
    virtual void create(object *) const = 0;
  };

  template <typename T>
  class creator : public creator_base
  {
  public:
    void create(object *obj) const override { ((T *)obj)->create(); }
  };
#endif


  class object
  {
    friend class object_manager;
    friend class __txx_initializer;
    friend class AppBase;

    static void initialize();
    static void on_enter_kernel();

  protected:
    std::string name;

  public:
    object(const std::string &);
  };
}
