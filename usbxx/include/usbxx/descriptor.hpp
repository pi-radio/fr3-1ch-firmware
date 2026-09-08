/*
 * usbxxdescriptor.hpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */

#ifndef USBXX_USBXXDESCRIPTOR_HPP_
#define USBXX_USBXXDESCRIPTOR_HPP_

#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <iterator>

#include <usbxx/endian.hpp>

#define USBD_MAX_NUM_CONFIGURATION                     1U
#define USBD_MAX_SUPPORTED_CLASS                       3U


#define USBD_CDC_ACM_CLASS_ACTIVATED                   1U

#define USBD_CONFIG_MAXPOWER                           25U
#define USBD_COMPOSITE_USE_IAD                         1U
#define USBD_DEVICE_FRAMEWORK_BUILDER_ENABLED          1U

#define USBD_FRAMEWORK_MAX_DESC_SZ                     200U



namespace USBXX
{
  static const uint8_t USBD_FULL_SPEED = 0x00U;
  static const uint8_t USBD_HIGH_SPEED = 0x01U;
  static const uint16_t  USBD_LANGID_STRING = 1033U;


  static constexpr auto configuration_descriptor_structure = std::to_array({1,1,2,1,1,1,1,1});
  static constexpr auto interface_association_descriptor_structure = std::to_array({1,1,1,1,1,1,1,1});
  static constexpr auto string_descriptor_structure = std::to_array({1,1,2});
  static constexpr auto dfu_functional_descriptor_structure = std::to_array({1,1,1,2,2,2});
  static constexpr auto class_audio_interface_descriptor_structure = std::to_array({1,1,1,1,1,1,1,1});
  static constexpr auto class_audio_input_terminal_descriptor_structure = std::to_array({1,1,1,1,2,1,1,2,1,1});
  static constexpr auto class_audio_output_terminal_descriptor_structure = std::to_array({1,1,1,1,2,1,1,1});
  static constexpr auto class_audio_feature_unit_descriptor_structure = std::to_array({1,1,1,1,1,1,1});
  static constexpr auto class_audio_streaming_interface_descriptor_structure = std::to_array({1,1,1,1,1,1});
  static constexpr auto class_audio_streaming_endpoint_descriptor_structure = std::to_array({1,1,1,1,1,1});
  static constexpr auto hub_descriptor_structure = std::to_array({1,1,1,2,1,1,1,1});
  static constexpr auto hid_descriptor_structure = std::to_array({1,1,2,1,1,1,2});
  static constexpr auto class_pima_storage_structure = std::to_array({2,2,2,4,4,4,4,4});
  static constexpr auto class_pima_object_structure = std::to_array({4,2,2,4,2,4,4,4,4,4,4,4,2,4,4});
  static constexpr auto ecm_interface_descriptor_structure = std::to_array({1,1,1,1,4,2,2,1});

  static constexpr auto usb_2_0_extension_descriptor_structure = std::to_array({1,1,1,4});
  static constexpr auto container_id_descriptor_structure = std::to_array({1,1,1,1,4,4,4,4});


#define UX_CONFIGURATION_DESCRIPTOR_ITEM                                2u
#define UX_STRING_DESCRIPTOR_ITEM                                       3u
#define UX_INTERFACE_DESCRIPTOR_ITEM                                    4u
#define UX_ENDPOINT_DESCRIPTOR_ITEM                                     5u
#define UX_DEVICE_QUALIFIER_DESCRIPTOR_ITEM                             6u
#define UX_OTHER_SPEED_DESCRIPTOR_ITEM                                  7u
#define UX_OTG_DESCRIPTOR_ITEM                                          9u
#define InterfaceAssociationDescriptor_ITEM                        11u
#define UX_DEVICE_CAPABILITY_DESCRIPTOR_ITEM                            16u
#define UX_DFU_FUNCTIONAL_DESCRIPTOR_ITEM                               0x21u
#define UX_HUB_DESCRIPTOR_ITEM                                          0x29u

#define DECLARE_DESCRIPTOR(x)   struct __attribute__((packed)) x

  DECLARE_DESCRIPTOR(DeviceDescriptor)
  {
    static constexpr uint8_t desc_type = 1u;
    static constexpr auto structure = std::to_array({1,1,2,1,1,1,1,2,2,2,1,1,1,1});
    static constexpr auto length = 18;

    uint8_t           bLength;
    uint8_t           bDescriptorType;
    uint16_t          bcdUSB;
    uint8_t           bDeviceClass;
    uint8_t           bDeviceSubClass;
    uint8_t           bDeviceProtocol;
    uint8_t           bMaxPacketSize0;
    uint16_t          idVendor;
    uint16_t          idProduct;
    uint16_t          bcdDevice;
    uint8_t           iManufacturer;
    uint8_t           iProduct;
    uint8_t           iSerialNumber;
    uint8_t           bNumConfigurations;
  };

