#include <usbxx/endpoint.hpp>

using namespace USBXX;

Endpoint::Endpoint(USBXX::DeviceBase *_device) :
      device(_device),
      interface(nullptr)
{
}

Endpoint::~Endpoint()
{
  device = nullptr;
  interface = nullptr;
}
