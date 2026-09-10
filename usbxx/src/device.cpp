/*
 * usbxx.cpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */
#include <stdexcept>
#include <cstring>
#include <cassert>

#include <usbxx/usbxx.hpp>

#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>

#include <usb.h>

/* Includes ------------------------------------------------------------------*/
#include <usbxx/ux_device_descriptors.h>

UX_SYSTEM ux_system;
UX_SYSTEM *_ux_system = &ux_system;

using namespace USBXX;

/* Define USBX Host variable.  */
UX_SYSTEM_SLAVE ux_system_slave;
UX_SYSTEM_SLAVE *_ux_system_slave = &ux_system_slave;


void DeviceBase::thread_entry()
{
  dbg::dbgout << "Initializing USB Hardware" << std::endl;

  dcd->initialize();

  // Call app thread, if desired

  dbg::dbgout << "Hardware initialized" << std::endl;
}


DeviceBase::DeviceBase() :
    fs_desc(USBD_FULL_SPEED),
    hs_desc(USBD_HIGH_SPEED),
    control_thread(this),
    control_request_sema("USB Device Control Thread Semaphore"),
    control_xfer(nullptr)
{
  ::memset(&ux_system_slave, 0, sizeof(ux_system_slave));
}


#include <threadxx/ring_buffer.hpp>

TXX::ring_buffer_base<int, 32> event_ring;

void DeviceBase::setup_device()
{
  fs_desc.build();
  hs_desc.build();

  strings.add_string(USBD_IDX_MFC_STR, get_manufacturer());
  strings.add_string(USBD_IDX_PRODUCT_STR, get_product());
  strings.add_string(USBD_IDX_SERIAL_STR, get_serial());

  lang_ids.add_language();
}

const char *what = nullptr;

void DeviceBase::start()
{
  try {
    start_system();
    setup_device();
    class_init();
    start_app();
  } catch(USBXX::runtime_error &e) {
    what = e.what();
    dbg::dbgout << "Exception in starting USB device: " << e.what() << std::endl;
    __asm volatile ("BKPT     %0" : : "i"(0));
  } catch (const std::exception &e) {
    what = e.what();
    dbg::dbgout << "Unknown exception in starting USB device!" << std::endl;
    __asm volatile ("BKPT     %0" : : "i"(0));
  }
}

void DeviceBase::disconnect()
{
  if (state == UX_DEVICE_CONFIGURED)
  {
        /* Get the pointer to the first interface.  */
    for (auto iface : interfaces) {
      auto class_ptr =  iface -> usb_class;

      if (class_ptr != nullptr)
        /*class_ptr ->*/ class_deactivate();

      iface->stop();
    }

    state =  UX_DEVICE_ATTACHED;
  }

  /* If the device was attached, we need to destroy the control endpoint.  */
  if (state == UX_DEVICE_ATTACHED)
    get_control_endpoint()->destroy();

  /* We are reverting to configuration 0.  */
  configuration_selected =  0;

  /* Set the device to be non attached.  */
  state =  UX_DEVICE_RESET;

  on_removed();
}

uint32_t DeviceBase::register_class(USBClass::ptr p_class,
                        uint32_t configuration_number,
                        uint32_t interface_number,
                        void *parameter)
{
  UINT     status;

  p_class->interface_parameter =  parameter;
  p_class->configuration_number =  configuration_number;
  p_class->interface_number =  interface_number;

  classes.push_back(p_class);

  /* Call the class initialization routine.  */
  status = /* class_inst-> */ class_initialize();

  /* Check the status.  */
  if (status != UX_SUCCESS)
      return(status);

  /* Return successful completion.  */
  return 0;
}

uint32_t DeviceBase::get_interface(uint8_t interface_value)
{

Transfer       *xfer;
uint32_t                    retval;

    auto endpoint = get_control_endpoint();

    /* If the device was in the configured state, there may be interfaces
       attached to the configuration.  */
    if (state == UX_DEVICE_CONFIGURED)
    {
      for (auto iface : interfaces) {
        if (iface -> descriptor.bInterfaceNumber == interface_value)
          xfer = get_control_transfer();

        /* Set the value of the alternate setting in the buffer.  */
        *xfer -> data =
            (UCHAR) iface -> descriptor.bAlternateSetting;

        /* Setup the length appropriately.  */
        xfer -> requested_length =  1;

        /* Set the phase of the transfer to data out.  */
        xfer -> phase =  TransferPhase::DATA_OUT;

        /* Send the descriptor with the appropriate length to the host.  */
        retval = xfer->transfer();

        /* Return the function status code.  */
        return(retval);
      }
    }

    /* The alternate setting value was not found, so we return a stall error.  */
    endpoint->stall();

    /* Return the status to the caller.  */
    return(UX_ERROR);
}

void DeviceBase::uninitialize(void)
{
  Transfer               *xfer;

  /* Allocate some memory for the Control Endpoint.  First get the address of the transfer request for the
       control endpoint. */
  xfer = get_control_transfer();

  /* Free memory for the control endpoint buffer.  */
  ::free(xfer -> data);

  // TODO -- RELEASE ALL ENDPOINTS
}


