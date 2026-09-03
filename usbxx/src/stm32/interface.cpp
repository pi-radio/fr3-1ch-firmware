#include <usbxx/device.hpp>

#include <usbxx/stm32/interface.hpp>


using namespace USBXX;

uint32_t Interface::start()
{
  UX_SLAVE_CLASS              *class_ptr;
  UINT                        status;
  UX_SLAVE_CLASS_COMMAND      class_command;


    /* Get the class for the interface.  */
    class_ptr =  _ux_system_slave -> ux_system_slave_interface_class_array[descriptor.bInterfaceNumber];

    /* Check if class driver is available. */
    if (class_ptr == nullptr)

        /* There is no class driver supported. */
        return (UX_NO_CLASS_MATCH);

    /* Build all the fields of the Class Command.  */
    class_command.ux_slave_class_command_request   =    UX_SLAVE_CLASS_COMMAND_QUERY;
    class_command.ux_slave_class_command_interface =   shared_from_this();
    class_command.ux_slave_class_command_class     =   descriptor.bInterfaceClass;
    class_command.ux_slave_class_command_subclass  =   descriptor.bInterfaceSubClass;
    class_command.ux_slave_class_command_protocol  =   descriptor.bInterfaceProtocol;
    class_command.ux_slave_class_command_vid       =   device -> descriptor.idVendor;
    class_command.ux_slave_class_command_pid       =   device -> descriptor.idProduct;

    /* We can now memorize the interface pointer associated with this class.  */
    class_ptr -> ux_slave_class_interface = shared_from_this();

    /* We have found a potential candidate. Call this registered class entry function.  */
    status = class_ptr -> ux_slave_class_entry_function(&class_command);

    /* The status tells us if the registered class wants to own this class.  */
    if (status == UX_SUCCESS)
    {

        /* Store the class container. */
        class_command.ux_slave_class_command_class_ptr =  class_ptr;

        /* Store the command.  */
        class_command.ux_slave_class_command_request =  UX_SLAVE_CLASS_COMMAND_ACTIVATE;

        /* Activate the class.  */
        status = class_ptr -> ux_slave_class_entry_function(&class_command);

        /* If the class was successfully activated, set the class for the interface.  */
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
