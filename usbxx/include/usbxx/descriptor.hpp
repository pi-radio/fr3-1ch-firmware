/*
 * usbxxdescriptor.hpp
 *
 *  Created on: Feb 20, 2026
 *      Author: zapman
 */

#ifndef USBXX_USBXXDESCRIPTOR_HPP_
#define USBXX_USBXXDESCRIPTOR_HPP_

#include <stdint.h>

#include <vector>
#include <string>

#if 0

#define USBD_MAX_NUM_CONFIGURATION                     1U
#define USBD_MAX_SUPPORTED_CLASS                       3U
#define USBD_MAX_CLASS_ENDPOINTS                       9U
#define USBD_MAX_CLASS_INTERFACES                      11U

#define USBD_CDC_ACM_CLASS_ACTIVATED                   1U

#define USBD_CONFIG_MAXPOWER                           25U
#define USBD_COMPOSITE_USE_IAD                         1U
#define USBD_DEVICE_FRAMEWORK_BUILDER_ENABLED          1U

#define USBD_FRAMEWORK_MAX_DESC_SZ                     200U
/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */
/* Enum Class Type */

struct USBXXEP
{
  uint8_t add;
  uint8_t type;
  uint16_t size;
  uint8_t is_used;
};


typedef enum
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
} USBD_CompositeClassTypeDef;

/* USB Endpoint handle structure */
typedef struct
{
  uint32_t status;
  uint32_t total_length;
  uint32_t rem_length;
  uint32_t maxpacket;
  uint16_t is_used;
  uint16_t bInterval;
} USBD_EndpointTypeDef;

/* USB endpoint handle structure */
typedef struct
{
  uint8_t add;
  uint8_t type;
  uint16_t size;
  uint8_t is_used;
} USBD_EPTypeDef;

/* USB Device handle structure */
typedef struct _USBD_DevClassHandleTypeDef
{
  uint8_t Speed;
  uint32_t classId;
  uint32_t NumClasses;
  USBD_CompositeElementTypeDef tclasslist[USBD_MAX_SUPPORTED_CLASS];
  uint32_t CurrDevDescSz;
  uint32_t CurrConfDescSz;
} USBD_DevClassHandleTypeDef;

/* USB Device endpoint direction */
typedef enum
{
  OUT   = 0x00,
  IN    = 0x80,
} USBD_EPDirectionTypeDef;

/* USB Device descriptors structure */
typedef struct
{
} __PACKED USBD_DeviceDescTypedef;

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
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wDescriptorLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t bMaxPower;
} __PACKED USBD_ConfigDescTypedef;

/* USB Qualifier descriptors structure */
typedef struct
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
} __PACKED USBD_DevQualiDescTypedef;

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



/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

uint8_t *USBD_Get_Device_Framework_Speed(uint8_t Speed, ULONG *Length);
uint8_t *USBD_Get_String_Framework(ULONG *Length);
uint8_t *USBD_Get_Language_Id_Framework(ULONG *Length);
uint16_t USBD_Get_Interface_Number(uint8_t class_type, uint8_t interface_type);
uint16_t USBD_Get_Configuration_Number(uint8_t class_type, uint8_t interface_type);

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private_defines */

/* USER CODE END Private_defines */

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

#define USBD_FULL_SPEED                               0x00U
#define USBD_HIGH_SPEED                               0x01U

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

/* Private macro -----------------------------------------------------------*/
/* USER CODE BEGIN Private_macro */

/* USER CODE END Private_macro */
#define __USBD_FRAMEWORK_SET_EP(epadd, eptype, epsize, HSinterval, FSinterval) do { \
                                /* Append Endpoint descriptor to Configuration descriptor */ \
                                pEpDesc = ((USBD_EpDescTypedef*)((uint32_t)pConf + *Sze)); \
                                pEpDesc->bLength            = (uint8_t)sizeof(USBD_EpDescTypedef); \
                                pEpDesc->bDescriptorType    = USB_DESC_TYPE_ENDPOINT; \
                                pEpDesc->bEndpointAddress   = (epadd); \
                                pEpDesc->bmAttributes       = (eptype); \
                                pEpDesc->wMaxPacketSize     = (epsize); \
                                if(pdev->Speed == USBD_HIGH_SPEED) \
                                { \
                                  pEpDesc->bInterval        = (HSinterval); \
                                } \
                                else \
                                { \
                                  pEpDesc->bInterval        = (FSinterval); \
                                } \
                                *Sze += (uint32_t)sizeof(USBD_EpDescTypedef); \
                              } while(0)

#define __USBD_FRAMEWORK_SET_IF(ifnum, alt, eps, class, subclass, protocol, istring) do {\
                                /* Interface Descriptor */ \
                                pIfDesc = ((USBD_IfDescTypedef*)((uint32_t)pConf + *Sze)); \
                                pIfDesc->bLength = (uint8_t)sizeof(USBD_IfDescTypedef); \
                                pIfDesc->bDescriptorType = USB_DESC_TYPE_INTERFACE; \
                                pIfDesc->bInterfaceNumber = (ifnum); \
                                pIfDesc->bAlternateSetting = (alt); \
                                pIfDesc->bNumEndpoints = (eps); \
                                pIfDesc->bInterfaceClass = (class); \
                                pIfDesc->bInterfaceSubClass = (subclass); \
                                pIfDesc->bInterfaceProtocol = (protocol); \
                                pIfDesc->iInterface = (istring); \
                                *Sze += (uint32_t)sizeof(USBD_IfDescTypedef); \
                              } while(0)

