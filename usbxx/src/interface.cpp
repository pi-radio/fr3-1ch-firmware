#include <usbxx/device.hpp>

#include <usbxx/stm32/interface.hpp>


using namespace USBXX;

uint32_t Interface::start()
{
  USBClass::ptr class_ptr;
  uint32_t                        status;

    /* Get the class for the interface.  */
    class_ptr =  device->get_interface_class(descriptor.bInterfaceNumber);

    /* Check if class driver is available. */
    if (class_ptr == nullptr)

        /* There is no class driver supported. */
        return (UX_NO_CLASS_MATCH);


    /* We can now memorize the interface pointer associated with this class.  */
    class_ptr->interface = shared_from_this();

    /* We have found a potential candidate. Call this registered class entry function.  */
    if (/* class_ptr-> */ device->class_query(shared_from_this()))
    {
        status = /*class_ptr -> */ device->class_activate(shared_from_this());

        if(status == 0)
            usb_class =  class_ptr;

        return(status);
    }

    /* There is no driver who want to own this class!  */
    return(UX_NO_CLASS_MATCH);
}


uint32_t Interface::stop()
{
  for (auto endpoint : endpoints) {
      endpoint->destroy();
  }

  endpoints.clear();

  return 0;
}
