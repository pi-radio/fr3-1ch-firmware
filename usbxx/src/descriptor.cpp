/*
 * usbxxdescriptor.cpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */

#include <cassert>

#include <threadxx/dbgstream.hpp>

#include <ux_api.h>

#include <usbxx/descriptor.hpp>

using namespace USBXX;

extern "C" {
/* Includes ------------------------------------------------------------------*/
#include "ux_device_descriptors.h"
}

USBXX::Descriptor::Descriptor(int _speed) : speed(_speed)
{
  p_cur = desc;
}

typedef USBXX::EndpointDesc USBD_EPTypeDef;
typedef USBXX::CompositeElement USBD_CompositeElementTypeDef;
typedef USBXX::CompositeClass USBD_CompositeClassTypeDef;

/* USB Device descriptors structure */
struct DeviceDescriptor
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t iManufacturer;
  uint8_t iProduct;
  uint8_t iSerialNumber;
  uint8_t bNumConfigurations;
} __PACKED;

/* USB Iad descriptors structure */
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bFirstInterface;
  uint8_t bInterfaceCount;
  uint8_t bFunctionClass;
  uint8_t bFunctionSubClass;
  uint8_t bFunctionProtocol;
  uint8_t iFunction;
} __PACKED USBD_IadDescTypedef;

/* USB interface descriptors structure */
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
} __PACKED USBD_IfDescTypedef;

/* USB endpoint descriptors structure */
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
} __PACKED USBD_EpDescTypedef;

/* USB Config descriptors structure */
struct ConfigDesc
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wDescriptorLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t bMaxPower;
} __PACKED;

/* USB Qualifier descriptors structure */
struct DevQualiDesc
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdDevice;
  uint8_t Class;
  uint8_t SubClass;
  uint8_t Protocol;
  uint8_t bMaxPacketSize;
  uint8_t bNumConfigurations;
  uint8_t bReserved;
} __PACKED;

typedef struct
{
  /* Header Functional Descriptor*/
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint16_t bcdCDC;
} __PACKED USBD_CDCHeaderFuncDescTypedef;

typedef struct
{
  /* Call Management Functional Descriptor*/
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmCapabilities;
  uint8_t bDataInterface;
} __PACKED USBD_CDCCallMgmFuncDescTypedef;

typedef struct
{
  /* ACM Functional Descriptor*/
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmCapabilities;
} __PACKED USBD_CDCACMFuncDescTypedef;

typedef struct
{
  /* Union Functional Descriptor*/
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bMasterInterface;
  uint8_t bSlaveInterface;
} __PACKED USBD_CDCUnionFuncDescTypedef;


