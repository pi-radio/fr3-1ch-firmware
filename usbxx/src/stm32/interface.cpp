#include <usbxx/device.hpp>

#include <usbxx/stm32/interface.hpp>


using namespace USBXX;

uint32_t Interface::start()
{
  USBClass              *class_ptr;
  UINT                        status;

    /* Get the class for the interface.  */
    class_ptr =  _ux_system_slave -> ux_system_slave_interface_class_array[descriptor.bInterfaceNumber];

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

        if(status == UX_SUCCESS)
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
      endpoint->used = false;
      endpoint -> ux_slave_endpoint_state =  0;
      endpoint -> ux_slave_endpoint_next_endpoint =  nullptr;
      endpoint -> ux_slave_endpoint_interface =  nullptr;
      endpoint -> ux_slave_endpoint_device =  nullptr;
  }

  return 0;
}
