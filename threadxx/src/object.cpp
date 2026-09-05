#include <threadxx/object.hpp>
#include <threadxx/app.hpp>

#include <vector>

using namespace TXX;

namespace TXX
{
  class object_manager
  {
    friend class TXX::object;

    bool in_kernel;
    std::vector<object *> objects;
    std::vector<std::tuple<object *, const creator_base &> > deferred;

    object_manager() : in_kernel(0)
    {
    }

    void register_object(object *obj)
    {
      objects.push_back(obj);
    }

    void on_enter_kernel()
    {
      for (auto t : deferred) {
        std::get<1>(t).create(std::get<0>(t));
      }

      in_kernel = true;
    }

  };

  static typename std::aligned_storage<sizeof(object_manager),
                                       alignof(object_manager)>::type mgr_buf;
  object_manager &mgr = reinterpret_cast<object_manager &>(mgr_buf);

  static int _counter;
}

void object::initialize()
{
  using namespace std;
  if (_counter++ == 0) {
    new (&mgr) object_manager();
  }
}

void object::on_enter_kernel()
{
  mgr.on_enter_kernel();
}


object::object(const std::string &_name, const creator_base &_creator) : name(_name)
{
  if (!mgr.in_kernel) {
    mgr.deferred.emplace_back(this, _creator);
  } else {
    _creator.create(this);
  }

  mgr.register_object(this);
}
