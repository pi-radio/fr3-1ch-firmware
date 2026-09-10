/*
 * usbxxdescriptor.cpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */

#include <cassert>
#include <algorithm>

#include <threadxx/dbgstream.hpp>

#include <usbxx/endian.hpp>
#include <usbxx/class.hpp>
#include <usbxx/descriptor.hpp>
#include <usbxx/device.hpp>

using namespace USBXX;

extern "C" {
/* Includes ------------------------------------------------------------------*/
//#include <usbxx/ux_device_descriptors.h>
}

#ifndef   __PACKED
#define __PACKED                               __attribute__((packed, aligned(1)))
#endif


#define USBD_VID                                      1155
#define USBD_PID                                      22288
#define USBD_LANGID_STRING                            1033
#define USBD_MANUFACTURER_STRING                      "Pi Radio"
#define USBD_PRODUCT_STRING                           "FR3 1CH"
#define USBD_SERIAL_NUMBER                            "000000000001"

#define USB_DESC_TYPE_INTERFACE                       0x04U
#define USB_DESC_TYPE_ENDPOINT                        0x05U
#define USB_DESC_TYPE_CONFIGURATION                   0x02U
#define USB_DESC_TYPE_IAD                             0x0BU

#define USBD_EP_TYPE_CTRL                             0x00U
#define USBD_EP_TYPE_ISOC                             0x01U
#define USBD_EP_TYPE_BULK                             0x02U
#define USBD_EP_TYPE_INTR                             0x03U


#define USB_BCDUSB                                    0x0200U
#define LANGUAGE_ID_MAX_LENGTH                        2U

#define USBD_IDX_MFC_STR                              0x01U
#define USBD_IDX_PRODUCT_STR                          0x02U
#define USBD_IDX_SERIAL_STR                           0x03U

#define USBD_MAX_EP0_SIZE                             64U
#define USBD_DEVICE_QUALIFIER_DESC_SIZE               0x0AU

#define USBD_STRING_FRAMEWORK_MAX_LENGTH              256U

/* Device CDC-ACM Class */
#define USBD_CDCACM_EPINCMD_ADDR                      0x81U
#define USBD_CDCACM_EPINCMD_FS_MPS                    8U
#define USBD_CDCACM_EPINCMD_HS_MPS                    8U
#define USBD_CDCACM_EPIN_ADDR                         0x82U
#define USBD_CDCACM_EPOUT_ADDR                        0x03U
#define USBD_CDCACM_EPIN_FS_MPS                       64U
#define USBD_CDCACM_EPIN_HS_MPS                       512U
#define USBD_CDCACM_EPOUT_FS_MPS                      64U
#define USBD_CDCACM_EPOUT_HS_MPS                      512U
#define USBD_CDCACM_EPINCMD_FS_BINTERVAL              5U
#define USBD_CDCACM_EPINCMD_HS_BINTERVAL              5U

#ifndef USBD_CONFIG_STR_DESC_IDX
#define USBD_CONFIG_STR_DESC_IDX                      0U
#endif /* USBD_CONFIG_STR_DESC_IDX */

#ifndef USBD_CONFIG_BMATTRIBUTES
#define USBD_CONFIG_BMATTRIBUTES                      0xC0U
#endif /* USBD_CONFIG_BMATTRIBUTES */



USBXX::Descriptor::Descriptor(int _speed) : speed(_speed)
{
  p_cur = desc;
}

#if 0
typedef USBXX::EndpointDesc USBD_EPTypeDef;
typedef USBXX::CompositeElement USBD_CompositeElementTypeDef;
typedef USBXX::CompositeClass USBD_CompositeClassTypeDef;
#endif

