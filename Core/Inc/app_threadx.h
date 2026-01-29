/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.h
  * @author  MCD Application Team
  * @brief   ThreadX applicative header file
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
#ifndef __APP_THREADX_H
#define __APP_THREADX_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */
#define USB_AVAILABLE_FLAG    (1 << 0)
#define DTR_HIGH_FLAG         (1 << 1)
#define RTS_HIGH_FLAG         (1 << 2)
#define REFRESH_FLAG          (1 << 3)
#define TERM_FLUSH_FLAG       (1 << 4)
/* USER CODE END PD */

/* Main thread defines -------------------------------------------------------*/
/* USER CODE BEGIN MTD */

/* USER CODE END MTD */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT App_ThreadX_Init(VOID *memory_ptr);
void MX_ThreadX_Init(void);

/* USER CODE BEGIN EFP */
extern void usb_connect(void);
extern void usb_disconnect(void);
extern void wait_usb(void);


extern void dtr_high(void);
extern void dtr_low(void);
extern void wait_dtr(void);

extern void rts_high(void);
extern void rts_low(void);
extern void wait_rts(void);



/* USER CODE END EFP */

/* USER CODE BEGIN 1 */
extern TX_EVENT_FLAGS_GROUP app_events;

extern int rx_cnt;

static inline int dtr_is_set(void)
{
  ULONG flags;

  tx_event_flags_get(&app_events, DTR_HIGH_FLAG, TX_AND, &flags, TX_NO_WAIT);

  return (flags) ? 1 : 0;
}

static inline int rts_is_set(void)
{
  ULONG flags;

  tx_event_flags_get(&app_events, RTS_HIGH_FLAG, TX_AND, &flags, TX_NO_WAIT);

  return (flags) ? 1 : 0;
}

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_THREADX_H */
