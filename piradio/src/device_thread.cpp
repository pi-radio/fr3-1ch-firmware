#include <piradio/hardware.hpp>

using namespace piradio::hardware;

DeviceThread::DeviceThread() : request_sema("HW Request Semaphore"),
                               request_mutex("HW Request Mutex"),
                               Thread("HW Request Thread")
{
  request_sema.create();
}

void DeviceThread::main()
{

}