  DECLARE_DESCRIPTOR(ConfigurationDescriptor)
  {
    static constexpr uint8_t desc_type = 2u;
    static constexpr auto structure = std::to_array({1,1,2,1,1,1,1,1});
    static constexpr uint32_t length = 9;

    uint8_t           bLength;
    uint8_t           bDescriptorType;
    uint16_t          wTotalLength;
    uint8_t           bNumInterfaces;
    uint8_t           bConfigurationValue;
    uint8_t           iConfiguration;
    uint8_t           bmAttributes;
    uint8_t           MaxPower;
  };


  /* Define USBX Interface Descriptor structure.  */

  DECLARE_DESCRIPTOR(InterfaceDescriptor)
  {
    static constexpr uint8_t desc_type = 4u;
    static constexpr auto structure = std::to_array({1,1,1,1,1,1,1,1,1});
    static constexpr auto length = 9;

    uint8_t           bLength;
    uint8_t           bDescriptorType;
    uint8_t           bInterfaceNumber;
    uint8_t           bAlternateSetting;
    uint8_t           bNumEndpoints;
    uint8_t           bInterfaceClass;
    uint8_t           bInterfaceSubClass;
    uint8_t           bInterfaceProtocol;
    uint8_t           iInterface;
    uint8_t           _align_size[3];
  };

  DECLARE_DESCRIPTOR(EndpointDescriptor)
  {
    static constexpr uint8_t desc_type = 5u;
    static constexpr auto structure = std::to_array({1,1,1,1,2,1});
    static constexpr uint8_t length = 7;

    uint8_t           bLength;
    uint8_t           bDescriptorType;
    uint8_t           bEndpointAddress;
    uint8_t           bmAttributes;
    uint16_t          wMaxPacketSize;
    uint8_t           bInterval;
    uint8_t           _align_size[1];

    EndpointDescriptor(uint8_t addr=0, uint8_t attributes=0,
        uint16_t packet_size=0, uint8_t interval=0) :
      bLength(length),
      bDescriptorType(desc_type),
      bEndpointAddress(addr),
      bmAttributes(attributes),
      wMaxPacketSize(packet_size),
      bInterval(interval), _align_size({0}) {
    }
  };





  DECLARE_DESCRIPTOR(BOSDescriptor)
  {
    static constexpr uint8_t desc_type = 15u;
    static constexpr auto structure = std::to_array({1,1,2,1});
    static constexpr uint8_t length = 5;

    uint8_t           bLength;
    uint8_t           bDescriptorType;
    uint16_t          wTotalLength;
    uint8_t           bNumDeviceCaps;
    uint8_t           _align_size[3];
  };

  DECLARE_DESCRIPTOR(DeviceQualifierDescriptor)
  {
    static constexpr uint8_t desc_type = 6u;
    static constexpr auto structure = std::to_array({1,1,2,1,1,1,1,1,1,1,1});
    static constexpr uint8_t length = 10;

    uint8_t           bLength;
    uint8_t           bDescriptorType;
    uint16_t          bcdUSB;
    uint8_t           bDeviceClass;
    uint8_t           bDeviceSubClass;
    uint8_t           bDeviceProtocol;
    uint8_t           bMaxPacketSize0;
    uint8_t           bNumConfigurations;
    uint8_t           bReserved;
    uint8_t           _align_size[2];
  };

  DECLARE_DESCRIPTOR(InterfaceAssociationDescriptor)
  {
    static constexpr uint8_t desc_type = 11;
    static constexpr auto structure = std::to_array({1,1,1,1,1,1,1,1});
    static constexpr uint8_t length = 8;

    uint8_t           bLength;
    uint8_t           bDescriptorType;
    uint8_t           bFirstInterface;
    uint8_t           bInterfaceCount;
    uint8_t           bFunctionClass;
    uint8_t           bFunctionSubClass;
    uint8_t           bFunctionProtocol;
    uint8_t           iFunction;
  } ;

  // Move to CDCACM
  DECLARE_DESCRIPTOR(CDCFunctionHeaderDescriptor)
  {
    static constexpr uint8_t desc_type = 0x24;
    static constexpr auto structure = std::to_array({1,1,1,2});
    static constexpr auto length = 5;

    /* Header Functional Descriptor*/
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdCDC;
  };

  DECLARE_DESCRIPTOR(CDCCallManagementFunctionDescriptor)
  {
    static constexpr uint8_t desc_type = 0x24;
    static constexpr auto structure = std::to_array({1,1,1,1,1});
    static constexpr auto length = 5;

    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmCapabilities;
    uint8_t bDataInterface;
  };

  DECLARE_DESCRIPTOR(CDCACMFunctionDescriptor)
  {
    static constexpr uint8_t desc_type = 0x24;
    static constexpr auto structure = std::to_array({1,1,1,1});
    static constexpr auto length = 4;

    /* ACM Functional Descriptor*/
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmCapabilities;
  };

