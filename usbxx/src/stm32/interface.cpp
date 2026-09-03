#include <usbxx/device.hpp>

#include <usbxx/stm32/interface.hpp>


using namespace USBXX;

UINT USBXX::_ux_device_stack_interface_start(Interface *interface_ptr)
{
  UX_SLAVE_CLASS              *class_ptr;
  UINT                        status;
  UX_SLAVE_CLASS_COMMAND      class_command;


    /* Get the class for the interface.  */
    class_ptr =  _ux_system_slave -> ux_system_slave_interface_class_array[interface_ptr -> descriptor.bInterfaceNumber];

    /* Check if class driver is available. */
    if (class_ptr == nullptr)

        /* There is no class driver supported. */
        return (UX_NO_CLASS_MATCH);

    /* Get the pointer to the device.  */
    auto device = _ux_system_slave->device;

    /* Build all the fields of the Class Command.  */
    class_command.ux_slave_class_command_request   =    UX_SLAVE_CLASS_COMMAND_QUERY;
    class_command.ux_slave_class_command_interface =   (VOID *)interface_ptr;
    class_command.ux_slave_class_command_class     =   interface_ptr -> descriptor.bInterfaceClass;
    class_command.ux_slave_class_command_subclass  =   interface_ptr -> descriptor.bInterfaceSubClass;
    class_command.ux_slave_class_command_protocol  =   interface_ptr -> descriptor.bInterfaceProtocol;
    class_command.ux_slave_class_command_vid       =   device -> descriptor.idVendor;
    class_command.ux_slave_class_command_pid       =   device -> descriptor.idProduct;

    /* We can now memorize the interface pointer associated with this class.  */
    class_ptr -> ux_slave_class_interface = interface_ptr;

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
            interface_ptr -> usb_class =  class_ptr;

        return(status);
    }

    /* There is no driver who want to own this class!  */
    return(UX_NO_CLASS_MATCH);
}


UINT USBXX::_ux_device_stack_interface_delete(Interface *interface_ptr)
{
  Endpoint       *endpoint;
  Endpoint       *next_endpoint;

    /* If trace is enabled, register this object.  */
    UX_TRACE_OBJECT_UNREGISTER(interface_ptr);

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_INTERFACE_DELETE, interface_ptr, 0, 0, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)

    /* Get the pointer to the device.  */
    auto device = _ux_system_slave->device;

    for (auto endpoint : interface_ptr->endpoints) {
        endpoint->destroy();
        endpoint->used = false;
        endpoint -> ux_slave_endpoint_state =  0;
        endpoint -> ux_slave_endpoint_next_endpoint =  nullptr;
        endpoint -> ux_slave_endpoint_interface =  nullptr;
        endpoint -> ux_slave_endpoint_device =  nullptr;
    }

    /* It's always from first one (to delete).  */
    /* Rebuild the first link.  */
    device -> first_interface =  interface_ptr -> next_interface;

    /* The interface is removed from the link, its memory must be cleaned and returned to the pool.  */
    interface_ptr -> usb_class      =  nullptr;
    interface_ptr -> class_instance =  nullptr;
    interface_ptr -> next_interface =  nullptr;
    interface_ptr -> status         =  UX_UNUSED;

    /* Return successful completion.  */
    return 0;
}
