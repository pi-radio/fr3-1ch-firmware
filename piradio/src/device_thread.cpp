#include <piradio/hardware.hpp>

using namespace piradio::hardware;

DeviceThread::DeviceThread() : Thread("HW Request Thread"),
    request_queue("HW Request Queue")
{
}



void DeviceThread::main()
{

}