void USBXX::Descriptor::build()
{
  auto *pDevDesc = allocate_section<DeviceDescriptor>();

  pDevDesc->bDescriptorType = DeviceDescriptor::desc_type;
  pDevDesc->bcdUSB = USB_BCDUSB;
  pDevDesc->bDeviceClass = 0x00;
  pDevDesc->bDeviceSubClass = 0x00;
  pDevDesc->bDeviceProtocol = 0x00;
  pDevDesc->bMaxPacketSize0 = USBD_MAX_EP0_SIZE;
  pDevDesc->idVendor = USBD_VID;
  pDevDesc->idProduct = USBD_PID;
  pDevDesc->bcdDevice = 0x0200;
  pDevDesc->iManufacturer = USBD_IDX_MFC_STR;
  pDevDesc->iProduct = USBD_IDX_PRODUCT_STR;
  pDevDesc->iSerialNumber = USBD_IDX_SERIAL_STR;
  pDevDesc->bNumConfigurations = USBD_MAX_NUM_CONFIGURATION;

  if (speed == USBD_HIGH_SPEED)
  {
    auto *pDevQualDesc = allocate_section<DeviceQualifierDescriptor>();
    pDevQualDesc->bcdUSB = 0x0200;
    pDevQualDesc->bDeviceClass = 0x00;
    pDevQualDesc->bDeviceSubClass = 0x00;
    pDevQualDesc->bDeviceProtocol = 0x00;
    pDevQualDesc->bMaxPacketSize0 = 0x40;
    pDevQualDesc->bNumConfigurations = 0x01;
    pDevQualDesc->bReserved = 0x00;
  }

  if (usb_classes.size() > 1) {
    pDevDesc->bDeviceClass = 0xEF;
    pDevDesc->bDeviceSubClass = 0x02;
    pDevDesc->bDeviceProtocol = 0x01;
  } else {
    // Hard wiring to just get done
    assert(usb_classes[0] == CompositeClass::CLASS_TYPE_CDC_ACM);
    pDevDesc->bDeviceClass = 0x02;
    pDevDesc->bDeviceSubClass = 0x02;
    pDevDesc->bDeviceProtocol = 0x00;
  }

  uint32_t config_start = get_desc_len();
  auto *config_desc = allocate_section<ConfigurationDescriptor>();

  config_desc->wTotalLength = 0U;
  config_desc->bNumInterfaces = 0U;
  config_desc->bConfigurationValue = 1U;
  config_desc->iConfiguration = USBD_CONFIG_STR_DESC_IDX;
  config_desc->bmAttributes = USBD_CONFIG_BMATTRIBUTES;
  config_desc->MaxPower = USBD_CONFIG_MAXPOWER;

  /* Build the device framework */
  for(auto cls : usb_classes)
  {
    add_class_to_conf(cls);
  }

  config_desc->bNumInterfaces = interfaces.size();
  config_desc->wTotalLength = get_desc_len() - config_start;
}

uint8_t USBXX::Descriptor::allocate_interface(uint8_t cls)
{
  uint8_t retval = interfaces.size();

  interfaces[retval].cls = cls;
  interfaces[retval].type = 0;

  classes[cls].interfaces.push_back(retval);

  return retval;
}

uint8_t USBXX::Descriptor::assign_endpoint(uint8_t cls, uint8_t addr, uint8_t type, uint16_t size)
{
  classes[cls].endpoints.push_back(addr);

  endpoints[addr].cls = cls;
  endpoints[addr].addr = addr;
  endpoints[addr].type = type;
  endpoints[addr].size = size;

  return addr;
}

void USBXX::Descriptor::add_endpoint_desc(uint8_t epaddr, uint8_t interval)
{
  auto *pEpDesc = allocate_section<EndpointDescriptor>();

  pEpDesc->bEndpointAddress   = (epaddr);
  pEpDesc->bmAttributes       = endpoints[epaddr].type;
  pEpDesc->wMaxPacketSize     = endpoints[epaddr].size;
  pEpDesc->bInterval          = interval;
}

void USBXX::Descriptor::add_interface_desc(uint8_t ifnum,
    uint8_t alt,
    uint8_t eps,
    uint8_t cls,
    uint8_t subclass,
    uint8_t protocol,
    uint8_t istring)
{
  auto *pIfDesc = allocate_section<InterfaceDescriptor>();
  pIfDesc->bInterfaceNumber = (ifnum);
  pIfDesc->bAlternateSetting = (alt);
  pIfDesc->bNumEndpoints = (eps);
  pIfDesc->bInterfaceClass = (cls);
  pIfDesc->bInterfaceSubClass = (subclass);
  pIfDesc->bInterfaceProtocol = (protocol);
  pIfDesc->iInterface = (istring);
}