#endif

namespace USBXX
{
  namespace Descriptors
  {
    enum USB_Class {
      None = 0,
      HID = 1,
      CDC_ACM = 2,
      MSC = 3,
      CDC_ECM = 4,
      DFU = 5,
      PIMA_MTP = 6,
      RNDIS = 7,
      VIDEO = 8,
      CCID = 9,
      PRINTER = 10,
      COMPOSITE = 0xEF
    };

    struct device
    {
      static constexpr uint8_t TYPE_ID = 1;

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
    } __attribute__(( packed ));

    struct device_qualifiers
    {
      static constexpr uint8_t TYPE_ID = 6;

      uint8_t bLength;
      uint8_t bDescriptorType;
      uint16_t bcdDevice;
      uint8_t Class;
      uint8_t SubClass;
      uint8_t Protocol;
      uint8_t bMaxPacketSize;
      uint8_t bNumConfigurations;
      uint8_t bReserved;
    } __attribute__(( packed ));

    struct config
    {
      uint8_t bLength;
      uint8_t bDescriptorType;
      uint16_t wDescriptorLength;
      uint8_t bNumInterfaces;
      uint8_t bConfigurationValue;
      uint8_t iConfiguration;
      uint8_t bmAttributes;
      uint8_t bMaxPower;
    } __attribute__(( packed ));
  };

  class DeviceInfo;
  class ClassInfo;
  class EndpointInfo;

  class ClassInfo
  {
  protected:
    DeviceInfo *dev;
    Descriptors::USB_Class cls;
    uint8_t subcls;
    uint8_t protocol;

    std::vector<EndpointInfo> eps;

  public:
    ClassInfo(DeviceInfo *_dev,
        Descriptors::USB_Class _cls,
        uint8_t _subcls,
        uint8_t _protocol) :
      dev(_dev),
      cls(_cls),
      subcls(_subcls),
      protocol(_protocol) { }

    Descriptors::USB_Class get_class() { return cls; }
    uint8_t get_subclass() { return subcls; }
    uint8_t get_protocol() { return protocol; }

    EndpointInfo &allocate_endpoint(bool inport);

    virtual void add_config() {}
  };

  class Class_CDCACM : public ClassInfo
  {
  public:
    Class_CDCACM(DeviceInfo *_dev);
  };


  class EndpointInfo
  {
    ClassInfo *cls;
    bool inport;
    uint8_t epno;

  public:
    EndpointInfo(ClassInfo *_cls,
                  bool _inport,
                  uint8_t _epno) : cls(_cls),
                                   inport(_inport),
                                   epno(_epno) { }

    uint8_t get_endpoint_no() {
      if (inport)
        return 0x80 | epno;
      else
        return epno;
    }
  };

  class DeviceInfo
  {
    friend class ClassInfo;
    friend class EndpointInfo;

  public:
    static constexpr uint16_t LANG_ID = 1033;
    static constexpr std::u8string LANG_ID_STR { LANG_ID & 0xFF, LANG_ID >> 8 };

    struct EP
    {
      uint8_t add;
      uint8_t type;
      uint16_t size;
      uint8_t is_used;
    };

    struct composite_element
    {
      uint32_t id;
      uint8_t iface;
      uint32_t active;
      EP eps[9];
      uint8_t interfaces[11];
    };


  protected:
    struct builder {
      std::vector<uint8_t> ser;

      uint8_t *allocate(size_t n) {
        auto sz = ser.size();
        if (ser.capacity() < sz + n) {
          ser.reserve(sz + n + 128);
        }
        uint8_t *retval = ser.data() + n;
        ser.resize(ser.size() + n);
        return retval;
      }

      template <typename T>
      T *allocate()
      {
        T *retval = (T *)allocate(sizeof(T));
        retval->bLength = sizeof(T);
        retval->bDescriptorType = T::TYPE_ID;
        return retval;
      }

      template <typename T>
      T *get_at(size_t i)
      {
        return (T *)(ser.data() + i);
      }

    };

    uint8_t nstr;
    std::u8string str_table;

    uint8_t add_string(const std::string &str);

    uint16_t _VID;
    uint16_t _PID;

    uint8_t _mfg;
    uint8_t _prod;
    uint8_t _serial;

    uint8_t _next_endpoint_no;

    EndpointInfo &allocate_endpoint(ClassInfo *cls, bool inport)
    {
      endpoints.emplace_back(cls, inport, _next_endpoint_no++);
      return endpoints.back();
    }

    std::vector<ClassInfo> classes;
    std::vector<EndpointInfo> endpoints;

    void output_class(ClassInfo &cls);

  public:
    DeviceInfo(uint16_t VID, uint16_t PID,
        const std::string &mfg, const std::string &prod, const std::string &serial);

    std::u8string build(bool high_speed = false);

    ClassInfo &add_class(Descriptors::USB_Class cls, uint8_t subcls, uint8_t protocol);
  };

};

#endif /* USBXX_USBXXDESCRIPTOR_HPP_ */
