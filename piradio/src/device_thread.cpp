#include <piradio/hardware.hpp>

using namespace piradio::hardware;

DeviceThread::DeviceThread() : Thread("HW Request Thread"),
    request_queue("HW Request Queue")
{
  // Needs to be moved to post-threadx initializer
  request_queue.create();
}



void DeviceThread::main()
{

}