void USBXX::Descriptor::add_class_to_conf(uint8_t cls)
{
  assert(cls == CLASS_TYPE_CDC_ACM);

  /******************************
   * FIX ME FIX ME FIX ME FIX ME FIX ME
   *
   * Move this to an external class to set up
   *
   * Just doing this here for expediency
   */

  uint8_t iface1 = allocate_interface(cls);
  uint8_t iface2 = allocate_interface(cls);

  assign_endpoint(cls, USBD_CDCACM_EPOUT_ADDR, USBD_EP_TYPE_BULK,
      is_hs() ? USBD_CDCACM_EPOUT_HS_MPS : USBD_CDCACM_EPOUT_FS_MPS);
  assign_endpoint(cls, USBD_CDCACM_EPIN_ADDR, USBD_EP_TYPE_BULK,
      is_hs() ? USBD_CDCACM_EPIN_HS_MPS : USBD_CDCACM_EPIN_FS_MPS);

  assign_endpoint(cls, USBD_CDCACM_EPINCMD_ADDR, USBD_EP_TYPE_INTR,
      is_hs() ? USBD_CDCACM_EPINCMD_HS_MPS : USBD_CDCACM_EPINCMD_FS_MPS);

  auto pIadDesc = allocate_section<InterfaceAssociationDescriptor>();
  pIadDesc->bFirstInterface = iface1;
  pIadDesc->bInterfaceCount = 2U;    /* 2 interfaces */
  pIadDesc->bFunctionClass = 0x02U;
  pIadDesc->bFunctionSubClass = 0x02U;
  pIadDesc->bFunctionProtocol = 0x01U;
  pIadDesc->iFunction = 0; /* String Index */
  /* USER CODE END Private_macro */

  add_interface_desc(iface1, 0U, 1U, 0x02, 0x02U, 0x01U, 0U);


  /* Header Functional Descriptor*/
  auto pHeadDesc = allocate_section<CDCFunctionHeaderDescriptor>();
  pHeadDesc->bLength = 0x05U;
  pHeadDesc->bDescriptorType = 0x24U;
  pHeadDesc->bDescriptorSubtype = 0x00U;
  pHeadDesc->bcdCDC = 0x0110;

  /* Call Management Functional Descriptor*/
  auto pCallMgmDesc = allocate_section<CDCCallManagementFunctionDescriptor>();
  pCallMgmDesc->bLength = 0x05U;
  pCallMgmDesc->bDescriptorType = 0x24U;
  pCallMgmDesc->bDescriptorSubtype = 0x01U;
  pCallMgmDesc->bmCapabilities = 0x00U;
  pCallMgmDesc->bDataInterface = iface2;

  /* ACM Functional Descriptor*/
  auto pACMDesc = allocate_section<CDCACMFunctionDescriptor>();
  pACMDesc->bDescriptorSubtype = 0x02U;
  pACMDesc->bmCapabilities = 0x02;

  /* Union Functional Descriptor*/
  auto pUnionDesc = allocate_section<CDCUnionFunctionDescriptor>();
  pUnionDesc->bDescriptorSubtype = 0x06U;
  pUnionDesc->bMasterInterface = iface1;
  pUnionDesc->bSlaveInterface = iface2;

  /* Append Endpoint descriptor to Configuration descriptor */
  add_endpoint_desc(USBD_CDCACM_EPINCMD_ADDR, 5);

  /* Data Interface Descriptor */
  add_interface_desc(iface2, 0U, 2U, 0x0A, 0U, 0U, 0U);

  /* Append Endpoint descriptor to Configuration descriptor */
  add_endpoint_desc(USBD_CDCACM_EPOUT_ADDR, 0);

  /* Append Endpoint descriptor to Configuration descriptor */
  add_endpoint_desc(USBD_CDCACM_EPIN_ADDR, 0);
}


uint16_t USBXX::Descriptor::get_interface_number(uint8_t cls, uint8_t iface_type)
{
  for (auto iface : classes[cls].interfaces) {
    if (interfaces[iface].type == iface_type) {
      return iface;
    }
  }

  throw USBXX::runtime_error("Unable to find requested interface");
}