  DECLARE_DESCRIPTOR(CDCUnionFunctionDescriptor)
  {
    static constexpr uint8_t desc_type = 0x24;
    static constexpr auto structure = std::to_array({1,1,1,1,1});
    static constexpr auto length = 5;

    /* Union Functional Descriptor*/
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bMasterInterface;
    uint8_t bSlaveInterface;
  };


#if 0
  #define InterfaceAssociationDescriptor_ENTRIES         8

  #define UX_DEVICE_QUALIFIER_DESCRIPTOR_ENTRIES                          9

  #define UX_BOS_DESCRIPTOR_ENTRIES                                       4

  #define UX_INTERFACE_DESCRIPTOR_ENTRIES                                 9


  #define UX_CONFIGURATION_DESCRIPTOR_ENTRIES                             8

#define UX_DEVICE_DESCRIPTOR_ENTRIES                                    14u
#endif

  template <typename T, size_t N>
  static void parse_descriptor_in(const std::array<T, N> &descriptor_structure, const uint8_t *raw_descriptor, uint8_t *out)
  {
    for (auto l : descriptor_structure) {
      switch(l)
      {
      case 4:
        while((uintptr_t) out & 3u)
          *out++ =  0;
        *((uint32_t *) out) =  usb_get_long(raw_descriptor);
        raw_descriptor +=  4;
        out += 4;
        break;

      case 2:
        while((uintptr_t) out & 1u)
            *out++ =  0;
        *((uint16_t *) out) = usb_get_short(raw_descriptor);
        raw_descriptor += 2;
        out += 2;
        break;

      default:
        *((uint8_t *) out) =  (uint8_t)*raw_descriptor;
        raw_descriptor++;
        out ++;
      }
    }
  }

  template <typename T>
  T read_in_descriptor(const uint8_t *buffer)
  {
    T retval;

    if (buffer[1] != T::desc_type) {
      throw std::runtime_error("Invalid descriptor type");
    }

    parse_descriptor_in(T::structure, buffer, (uint8_t *)&retval);

    return retval;
  }

  struct DescriptorBuffer
  {
    uint32_t len;
    const uint8_t *buffer;

    DescriptorBuffer(uint32_t _len = 0, const uint8_t *_buffer = nullptr) :
      len(_len), buffer(_buffer)
    {
    }

    uint32_t cur_len() {
      if (buffer == nullptr) {
        return 0;
      }

      return *buffer;
    }

    uint8_t cur_type() {
      if (buffer == nullptr) {
        return -1;
      }

      return *(buffer + 1);
    }

    template <typename T, size_t N, typename T2>
    void parse_descriptor(const std::array<T, N> &descriptor_structure, T2 *out)
    {
      parse_descriptor_in(descriptor_structure, buffer, (uint8_t *)out);
    }

    template <typename T>
    T read_in()
    {
      return read_in_descriptor<T>(buffer);
    }
  };

  class DescriptorIterator
  {
    DescriptorBuffer buf;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = DescriptorBuffer;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    DescriptorIterator(uint32_t _len, const uint8_t *_buffer) : buf(_len, _buffer) {
    }

    value_type operator*() {
      return buf;
    }

    value_type *operator->() { return &buf; }

    DescriptorIterator &operator++() {
      uint32_t cur_len = buf.cur_len();

      if (cur_len >= buf.len) {
        buf.len = 0;
        buf.buffer = nullptr;
        return *this;
      }

      buf.buffer += cur_len;
      buf.len -= cur_len;

      assert(type() != 0);

      return *this;
    }



    bool operator==(const DescriptorIterator &other) {
      return (buf.buffer == other.buf.buffer);
    }

    bool operator!=(const DescriptorIterator &other) {
      return (buf.buffer != other.buf.buffer);
    }

    uint8_t type() {
      return buf.buffer[1];
    }

    void trim(uint32_t new_len) {
      if (new_len > buf.len) {
        throw std::runtime_error("Trim value larger than original buffer");
      }

      buf.len = new_len;
    }

    static DescriptorIterator end() {
      return DescriptorIterator(0, nullptr);
    }
  };

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
      p_cur += T::length;

      retval->bLength = T::length;
      retval->bDescriptorType = T::desc_type;

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

    const uint8_t *get_desc() const { return desc; }
    size_t get_desc_len() const { return p_cur - desc; /* dev_class.CurrDevDescSz + dev_class.CurrConfDescSz; */ }

    uint16_t get_interface_number(uint8_t class_type, uint8_t interface_type);

    bool is_hs() { return speed == USBD_HIGH_SPEED; }

    DescriptorIterator begin() const { return DescriptorIterator(get_desc_len(), get_desc()); }
    DescriptorIterator end() const { return DescriptorIterator(0, nullptr); }
  };


};

#endif /* USBXX_USBXXDESCRIPTOR_HPP_ */
