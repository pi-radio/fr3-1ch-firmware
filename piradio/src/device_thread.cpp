#include <piradio/hardware.hpp>

using namespace piradio::hardware;

DeviceThread::DeviceThread() : Thread("Device Driver Request Thread"),
    request_queue("HW Request Queue")
{
}



void DeviceThread::main()
{
  while (true) {
    auto r = request_queue.pop();

    r->do_process();
  }
}


int DeviceThread::process(Request::ptr p)
{
  request_queue.push(p);

  p->sema.get();

  return p->result;
}


DeviceThread DeviceThread::hw_thread;