void USBXX::Strings::add_string(uint8_t idx, const std::string &s, uint16_t lang_id)
{
  str[pos++] = lang_id & 0xFF;
  str[pos++] = lang_id >> 8;
  str[pos++] = idx;

  str[pos++] = s.size();

  std::memcpy(str + pos, s.c_str(), s.size());
  pos += s.size();
}

uint32_t DeviceBase::send_device_descriptor(uint32_t descriptor_type, uint32_t request_index, uint32_t host_length)
{
  uint32_t length = host_length;
  auto xfer = get_control_transfer();

  if (descriptor_type == DeviceDescriptor::desc_type)
  {
    length = std::min(length, (uint32_t)DeviceDescriptor::length);
  }
  else
  {
    length = std::min(length, (uint32_t)DeviceQualifierDescriptor::length);
  }

  for (auto d : get_current_descriptor()) {
    if (d.cur_type() != descriptor_type) {
      continue;
    }

    ::memcpy(xfer->data, d.buffer, length);

    return transfer_request(xfer, length, host_length);
  }

  return -1;
}


uint32_t DeviceBase::send_compound_descriptor(uint32_t descriptor_type, uint32_t descriptor_index, uint32_t request_index, uint32_t host_length)
{
  BOSDescriptor               bos_descriptor;
  ConfigurationDescriptor     configuration_descriptor;
  uint32_t                        target_descriptor_length = 0;
  uint32_t                        parsed_descriptor_index = 0;

  const Descriptor &desc = (descriptor_type == UX_OTHER_SPEED_DESCRIPTOR_ITEM) ? fs_desc : get_current_descriptor();

  auto di = desc.begin();

  if (descriptor_type == UX_OTHER_SPEED_DESCRIPTOR_ITEM)
    descriptor_type = UX_CONFIGURATION_DESCRIPTOR_ITEM;

  for (; di != desc.end(); ++di)
  {
    auto d = *di;

    if (d.cur_type() != descriptor_type)
      continue;

    if (descriptor_type == BOSDescriptor::desc_type)
    {
      bos_descriptor = d.read_in<BOSDescriptor>();

      target_descriptor_length = bos_descriptor.wTotalLength;
      break;
    }

    if (parsed_descriptor_index == descriptor_index)
    {
      configuration_descriptor = d.read_in<ConfigurationDescriptor>();

      target_descriptor_length = configuration_descriptor.wTotalLength;

      break;
    }

    parsed_descriptor_index++;
  }

  if (di == desc.end()) {
    return -1;
  }

  auto d = *di;

  uint32_t length = std::min(target_descriptor_length, host_length);


  auto xfer = get_control_transfer();

  /* Check buffer length, since total descriptors length may exceed buffer...  */
  if (length > xfer->buffer_size)
  {
    get_control_endpoint()->stall();
    throw USBXX::runtime_error("Control request length too long");
  }


  /* Copy the device descriptor into the transfer request memory.  */
  ::memcpy(xfer->data, d.buffer, length); /* Use case of memcpy is verified. */

  /* Now we need to hack the found descriptor because this request expect a requested
      descriptor type instead of the regular descriptor.  */
  xfer->data[1] = descriptor_type;

  /* We can return the configuration descriptor.  */
  return transfer_request(xfer, length, host_length);

}

