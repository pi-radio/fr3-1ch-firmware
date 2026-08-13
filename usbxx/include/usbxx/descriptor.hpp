/*
 * usbxxdescriptor.hpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */

#ifndef USBXX_USBXXDESCRIPTOR_HPP_
#define USBXX_USBXXDESCRIPTOR_HPP_

#include <stdint.h>

#include <cstring>
#include <vector>
#include <string>
#include <array>
#include <map>

#define USBD_MAX_NUM_CONFIGURATION                     1U
#define USBD_MAX_SUPPORTED_CLASS                       3U
#define USBD_MAX_CLASS_ENDPOINTS                       9U
#define USBD_MAX_CLASS_INTERFACES                      11U

#define USBD_CDC_ACM_CLASS_ACTIVATED                   1U

#define USBD_CONFIG_MAXPOWER                           25U
#define USBD_COMPOSITE_USE_IAD                         1U
#define USBD_DEVICE_FRAMEWORK_BUILDER_ENABLED          1U

#define USBD_FRAMEWORK_MAX_DESC_SZ                     200U



namespace USBXX
{
  static const uint8_t USBD_FULL_SPEED = 0x00U;
  static const uint8_t USBD_HIGH_SPEED = 0x01U;

  enum CompositeClass
  {
    CLASS_TYPE_NONE     = 0,
    CLASS_TYPE_HID      = 1,
    CLASS_TYPE_CDC_ACM  = 2,
    CLASS_TYPE_MSC      = 3,
    CLASS_TYPE_CDC_ECM  = 4,
    CLASS_TYPE_DFU      = 5,
    CLASS_TYPE_PIMA_MTP = 6,
    CLASS_TYPE_RNDIS    = 7,
    CLASS_TYPE_VIDEO    = 8,
    CLASS_TYPE_CCID     = 9,
    CLASS_TYPE_PRINTER  = 10,
  } ;

  /* USB endpoint handle structure */
  struct EndpointDesc
  {
    uint8_t add;
    uint8_t type;
    uint16_t size;
    uint8_t is_used;
  } ;

  /* USB Composite handle structure */
  struct CompositeElement
  {
    CompositeClass ClassType;
    uint32_t ClassId;
    uint8_t InterfaceType;
    uint32_t Active;
    uint32_t NumEps;
    uint32_t NumIf;
    EndpointDesc Eps[USBD_MAX_CLASS_ENDPOINTS];
    uint8_t Ifs[USBD_MAX_CLASS_INTERFACES];
  };

#define USBD_LANGID_STRING                            1033

  class Strings
  {
    static const size_t max_len = 256;

    uint8_t str[max_len] __attribute__ ((aligned (4)));
    uint32_t pos;

  public:
    Strings() : pos(0) {}

    void add_string(uint8_t idx, const std::string &s, uint16_t lang_id = USBD_LANGID_STRING);

    uint8_t *get_buffer() { return str; }
    size_t get_buffer_len() { return pos; }
  };

  class LanguageIDs
  {
    static const size_t max_len = 256;

    uint8_t buf[max_len] __attribute__ ((aligned (4)));
    uint32_t pos;

  public:
    LanguageIDs() : pos(0) {}

    void add_language(uint16_t lang_id = USBD_LANGID_STRING) {
      buf[pos++] = lang_id & 0xFF;
      buf[pos++] = lang_id >> 8;
    }

    uint8_t *get_buffer() { return buf; }
    size_t get_buffer_len() { return pos; }
  };

  class Descriptor
  {
    class cls_info;
    class ep_info;

    struct ep_info
    {
      uint8_t cls;
      uint8_t addr;
      uint8_t type;
      uint16_t size;
    };

    struct if_info
    {
      uint8_t cls;
      uint8_t type;
    };

    struct cls_info
    {
      std::vector<uint8_t> endpoints;
      std::vector<uint8_t> interfaces;
    };

    std::string mfg;
    std::string prod;
    std::string serial;

    int speed;

    std::vector<uint8_t> usb_classes;

    uint8_t desc[USBD_FRAMEWORK_MAX_DESC_SZ] __attribute__ ((aligned (4))) = {0};

    uint8_t *p_cur;

    void add_class_to_conf(uint8_t);

    std::map<uint8_t, ep_info> endpoints;
    std::map<uint8_t, if_info> interfaces;
    std::map<uint8_t, cls_info> classes;

    uint8_t assign_endpoint(uint8_t cls, uint8_t addr, uint8_t type, uint16_t size);

    void add_endpoint_desc(uint8_t epaddr, uint8_t interval);

    void add_interface_desc(uint8_t ifnum,
        uint8_t alt,
        uint8_t eps,
        uint8_t cls,
        uint8_t subclass,
        uint8_t protocol,
        uint8_t istring);


  public:
    Descriptor(int _speed);

    template <typename T>
    T *allocate_section() {
      T *retval = (T *)p_cur;
      p_cur += sizeof(T);

      retval->bLength = sizeof(T);

      return retval;
    }

    uint8_t allocate_interface(uint8_t cls);

    void add_class(uint8_t cls) { usb_classes.push_back(cls); }

    void set_manufacturer(const std::string &_mfg) { mfg = _mfg; }
    void set_product(const std::string &_prod) { prod = _prod; }
    void set_serial(const std::string &_serial) { serial = _serial; }

    std::string get_manufacturer() { return mfg; }
    std::string get_product() { return prod; }
    std::string get_serial() { return serial; }

    void build();

    uint8_t  *get_desc() { return desc; }
    size_t get_desc_len() { return p_cur - desc; /* dev_class.CurrDevDescSz + dev_class.CurrConfDescSz; */ }

    uint16_t get_interface_number(uint8_t class_type, uint8_t interface_type);

    bool is_hs() { return speed == USBD_HIGH_SPEED; }
  };


};

#endif /* USBXX_USBXXDESCRIPTOR_HPP_ */
