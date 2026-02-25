/*
 * usbxxdescriptor.cpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */

#include <threadxx/dbgstream.hpp>

#include <ux_api.h>

#include <usbxx/descriptor.hpp>

using namespace USBXX;

uint8_t DeviceInfo::add_string(const std::string &str)
{
  uint8_t retval = ++nstr;

  str_table += (uint8_t)(LANG_ID & 0xFF);
  str_table += (uint8_t)(LANG_ID >> 8);
  str_table += retval;

  str_table += (uint8_t)str.length();

  for (auto c : str) {
    str_table += (uint8_t)c;
  }

  dbg::dbgout << "Added string " << (int)retval << ": " << str << std::endl;

  return retval;
}

DeviceInfo::DeviceInfo(uint16_t VID, uint16_t PID,
    const std::string &mfg, const std::string &prod, const std::string &serial) : nstr(0)
{
  _next_endpoint_no = 1;

  _VID = VID;
  _PID = PID;
  _mfg = add_string(mfg);
  _prod = add_string(prod);
  _serial = add_string(serial);
}

ClassInfo &DeviceInfo::add_class(Descriptors::USB_Class cls, uint8_t subcls, uint8_t protocol)
{
  classes.emplace_back(this, cls, subcls, protocol);

  return classes.back();
};

EndpointInfo &ClassInfo::allocate_endpoint(bool inport)
{
  return dev->allocate_endpoint(this, inport);
}

std::u8string DeviceInfo::build(bool high_speed)
{
  builder b;

  auto dd = b.allocate<Descriptors::device>();


  dd->bcdUSB = 0x0200U;
  dd->bDeviceProtocol = 0x00;
  dd->bMaxPacketSize = 64U;
  dd->idVendor = _VID;
  dd->idProduct = _PID;
  dd->bcdDevice = 0x0200;
  dd->iManufacturer = _mfg;
  dd->iProduct = _prod;
  dd->iSerialNumber = _serial;
  // TODO: Compute the number of configurations
  dd->bNumConfigurations = 1; //USBD_MAX_NUM_CONFIGURATION;

  if (classes.size() > 1) {
    dd->bDeviceClass = Descriptors::COMPOSITE;
    dd->bDeviceSubClass = 0x02;
    dd->bDeviceProtocol = 0x01;
  } else {
    dd->bDeviceClass = classes[0].get_class();
    dd->bDeviceSubClass = classes[0].get_subclass();
    dd->bDeviceProtocol = classes[0].get_protocol();
  }

  if (high_speed) {
    auto dq = b.allocate<Descriptors::device_qualifiers>();

    dq->bcdDevice = 0x0200;
    dq->Class = 0x00;
    dq->SubClass = 0x00;
    dq->Protocol = 0x00;
    dq->bMaxPacketSize = 0x40;
    dq->bNumConfigurations = 0x01;
    dq->bReserved = 0x00;
  }

  if (classes.size() > 1)
    for (auto cls : classes) {
      output_class(cls);
    }
}


#if 0
  while (Idx_Instance < USBD_MAX_SUPPORTED_CLASS)
  {
    if ((pdev->classId < USBD_MAX_SUPPORTED_CLASS) &&
        (pdev->NumClasses < USBD_MAX_SUPPORTED_CLASS) &&
        (UserClassInstance[Idx_Instance] != CLASS_TYPE_NONE))
    {
      /* Call the composite class builder */
      (void)USBD_FrameWork_AddClass(pdev,
                                    (USBD_CompositeClassTypeDef)UserClassInstance[Idx_Instance],
                                    0, Speed,
                                    (pDevFrameWorkDesc + pdev->CurrDevDescSz));

      /* Increment the ClassId for the next occurrence */
      pdev->classId ++;
      pdev->NumClasses ++;
    }

    Idx_Instance++;
  }

  /* Check if there is a composite class and update device class */
  if (pdev->NumClasses > 1)
  {
    pDevDesc->bDeviceClass = 0xEF;
    pDevDesc->bDeviceSubClass = 0x02;
    pDevDesc->bDeviceProtocol = 0x01;
  }
  else
  {
    /* Check if the CDC ACM class is set and update device class */
    if (UserClassInstance[0] == CLASS_TYPE_CDC_ACM)
    {
      pDevDesc->bDeviceClass = 0x02;
      pDevDesc->bDeviceSubClass = 0x02;
      pDevDesc->bDeviceProtocol = 0x00;
    }
  }

  return pDevFrameWorkDesc;

#endif

void DeviceInfo::output_class(ClassInfo &cls)
{

}