uint32_t DeviceBase::send_descriptor(const ControlRequest &req)
{
  Transfer               *xfer;
  uint32_t                            status =  UX_ERROR;
  uint8_t                           *string_memory;
  uint8_t                           *string_framework;
  uint32_t                           string_framework_length;
  uint32_t                           string_length;

  /* Get the pointer to the transfer request associated with the endpoint.  */
  xfer = get_control_transfer();

  /* Set the direction to OUT.  */
  xfer->phase = TransferPhase::DATA_OUT;

  auto descriptor_index = req.value & 0xff;
  auto descriptor_type =  (uint8_t) ((req.value >> 8) & 0xff);

  /* Default descriptor length is host length.  */
  //length =  host_length;

  /* What type of descriptor do we need to return?  */
  switch (descriptor_type)
  {

    case DeviceDescriptor::desc_type:
    case UX_DEVICE_QUALIFIER_DESCRIPTOR_ITEM:
      return send_device_descriptor(descriptor_type, req.index, req.length);

    case BOSDescriptor::desc_type:
    case UX_OTHER_SPEED_DESCRIPTOR_ITEM:
    case UX_CONFIGURATION_DESCRIPTOR_ITEM:
      return send_compound_descriptor(descriptor_type, descriptor_index, req.index, req.length);

    case UX_STRING_DESCRIPTOR_ITEM:

        /* We need to filter for the index 0 which is the language ID string.  */
        if (descriptor_index == 0)
        {

            /* We need to check request buffer size in case it's possible exceed. */
            if (lang_ids.get_buffer_len() + 2 > xfer->buffer_size)
            {
              get_control_endpoint()->stall();
              throw USBXX::runtime_error("Invalid language id framework length");
            }

            xfer->data[0] = (uint8_t)(lang_ids.get_buffer_len() + 2);
            xfer->data[1] =  UX_STRING_DESCRIPTOR_ITEM;

            /* Store the language ID into the buffer.  */
            ::memcpy(xfer->data+2, lang_ids.get_buffer(),
                lang_ids.get_buffer_len()); /* Use case of memcpy is verified. */

            auto len = std::min((uint32_t)req.length, (uint32_t)xfer->data[0]);

            /* We can return the string language ID descriptor.  */
            status = transfer_request(xfer, len, req.length);
        }
        else
        {
            /* The host wants a specific string index returned. Get the string framework pointer
               and length.  */
            string_framework = strings.get_buffer();
            string_framework_length = strings.get_buffer_len();

            /* We search through the string framework until we find the right index.
               The index is in the lower byte of the descriptor type. */
            while (string_framework_length != 0)
            {

                /* Ensure we have the correct language page.  */
                if (usb_get_short(string_framework) == req.index)
                {

                    /* Check the index.  */
                    if (*(string_framework + 2) == descriptor_index)
                    {

                        /* We need to check request buffer size in case it's possible exceed. */
                        if (((*(string_framework + 3)*2) + 2) > xfer->buffer_size)
                        {
                            get_control_endpoint()->stall();
                            throw USBXX::runtime_error("String request invalid");
                        }

                        /* We have a request to send back a string. Use the transfer request buffer.  */
                        string_memory =  xfer -> data;

                        /* Store the length in the string buffer. The length
                           of the string descriptor is stored in the third byte,
                           hence the ' + 3'. The encoding must be in 16-bit
                           unicode, hence the '*2'. The length includes the size
                           of the length itself as well as the descriptor type,
                           hence the ' + 2'.  */
                        *string_memory =  (uint8_t)((*(string_framework + 3)*2) + 2);

                        /* Store the Descriptor type. */
                        *(string_memory + 1) =  UX_STRING_DESCRIPTOR_ITEM;

                        /* Create the Unicode string.  */
                        for (string_length = 0; string_length <  *(string_framework + 3) ; string_length ++)
                        {

                            /* Insert a Unicode byte.  */
                            *(string_memory + 2 + (string_length * 2)) =  *(string_framework + 4 + string_length);

                            /* Insert a zero after the Unicode byte.  */
                            *(string_memory + 2 + (string_length * 2) + 1) =  0;
                        }

                        auto len = std::min((uint32_t)req.length, (uint32_t)((*(string_framework + 3)*2) + 2));

                        /* We can return the string descriptor.  */
                        status = transfer_request(xfer, len, req.length);
                        break;
                    }
                }

                /* This is the wrong string descriptor, jump to the next.  */
                string_framework_length -=  (uint32_t) *(string_framework + 3) + 4;
                string_framework +=  (uint32_t) *(string_framework + 3) + 4;
            }

            /* Have we exhausted all the string descriptors?  */
            if (string_framework_length == 0)
            {
                get_control_endpoint()->stall();
                return(UX_ERROR);
            }
        }
        break;

    default:
      get_control_endpoint()->stall();
      return(UX_ERROR);
    }

    /* Return the status to the caller.  */
    return(status);
}