uint32_t DeviceBase::set_feature(const ControlRequest &req)
{
  auto endpoint = get_control_endpoint();

  /* The feature can be for either the device or the endpoint.  */
  switch (req.recipient)
  {

  case RequestRecipient::DEVICE:

    /* Check if we have a DEVICE_REMOTE_WAKEUP Feature.  */
    if (req.value == UX_REQUEST_FEATURE_DEVICE_REMOTE_WAKEUP)
    {
      if (_ux_system_slave -> ux_system_slave_remote_wakeup_capability)
      {
        _ux_system_slave -> ux_system_slave_remote_wakeup_enabled = UX_TRUE;
        return (UX_SUCCESS);
      }
      else
        return (UX_FUNCTION_NOT_SUPPORTED);
    }

#ifdef UX_OTG_SUPPORT
        /* Check if we have a A_HNP_SUPPORT Feature. This is set when the Host is HNP capable. */
        if (request_value == UX_OTG_FEATURE_A_HNP_SUPPORT)
        {

            /* Store the A_HNP_SUPPORT flag.  */
            _ux_system_otg -> ux_system_otg_slave_set_feature_flag |= UX_OTG_FEATURE_A_HNP_SUPPORT;

            /* OK.  */
            return 0;
        }

        /* Check if the host asks us to perform HNP.  If also we become the host.  */
        if (request_value == UX_OTG_FEATURE_B_HNP_ENABLE)
        {

            /* The ISR will pick up the suspend event and check if we need to become IDLE or HOST.  */
            _ux_system_otg -> ux_system_otg_slave_set_feature_flag |= UX_OTG_FEATURE_B_HNP_ENABLE;

            /* OK.  */
            return 0;
        }
#endif

        /* Request value not supported.  */
    return(UX_FUNCTION_NOT_SUPPORTED);

  case RequestRecipient::ENDPOINT:
  {
    auto eptgt = dcd->get_endpoint(req.index);

    if (eptgt == nullptr) {
      get_control_endpoint()->stall();
      return 0;
    }

    eptgt->stall();

    return 0;
  }

  default:
    endpoint->stall();
    return 0;
  }
}

uint32_t DeviceBase::set_interface(DescriptorIterator &di,
    uint32_t alternate_setting_value)
{
  UINT  status;

  interfaces.push_back(std::make_shared<Interface>(this));

  auto iface = interfaces.back();

  iface->descriptor = di->read_in<InterfaceDescriptor>();

  ++di;


  for(; di != DescriptorIterator::end(); ++di)
  {
    /* Check if this is an endpoint descriptor.  */
    switch(di.type())
    {
      case UX_ENDPOINT_DESCRIPTOR_ITEM:
      {
        auto desc = di->read_in<EndpointDescriptor>();
          /* Find a free endpoint in the pool and hook it to the
             existing interface after it's created by DCD.  */

        auto endpoint = dcd->allocate_endpoint(iface, desc);

        /* Create the endpoint at the DCD level.  */
        status = endpoint->create();

          /* Do a sanity check on endpoint creation.  */
        if (status != UX_SUCCESS)
        {
          assert(0);
          return(status);
        }

        iface->endpoints.push_back(endpoint);
      }
      break;

      case UX_CONFIGURATION_DESCRIPTOR_ITEM:
      case UX_INTERFACE_DESCRIPTOR_ITEM:
        return iface->start();

      default:
        break;
    }
  }

  /* The interface attached to this configuration must be started at the class
     level.  */
  return iface->start();
}


uint32_t  DeviceBase::clear_feature(const ControlRequest &req)
{
    /* Get the control endpoint for the device.  */
    auto endpoint = get_control_endpoint();

    /* The request can be for either the device or the endpoint.  */
    switch (req.recipient)
    {
    case RequestRecipient::DEVICE:

        /* Check if we have a DEVICE_REMOTE_WAKEUP Feature.  */
        if (req.value == UX_REQUEST_FEATURE_DEVICE_REMOTE_WAKEUP)
        {

            /* Check if we have the capability. */
            if (_ux_system_slave -> ux_system_slave_remote_wakeup_capability)
            {

                /* Disable the feature. */
                _ux_system_slave -> ux_system_slave_remote_wakeup_enabled = UX_FALSE;
            }

            else

                /* Protocol error. */
                return (UX_FUNCTION_NOT_SUPPORTED);
        }

        break;

    case RequestRecipient::ENDPOINT:

        /* The only clear feature for endpoint is ENDPOINT_STALL. This clears
           the endpoint of the stall situation and resets its data toggle.
           We need to find the endpoint through the interface(s). */
      for(auto iface : interfaces) {
          for (auto endpoint_target : iface->endpoints) {
              if (endpoint_target -> descriptor.bEndpointAddress != req.index)
                continue;

              endpoint_target->reset();
              endpoint_target -> state = UX_ENDPOINT_RESET;
              return 0;
            }
      }

        /* Intentional fallthrough and go into the default case. */
        /* fall through */

    /* We get here when the endpoint is wrong. Should not happen though.  */
    default:

        /* We stall the command.  */
      endpoint->stall();

        /* No more work to do here.  The command failed but the upper layer does not depend on it.  */
        return 0;
    }

    /* Return the function status.  */
    return 0;
}
