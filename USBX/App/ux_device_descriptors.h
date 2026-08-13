/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_descriptors.h
  * @author  MCD Application Team
  * @brief   USBX Device descriptor header file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UX_DEVICE_DESCRIPTORS_H__
#define __UX_DEVICE_DESCRIPTORS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "ux_api.h"
#include "ux_stm32_config.h"
#include "ux_device_class_cdc_acm.h"

uint8_t *USBD_Get_Device_Framework_Speed(uint8_t Speed, ULONG *Length);
uint8_t *USBD_Get_String_Framework(ULONG *Length);
uint8_t *USBD_Get_Language_Id_Framework(ULONG *Length);
uint16_t USBD_Get_Interface_Number(uint8_t class_type, uint8_t interface_type);
uint16_t USBD_Get_Configuration_Number(uint8_t class_type, uint8_t interface_type);


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


#ifdef __cplusplus
}
#endif
#endif  /* __UX_DEVICE_DESCRIPTORS_H__ */
