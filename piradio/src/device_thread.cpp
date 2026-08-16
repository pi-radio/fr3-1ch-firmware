#include <piradio/hardware.hpp>

using namespace piradio::hardware;

DeviceThread::DeviceThread() : request_queue("HW Request Queue"),
                               Thread("HW Request Thread")
{
  // Needs to be moved to post-threadx initializer
  request_queue.create();
}



void DeviceThread::main()
{

}
