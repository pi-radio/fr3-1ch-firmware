#pragma once

#include <stdexcept>



namespace USBXX
{
  class runtime_error : public std::runtime_error
  {
  public:
    runtime_error(const std::string &_what);
  };

}