void USBXX::Descriptor::build()
{
  auto *pDevDesc = allocate_section<DeviceDescriptor>();

  pDevDesc->bDescriptorType = UX_DEVICE_DESCRIPTOR_ITEM;
  pDevDesc->bcdUSB = USB_BCDUSB;
  pDevDesc->bDeviceClass = 0x00;
  pDevDesc->bDeviceSubClass = 0x00;
  pDevDesc->bDeviceProtocol = 0x00;
  pDevDesc->bMaxPacketSize = USBD_MAX_EP0_SIZE;
  pDevDesc->idVendor = USBD_VID;
  pDevDesc->idProduct = USBD_PID;
  pDevDesc->bcdDevice = 0x0200;
  pDevDesc->iManufacturer = USBD_IDX_MFC_STR;
  pDevDesc->iProduct = USBD_IDX_PRODUCT_STR;
  pDevDesc->iSerialNumber = USBD_IDX_SERIAL_STR;
  pDevDesc->bNumConfigurations = USBD_MAX_NUM_CONFIGURATION;

  if (speed == USBD_HIGH_SPEED)
  {
    auto *pDevQualDesc = allocate_section<DevQualiDesc>();
    pDevQualDesc->bDescriptorType = UX_DEVICE_QUALIFIER_DESCRIPTOR_ITEM;
    pDevQualDesc->bcdDevice = 0x0200;
    pDevQualDesc->Class = 0x00;
    pDevQualDesc->SubClass = 0x00;
    pDevQualDesc->Protocol = 0x00;
    pDevQualDesc->bMaxPacketSize = 0x40;
    pDevQualDesc->bNumConfigurations = 0x01;
    pDevQualDesc->bReserved = 0x00;
  }

  if (usb_classes.size() > 1) {
    pDevDesc->bDeviceClass = 0xEF;
    pDevDesc->bDeviceSubClass = 0x02;
    pDevDesc->bDeviceProtocol = 0x01;
  } else {
    // Hard wiring to just get done
    assert(usb_classes[0] == CLASS_TYPE_CDC_ACM);
    pDevDesc->bDeviceClass = 0x02;
    pDevDesc->bDeviceSubClass = 0x02;
    pDevDesc->bDeviceProtocol = 0x00;
  }

  uint32_t config_start = get_desc_len();
  auto *config_desc = allocate_section<ConfigDesc>();

  config_desc->bLength = (uint8_t)sizeof(ConfigDesc);
  config_desc->bDescriptorType = USB_DESC_TYPE_CONFIGURATION;
  config_desc->wDescriptorLength = 0U;
  config_desc->bNumInterfaces = 0U;
  config_desc->bConfigurationValue = 1U;
  config_desc->iConfiguration = USBD_CONFIG_STR_DESC_IDX;
  config_desc->bmAttributes = USBD_CONFIG_BMATTRIBUTES;
  config_desc->bMaxPower = USBD_CONFIG_MAXPOWER;

  /* Build the device framework */
  for(auto cls : usb_classes)
  {
    add_class_to_conf(cls);
  }

  config_desc->bNumInterfaces = interfaces.size();
  config_desc->wDescriptorLength = get_desc_len() - config_start;
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
  auto *pEpDesc = allocate_section<USBD_EpDescTypedef>();

  pEpDesc->bDescriptorType    = USB_DESC_TYPE_ENDPOINT;
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
  auto *pIfDesc = allocate_section<USBD_IfDescTypedef>();
  pIfDesc->bLength = (uint8_t)sizeof(USBD_IfDescTypedef);
  pIfDesc->bDescriptorType = USB_DESC_TYPE_INTERFACE;
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

  auto pIadDesc = allocate_section<USBD_IadDescTypedef>();
  pIadDesc->bLength = (uint8_t)sizeof(USBD_IadDescTypedef);
  pIadDesc->bDescriptorType = USB_DESC_TYPE_IAD; /* IAD descriptor */
  pIadDesc->bFirstInterface = iface1;
  pIadDesc->bInterfaceCount = 2U;    /* 2 interfaces */
  pIadDesc->bFunctionClass = 0x02U;
  pIadDesc->bFunctionSubClass = 0x02U;
  pIadDesc->bFunctionProtocol = 0x01U;
  pIadDesc->iFunction = 0; /* String Index */
  /* USER CODE END Private_macro */

  add_interface_desc(iface1, 0U, 1U, 0x02, 0x02U, 0x01U, 0U);


  /* Header Functional Descriptor*/
  auto pHeadDesc = allocate_section<USBD_CDCHeaderFuncDescTypedef>();
  pHeadDesc->bLength = 0x05U;
  pHeadDesc->bDescriptorType = 0x24U;
  pHeadDesc->bDescriptorSubtype = 0x00U;
  pHeadDesc->bcdCDC = 0x0110;

  /* Call Management Functional Descriptor*/
  auto pCallMgmDesc = allocate_section<USBD_CDCCallMgmFuncDescTypedef>();
  pCallMgmDesc->bLength = 0x05U;
  pCallMgmDesc->bDescriptorType = 0x24U;
  pCallMgmDesc->bDescriptorSubtype = 0x01U;
  pCallMgmDesc->bmCapabilities = 0x00U;
  pCallMgmDesc->bDataInterface = iface2;

  /* ACM Functional Descriptor*/
  auto pACMDesc = allocate_section<USBD_CDCACMFuncDescTypedef>();
  pACMDesc->bLength = 0x04U;
  pACMDesc->bDescriptorType = 0x24U;
  pACMDesc->bDescriptorSubtype = 0x02U;
  pACMDesc->bmCapabilities = 0x02;

  /* Union Functional Descriptor*/
  auto pUnionDesc = allocate_section<USBD_CDCUnionFuncDescTypedef>();
  pUnionDesc->bLength = 0x05U;
  pUnionDesc->bDescriptorType = 0x24U;
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

  throw std::runtime_error("Unable to find requested interface");
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

