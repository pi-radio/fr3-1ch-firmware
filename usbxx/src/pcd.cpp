extern "C" {
/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
}

#include <stdexcept>

class PCD
{
  PCD_HandleTypeDef hpcd;
  USB_DRD_TypeDef *USBx;

public:
  PCD() {
    USBx = USB_DRD_FS;

    hpcd.Instance = USBx;
    hpcd.Init.dev_endpoints = 8;
    hpcd.Init.speed = USBD_FS_SPEED;
    hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd.Init.Sof_enable = DISABLE;
    hpcd.Init.low_power_enable = DISABLE;
    hpcd.Init.lpm_enable = DISABLE;
    hpcd.Init.battery_charging_enable = DISABLE;
    hpcd.Init.vbus_sensing_enable = DISABLE;
    hpcd.Init.bulk_doublebuffer_enable = DISABLE;
    hpcd.Init.iso_singlebuffer_enable = DISABLE;
  }

  void init() {
    uint8_t i;

    if (hpcd.State == HAL_PCD_STATE_RESET)
      /* Allocate lock resource and initialize it */
      hpcd.Lock = HAL_UNLOCKED;

    hpcd.State = HAL_PCD_STATE_BUSY;

    /* Disable the Interrupts */
    __HAL_PCD_DISABLE(&hpcd);

    /* Reset after a PHY select */
    reset();

    /* Clear pending interrupts */
    USBx->ISTR = 0U;

    /* Force Device Mode */
    if (USB_SetCurrentMode(hpcd.Instance, USB_DEVICE_MODE) != HAL_OK)
    {
      hpcd.State = HAL_PCD_STATE_ERROR;
      throw std::runtime_error("Unable to set USB mode");
    }

    /* Init endpoints structures */
    for (i = 0U; i < hpcd.Init.dev_endpoints; i++)
    {
      /* Init ep structure */
      hpcd.IN_ep[i].is_in = 1U;
      hpcd.IN_ep[i].num = i;
      /* Control until ep is activated */
      hpcd.IN_ep[i].type = EP_TYPE_CTRL;
      hpcd.IN_ep[i].maxpacket = 0U;
      hpcd.IN_ep[i].xfer_buff = 0U;
      hpcd.IN_ep[i].xfer_len = 0U;
    }

    for (i = 0U; i < hpcd.Init.dev_endpoints; i++)
    {
      hpcd.OUT_ep[i].is_in = 0U;
      hpcd.OUT_ep[i].num = i;
      /* Control until ep is activated */
      hpcd.OUT_ep[i].type = EP_TYPE_CTRL;
      hpcd.OUT_ep[i].maxpacket = 0U;
      hpcd.OUT_ep[i].xfer_buff = 0U;
      hpcd.OUT_ep[i].xfer_len = 0U;
    }

    /* Init Device */
    if (USB_DevInit(hpcd.Instance, hpcd.Init) != HAL_OK)
    {
      hpcd.State = HAL_PCD_STATE_ERROR;
      throw std::runtime_error("Unable to initialize USB device");
    }

    hpcd.USB_Address = 0U;
    hpcd.State = HAL_PCD_STATE_READY;

    /* Activate LPM */
    if (hpcd.Init.lpm_enable == 1U)
    {
      (void)HAL_PCDEx_ActivateLPM(&hpcd);
    }

    (void)USB_DevDisconnect(USBx);
  }

  void reset() {
    USBx->CNTR &= ~USB_CNTR_HOST;
    USBx->CNTR |= USB_CNTR_USBRST;
  }
};

extern "C" {
/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/** @addtogroup STM32H5xx_HAL_Driver
  * @{
  */

/** @defgroup PCD PCD
  * @brief PCD HAL module driver
  * @{
  */

#ifdef HAL_PCD_MODULE_ENABLED

#if defined (USB_OTG_FS) || defined (USB_OTG_HS) || defined (USB_DRD_FS)

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup PCD_Private_Macros PCD Private Macros
  * @{
  */
#define PCD_MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define PCD_MAX(a, b)  (((a) > (b)) ? (a) : (b))
/**
  * @}
  */

/* Private functions prototypes ----------------------------------------------*/
/** @defgroup PCD_Private_Functions PCD Private Functions
  * @{
  */
#if defined (USB_DRD_FS)
static HAL_StatusTypeDef PCD_EP_ISR_Handler(PCD_HandleTypeDef *hpcd);
#if (USE_USB_DOUBLE_BUFFER == 1U)
static HAL_StatusTypeDef HAL_PCD_EP_DB_Transmit(PCD_HandleTypeDef *hpcd, PCD_EPTypeDef *ep, uint16_t wEPVal);
static uint16_t HAL_PCD_EP_DB_Receive(PCD_HandleTypeDef *hpcd, PCD_EPTypeDef *ep, uint16_t wEPVal);
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */
#endif /* defined (USB_DRD_FS) */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PCD_Exported_Functions PCD Exported Functions
  * @{
  */

/** @defgroup PCD_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
            ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the PCD according to the specified
  *         parameters in the PCD_InitTypeDef and initialize the associated handle.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{
  uint8_t i;

  /* Check the PCD handle allocation */
  if (hpcd == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));

  if (hpcd->State == HAL_PCD_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hpcd->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_PCD_MspInit(hpcd);
  }

  hpcd->State = HAL_PCD_STATE_BUSY;

  /* Disable the Interrupts */
  __HAL_PCD_DISABLE(hpcd);

  /*Init the Core (common init.) */
  if (USB_CoreInit(hpcd->Instance, hpcd->Init) != HAL_OK)
  {
    hpcd->State = HAL_PCD_STATE_ERROR;
    return HAL_ERROR;
  }

  /* Force Device Mode */
  if (USB_SetCurrentMode(hpcd->Instance, USB_DEVICE_MODE) != HAL_OK)
  {
    hpcd->State = HAL_PCD_STATE_ERROR;
    return HAL_ERROR;
  }

  /* Init endpoints structures */
  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
  {
    /* Init ep structure */
    hpcd->IN_ep[i].is_in = 1U;
    hpcd->IN_ep[i].num = i;
    /* Control until ep is activated */
    hpcd->IN_ep[i].type = EP_TYPE_CTRL;
    hpcd->IN_ep[i].maxpacket = 0U;
    hpcd->IN_ep[i].xfer_buff = 0U;
    hpcd->IN_ep[i].xfer_len = 0U;
  }

  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
  {
    hpcd->OUT_ep[i].is_in = 0U;
    hpcd->OUT_ep[i].num = i;
    /* Control until ep is activated */
    hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
    hpcd->OUT_ep[i].maxpacket = 0U;
    hpcd->OUT_ep[i].xfer_buff = 0U;
    hpcd->OUT_ep[i].xfer_len = 0U;
  }

  /* Init Device */
  if (USB_DevInit(hpcd->Instance, hpcd->Init) != HAL_OK)
  {
    hpcd->State = HAL_PCD_STATE_ERROR;
    return HAL_ERROR;
  }

  hpcd->USB_Address = 0U;
  hpcd->State = HAL_PCD_STATE_READY;

  /* Activate LPM */
  if (hpcd->Init.lpm_enable == 1U)
  {
    (void)HAL_PCDEx_ActivateLPM(hpcd);
  }

  (void)USB_DevDisconnect(hpcd->Instance);

  return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd)
{
  /* Check the PCD handle allocation */
  if (hpcd == NULL)
  {
    return HAL_ERROR;
  }

  hpcd->State = HAL_PCD_STATE_BUSY;

  /* Stop Device */
  if (USB_StopDevice(hpcd->Instance) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* DeInit the low level hardware: CLOCK, NVIC.*/
  HAL_PCD_MspDeInit(hpcd);

  hpcd->State = HAL_PCD_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd PCD handle
  * @retval None
  */
__weak void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PCD_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes PCD MSP.
  * @param  hpcd PCD handle
  * @retval None
  */
__weak void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PCD_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group2 Input and Output operation functions
  *  @brief   Data transfers functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the PCD data
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Start the USB device
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd);

  __HAL_PCD_ENABLE(hpcd);
  (void)USB_DevConnect(hpcd->Instance);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @brief  Stop the USB device.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd);
  __HAL_PCD_DISABLE(hpcd);
  (void)USB_DevDisconnect(hpcd->Instance);

  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

#if defined (USB_DRD_FS)
/**
  * @brief  This function handles PCD interrupt request.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
  uint32_t wIstr = USB_ReadInterrupts(hpcd->Instance);

  if ((wIstr & USB_ISTR_CTR) == USB_ISTR_CTR)
  {
    /* servicing of the endpoint correct transfer interrupt */
    /* clear of the CTR flag into the sub */
    (void)PCD_EP_ISR_Handler(hpcd);

    return;
  }

  if ((wIstr & USB_ISTR_RESET) == USB_ISTR_RESET)
  {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_RESET);

    HAL_PCD_ResetCallback(hpcd);

    (void)HAL_PCD_SetAddress(hpcd, 0U);

    return;
  }

  if ((wIstr & USB_ISTR_PMAOVR) == USB_ISTR_PMAOVR)
  {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_PMAOVR);

    return;
  }

  if ((wIstr & USB_ISTR_ERR) == USB_ISTR_ERR)
  {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_ERR);

    return;
  }

  if ((wIstr & USB_ISTR_WKUP) == USB_ISTR_WKUP)
  {
    hpcd->Instance->CNTR &= ~(USB_CNTR_SUSPRDY);
    hpcd->Instance->CNTR &= ~(USB_CNTR_SUSPEN);

    if (hpcd->LPM_State == LPM_L1)
    {
      hpcd->LPM_State = LPM_L0;

      HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L0_ACTIVE);
    }

    HAL_PCD_ResumeCallback(hpcd);

    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_WKUP);

    return;
  }

  if ((wIstr & USB_ISTR_SUSP) == USB_ISTR_SUSP)
  {
    /* Force low-power mode in the macrocell */
    hpcd->Instance->CNTR |= USB_CNTR_SUSPEN;

    /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_SUSP);

    hpcd->Instance->CNTR |= USB_CNTR_SUSPRDY;

    HAL_PCD_SuspendCallback(hpcd);

    return;
  }

  /* Handle LPM Interrupt */
  if ((wIstr & USB_ISTR_L1REQ) == USB_ISTR_L1REQ)
  {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_L1REQ);
    if (hpcd->LPM_State == LPM_L0)
    {
      /* Force suspend and low-power mode before going to L1 state*/
      hpcd->Instance->CNTR |= USB_CNTR_SUSPRDY;
      hpcd->Instance->CNTR |= USB_CNTR_SUSPEN;

      hpcd->LPM_State = LPM_L1;
      hpcd->BESL = ((uint32_t)hpcd->Instance->LPMCSR & USB_LPMCSR_BESL) >> 2;
      HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L1_ACTIVE);
    }
    else
    {
      HAL_PCD_SuspendCallback(hpcd);
    }

    return;
  }

  if ((wIstr & USB_ISTR_SOF) == USB_ISTR_SOF)
  {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_SOF);

    HAL_PCD_SOFCallback(hpcd);

    return;
  }

  if ((wIstr & USB_ISTR_ESOF) == USB_ISTR_ESOF)
  {
    /* clear ESOF flag in ISTR */
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_ESOF);

    return;
  }
}
#endif /* defined (USB_DRD_FS) */


/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group3 Peripheral Control functions
  *  @brief   management functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the PCD data
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Connect the USB device
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd);


  (void)USB_DevConnect(hpcd->Instance);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @brief  Disconnect the USB device.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd);
  (void)USB_DevDisconnect(hpcd->Instance);

  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @brief  Set the USB Device address.
  * @param  hpcd PCD handle
  * @param  address new device address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address)
{
  __HAL_LOCK(hpcd);
  hpcd->USB_Address = address;
  (void)USB_SetDevAddress(hpcd->Instance, address);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}
/**
  * @brief  Open and configure an endpoint.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  ep_mps endpoint max packet size
  * @param  ep_type endpoint type
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                  uint16_t ep_mps, uint8_t ep_type)
{
  HAL_StatusTypeDef ret = HAL_OK;
  PCD_EPTypeDef *ep;

  if ((ep_addr & 0x80U) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 1U;
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 0U;
  }

  ep->num = ep_addr & EP_ADDR_MSK;
  ep->maxpacket = (uint32_t)ep_mps & 0x7FFU;
  ep->type = ep_type;

  /* Set initial data PID. */
  if (ep_type == EP_TYPE_BULK)
  {
    ep->data_pid_start = 0U;
  }

  __HAL_LOCK(hpcd);
  (void)USB_ActivateEndpoint(hpcd->Instance, ep);
  __HAL_UNLOCK(hpcd);

  return ret;
}

/**
  * @brief  Deactivate an endpoint.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  PCD_EPTypeDef *ep;

  if ((ep_addr & 0x80U) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 1U;
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 0U;
  }
  ep->num = ep_addr & EP_ADDR_MSK;

  __HAL_LOCK(hpcd);
  (void)USB_DeactivateEndpoint(hpcd->Instance, ep);
  __HAL_UNLOCK(hpcd);
  return HAL_OK;
}


/**
  * @brief  Receive an amount of data.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the reception buffer
  * @param  len amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
  PCD_EPTypeDef *ep;

  ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];

  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;
  ep->xfer_len = len;
  ep->xfer_count = 0U;
  ep->is_in = 0U;
  ep->num = ep_addr & EP_ADDR_MSK;


  (void)USB_EPStartXfer(hpcd->Instance, ep);

  return HAL_OK;
}

/**
  * @brief  Get Received Data Size
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval Data Size
  */
uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef const *hpcd, uint8_t ep_addr)
{
  return hpcd->OUT_ep[ep_addr & EP_ADDR_MSK].xfer_count;
}
/**
  * @brief  Send an amount of data
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the transmission buffer
  * @param  len amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
  PCD_EPTypeDef *ep;

  ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];

  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;
  ep->xfer_len = len;
#if defined (USB_DRD_FS)
  ep->xfer_fill_db = 1U;
  ep->xfer_len_db = len;
#endif /* defined (USB_DRD_FS) */
  ep->xfer_count = 0U;
  ep->is_in = 1U;
  ep->num = ep_addr & EP_ADDR_MSK;

  (void)USB_EPStartXfer(hpcd->Instance, ep);

  return HAL_OK;
}

/**
  * @brief  Set a STALL condition over an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  PCD_EPTypeDef *ep;

  if (((uint32_t)ep_addr & EP_ADDR_MSK) > hpcd->Init.dev_endpoints)
  {
    return HAL_ERROR;
  }

  if ((0x80U & ep_addr) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 1U;
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr];
    ep->is_in = 0U;
  }

  ep->is_stall = 1U;
  ep->num = ep_addr & EP_ADDR_MSK;

  __HAL_LOCK(hpcd);

  (void)USB_EPSetStall(hpcd->Instance, ep);

  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @brief  Clear a STALL condition over in an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  PCD_EPTypeDef *ep;

  if (((uint32_t)ep_addr & 0x0FU) > hpcd->Init.dev_endpoints)
  {
    return HAL_ERROR;
  }

  if ((0x80U & ep_addr) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 1U;
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 0U;
  }

  ep->is_stall = 0U;
  ep->num = ep_addr & EP_ADDR_MSK;

  __HAL_LOCK(hpcd);
  (void)USB_EPClearStall(hpcd->Instance, ep);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
   * @brief  Abort an USB EP transaction.
   * @param  hpcd PCD handle
   * @param  ep_addr endpoint address
   * @retval HAL status
   */
HAL_StatusTypeDef HAL_PCD_EP_Abort(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  HAL_StatusTypeDef ret;
  PCD_EPTypeDef *ep;

  if ((0x80U & ep_addr) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
  }

  /* Stop Xfer */
  ret = USB_EPStopXfer(hpcd->Instance, ep);

  return ret;
}

/**
  * @brief  Flush an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  __HAL_LOCK(hpcd);

  if ((ep_addr & 0x80U) == 0x80U)
  {
    (void)USB_FlushTxFifo(hpcd->Instance, (uint32_t)ep_addr & EP_ADDR_MSK);
  }
  else
  {
    (void)USB_FlushRxFifo(hpcd->Instance);
  }

  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @brief  Activate remote wakeup signalling
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  return (USB_ActivateRemoteWakeup(hpcd->Instance));
}

/**
  * @brief  De-activate remote wakeup signalling.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  return (USB_DeActivateRemoteWakeup(hpcd->Instance));
}

/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group4 Peripheral State functions
  *  @brief   Peripheral State functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the PCD handle state.
  * @param  hpcd PCD handle
  * @retval HAL state
  */
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef const *hpcd)
{
  return hpcd->State;
}


#if defined (USB_DRD_FS)
/**
  * @brief  This function handles PCD Endpoint interrupt request.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
static HAL_StatusTypeDef PCD_EP_ISR_Handler(PCD_HandleTypeDef *hpcd)
{
  PCD_EPTypeDef *ep;
  uint16_t count;
  uint16_t wIstr;
  uint16_t wEPVal;
  uint16_t TxPctSize;
  uint8_t epindex;

#if (USE_USB_DOUBLE_BUFFER != 1U)
  count = 0U;
#endif /* USE_USB_DOUBLE_BUFFER */

  /* stay in loop while pending interrupts */
  while ((hpcd->Instance->ISTR & USB_ISTR_CTR) != 0U)
  {
    wIstr = (uint16_t)hpcd->Instance->ISTR;

    /* extract highest priority endpoint number */
    epindex = (uint8_t)(wIstr & USB_ISTR_IDN);

    if (epindex == 0U)
    {
      /* Decode and service control endpoint interrupt */

      /* DIR bit = origin of the interrupt */
      if ((wIstr & USB_ISTR_DIR) == 0U)
      {
        /* DIR = 0 */

        /* DIR = 0 => IN  int */
        /* DIR = 0 implies that (EP_CTR_TX = 1) always */
        PCD_CLEAR_TX_EP_CTR(hpcd->Instance, PCD_ENDP0);
        ep = &hpcd->IN_ep[0];

        ep->xfer_count = PCD_GET_EP_TX_CNT(hpcd->Instance, ep->num);
        ep->xfer_buff += ep->xfer_count;

        /* TX COMPLETE */
        HAL_PCD_DataInStageCallback(hpcd, 0U);

        if ((hpcd->USB_Address > 0U) && (ep->xfer_len == 0U))
        {
          hpcd->Instance->DADDR = ((uint16_t)hpcd->USB_Address | USB_DADDR_EF);
          hpcd->USB_Address = 0U;
        }
      }
      else
      {
        /* DIR = 1 */

        /* DIR = 1 & CTR_RX => SETUP or OUT int */
        /* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */
        ep = &hpcd->OUT_ep[0];
        wEPVal = (uint16_t)PCD_GET_ENDPOINT(hpcd->Instance, PCD_ENDP0);

        if ((wEPVal & USB_EP_SETUP) != 0U)
        {
          /* Get SETUP Packet */
          ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);

          if (ep->xfer_count != 8U)
          {
            /* Set Stall condition for EP0 IN/OUT */
            PCD_SET_EP_RX_STATUS(hpcd->Instance, PCD_ENDP0, USB_EP_RX_STALL);
            PCD_SET_EP_TX_STATUS(hpcd->Instance, PCD_ENDP0, USB_EP_TX_STALL);

            /* SETUP bit kept frozen while CTR_RX = 1 */
            PCD_CLEAR_RX_EP_CTR(hpcd->Instance, PCD_ENDP0);

            return HAL_OK;
          }

          USB_ReadPMA(hpcd->Instance, (uint8_t *)hpcd->Setup,
                      ep->pmaadress, (uint16_t)ep->xfer_count);

          /* SETUP bit kept frozen while CTR_RX = 1 */
          PCD_CLEAR_RX_EP_CTR(hpcd->Instance, PCD_ENDP0);

          /* Process SETUP Packet*/
          HAL_PCD_SetupStageCallback(hpcd);
        }
        else if ((wEPVal & USB_EP_VTRX) != 0U)
        {
          PCD_CLEAR_RX_EP_CTR(hpcd->Instance, PCD_ENDP0);

          /* Get Control Data OUT Packet */
          ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);

          if (ep->xfer_count == 0U)
          {
            /* Status phase re-arm for next setup */
            PCD_SET_EP_RX_STATUS(hpcd->Instance, PCD_ENDP0, USB_EP_RX_VALID);
          }
          else
          {
            if (ep->xfer_buff != 0U)
            {
              USB_ReadPMA(hpcd->Instance, ep->xfer_buff,
                          ep->pmaadress, (uint16_t)ep->xfer_count);  /* max 64bytes */

              ep->xfer_buff += ep->xfer_count;

              /* Process Control Data OUT Packet */
              HAL_PCD_DataOutStageCallback(hpcd, 0U);
            }
          }
        }
      }
    }
    else
    {
      /* Decode and service non control endpoints interrupt */
      /* process related endpoint register */
      wEPVal = (uint16_t)PCD_GET_ENDPOINT(hpcd->Instance, epindex);

      if ((wEPVal & USB_EP_VTRX) != 0U)
      {
        /* clear int flag */
        PCD_CLEAR_RX_EP_CTR(hpcd->Instance, epindex);
        ep = &hpcd->OUT_ep[epindex];

        /* OUT Single Buffering */
        if (ep->doublebuffer == 0U)
        {
          count = (uint16_t)PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);

          if (count != 0U)
          {
            USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaadress, count);
          }
        }
#if (USE_USB_DOUBLE_BUFFER == 1U)
        else
        {
          /* manage double buffer bulk out */
          if (ep->type == EP_TYPE_BULK)
          {
            count = HAL_PCD_EP_DB_Receive(hpcd, ep, wEPVal);
          }
          else /* manage double buffer iso out */
          {
            /* free EP OUT Buffer */
            PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 0U);

            if ((PCD_GET_ENDPOINT(hpcd->Instance, ep->num) & USB_EP_DTOG_RX) != 0U)
            {
              /* read from endpoint BUF0Addr buffer */
              count = (uint16_t)PCD_GET_EP_DBUF0_CNT(hpcd->Instance, ep->num);

              if (count != 0U)
              {
                USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr0, count);
              }
            }
            else
            {
              /* read from endpoint BUF1Addr buffer */
              count = (uint16_t)PCD_GET_EP_DBUF1_CNT(hpcd->Instance, ep->num);

              if (count != 0U)
              {
                USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr1, count);
              }
            }
          }
        }
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */

        /* multi-packet on the NON control OUT endpoint */
        ep->xfer_count += count;

        if ((ep->xfer_len == 0U) || (count < ep->maxpacket))
        {
          /* RX COMPLETE */
          HAL_PCD_DataOutStageCallback(hpcd, ep->num);
        }
        else
        {
           ep->xfer_buff += count;
          (void)USB_EPStartXfer(hpcd->Instance, ep);
        }
      }

      if ((wEPVal & USB_EP_VTTX) != 0U)
      {
        ep = &hpcd->IN_ep[epindex];

        /* clear int flag */
        PCD_CLEAR_TX_EP_CTR(hpcd->Instance, epindex);

        if (ep->type == EP_TYPE_ISOC)
        {
          ep->xfer_len = 0U;

#if (USE_USB_DOUBLE_BUFFER == 1U)
          if (ep->doublebuffer != 0U)
          {
            if ((wEPVal & USB_EP_DTOG_TX) != 0U)
            {
              PCD_SET_EP_DBUF0_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);
            }
            else
            {
              PCD_SET_EP_DBUF1_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);
            }
          }
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */

          /* TX COMPLETE */
          HAL_PCD_DataInStageCallback(hpcd, ep->num);
        }
        else
        {
          /* Manage Single Buffer Transaction */
          if ((wEPVal & USB_EP_KIND) == 0U)
          {
            /* Multi-packet on the NON control IN endpoint */
            TxPctSize = (uint16_t)PCD_GET_EP_TX_CNT(hpcd->Instance, ep->num);

            if (ep->xfer_len > TxPctSize)
            {
              ep->xfer_len -= TxPctSize;
            }
            else
            {
              ep->xfer_len = 0U;
            }

            /* Zero Length Packet? */
            if (ep->xfer_len == 0U)
            {
              /* TX COMPLETE */
              HAL_PCD_DataInStageCallback(hpcd, ep->num);
            }
            else
            {
              /* Transfer is not yet Done */
              ep->xfer_buff += TxPctSize;
              ep->xfer_count += TxPctSize;
              (void)USB_EPStartXfer(hpcd->Instance, ep);
            }
          }
#if (USE_USB_DOUBLE_BUFFER == 1U)
          /* Double Buffer bulk IN (bulk transfer Len > Ep_Mps) */
          else
          {
            (void)HAL_PCD_EP_DB_Transmit(hpcd, ep, wEPVal);
          }
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */
        }
      }
    }
  }

  return HAL_OK;
}


#if (USE_USB_DOUBLE_BUFFER == 1U)
/**
  * @brief  Manage double buffer bulk out transaction from ISR
  * @param  hpcd PCD handle
  * @param  ep current endpoint handle
  * @param  wEPVal Last snapshot of EPRx register value taken in ISR
  * @retval HAL status
  */
static uint16_t HAL_PCD_EP_DB_Receive(PCD_HandleTypeDef *hpcd,
                                      PCD_EPTypeDef *ep, uint16_t wEPVal)
{
  uint16_t count;

  /* Manage Buffer0 OUT */
  if ((wEPVal & USB_EP_DTOG_RX) != 0U)
  {
    /* Get count of received Data on buffer0 */
    count = (uint16_t)PCD_GET_EP_DBUF0_CNT(hpcd->Instance, ep->num);

    if (ep->xfer_len >= count)
    {
      ep->xfer_len -= count;
    }
    else
    {
      ep->xfer_len = 0U;
    }

    if (ep->xfer_len == 0U)
    {
      /* Set NAK to OUT endpoint since double buffer is enabled */
      PCD_SET_EP_RX_STATUS(hpcd->Instance, ep->num, USB_EP_RX_NAK);
    }

    /* Check if Buffer1 is in blocked state which requires to toggle */
    if ((wEPVal & USB_EP_DTOG_TX) != 0U)
    {
      PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 0U);
    }

    if (count != 0U)
    {
      USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr0, count);
    }
  }
  /* Manage Buffer 1 DTOG_RX=0 */
  else
  {
    /* Get count of received data */
    count = (uint16_t)PCD_GET_EP_DBUF1_CNT(hpcd->Instance, ep->num);

    if (ep->xfer_len >= count)
    {
      ep->xfer_len -= count;
    }
    else
    {
      ep->xfer_len = 0U;
    }

    if (ep->xfer_len == 0U)
    {
      /* Set NAK on the current endpoint */
      PCD_SET_EP_RX_STATUS(hpcd->Instance, ep->num, USB_EP_RX_NAK);
    }

    /* Need to FreeUser Buffer */
    if ((wEPVal & USB_EP_DTOG_TX) == 0U)
    {
      PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 0U);
    }

    if (count != 0U)
    {
      USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr1, count);
    }
  }

  return count;
}


/**
  * @brief  Manage double buffer bulk IN transaction from ISR
  * @param  hpcd PCD handle
  * @param  ep current endpoint handle
  * @param  wEPVal Last snapshot of EPRx register value taken in ISR
  * @retval HAL status
  */
static HAL_StatusTypeDef HAL_PCD_EP_DB_Transmit(PCD_HandleTypeDef *hpcd,
                                                PCD_EPTypeDef *ep, uint16_t wEPVal)
{
  uint32_t len;
  uint16_t TxPctSize;

  /* Data Buffer0 ACK received */
  if ((wEPVal & USB_EP_DTOG_TX) != 0U)
  {
    /* multi-packet on the NON control IN endpoint */
    TxPctSize = (uint16_t)PCD_GET_EP_DBUF0_CNT(hpcd->Instance, ep->num);

    if (ep->xfer_len > TxPctSize)
    {
      ep->xfer_len -= TxPctSize;
    }
    else
    {
      ep->xfer_len = 0U;
    }

    /* Transfer is completed */
    if (ep->xfer_len == 0U)
    {
      PCD_SET_EP_DBUF0_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);
      PCD_SET_EP_DBUF1_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);

      if (ep->type == EP_TYPE_BULK)
      {
        /* Set Bulk endpoint in NAK state */
        PCD_SET_EP_TX_STATUS(hpcd->Instance, ep->num, USB_EP_TX_NAK);
      }

      /* TX COMPLETE */
      HAL_PCD_DataInStageCallback(hpcd, ep->num);

      if ((wEPVal & USB_EP_DTOG_RX) != 0U)
      {
        PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 1U);
      }

      return HAL_OK;
    }
    else /* Transfer is not yet Done */
    {
      /* Need to Free USB Buffer */
      if ((wEPVal & USB_EP_DTOG_RX) != 0U)
      {
        PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 1U);
      }

      /* Still there is data to Fill in the next Buffer */
      if (ep->xfer_fill_db == 1U)
      {
        ep->xfer_buff += TxPctSize;
        ep->xfer_count += TxPctSize;

        /* Calculate the len of the new buffer to fill */
        if (ep->xfer_len_db >= ep->maxpacket)
        {
          len = ep->maxpacket;
          ep->xfer_len_db -= len;
        }
        else if (ep->xfer_len_db == 0U)
        {
          len = TxPctSize;
          ep->xfer_fill_db = 0U;
        }
        else
        {
          ep->xfer_fill_db = 0U;
          len = ep->xfer_len_db;
          ep->xfer_len_db = 0U;
        }

        /* Write remaining Data to Buffer */
        /* Set the Double buffer counter for pma buffer0 */
        PCD_SET_EP_DBUF0_CNT(hpcd->Instance, ep->num, ep->is_in, len);

        /* Copy user buffer to USB PMA */
        USB_WritePMA(hpcd->Instance, ep->xfer_buff,  ep->pmaaddr0, (uint16_t)len);
      }
    }
  }
  else /* Data Buffer1 ACK received */
  {
    /* multi-packet on the NON control IN endpoint */
    TxPctSize = (uint16_t)PCD_GET_EP_DBUF1_CNT(hpcd->Instance, ep->num);

    if (ep->xfer_len >= TxPctSize)
    {
      ep->xfer_len -= TxPctSize;
    }
    else
    {
      ep->xfer_len = 0U;
    }

    /* Transfer is completed */
    if (ep->xfer_len == 0U)
    {
      PCD_SET_EP_DBUF0_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);
      PCD_SET_EP_DBUF1_CNT(hpcd->Instance, ep->num, ep->is_in, 0U);

      if (ep->type == EP_TYPE_BULK)
      {
        /* Set Bulk endpoint in NAK state */
        PCD_SET_EP_TX_STATUS(hpcd->Instance, ep->num, USB_EP_TX_NAK);
      }

      /* TX COMPLETE */
      HAL_PCD_DataInStageCallback(hpcd, ep->num);

      /* need to Free USB Buff */
      if ((wEPVal & USB_EP_DTOG_RX) == 0U)
      {
        PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 1U);
      }

      return HAL_OK;
    }
    else /* Transfer is not yet Done */
    {
      /* Need to Free USB Buffer */
      if ((wEPVal & USB_EP_DTOG_RX) == 0U)
      {
        PCD_FREE_USER_BUFFER(hpcd->Instance, ep->num, 1U);
      }

      /* Still there is data to Fill in the next Buffer */
      if (ep->xfer_fill_db == 1U)
      {
        ep->xfer_buff += TxPctSize;
        ep->xfer_count += TxPctSize;

        /* Calculate the len of the new buffer to fill */
        if (ep->xfer_len_db >= ep->maxpacket)
        {
          len = ep->maxpacket;
          ep->xfer_len_db -= len;
        }
        else if (ep->xfer_len_db == 0U)
        {
          len = TxPctSize;
          ep->xfer_fill_db = 0U;
        }
        else
        {
          len = ep->xfer_len_db;
          ep->xfer_len_db = 0U;
          ep->xfer_fill_db = 0;
        }

        /* Set the Double buffer counter for pma buffer1 */
        PCD_SET_EP_DBUF1_CNT(hpcd->Instance, ep->num, ep->is_in, len);

        /* Copy the user buffer to USB PMA */
        USB_WritePMA(hpcd->Instance, ep->xfer_buff,  ep->pmaaddr1, (uint16_t)len);
      }
    }
  }

  /* Enable endpoint IN */
  PCD_SET_EP_TX_STATUS(hpcd->Instance, ep->num, USB_EP_TX_VALID);

  return HAL_OK;
}
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */
#endif /* defined (USB_DRD_FS) */

/**
  * @}
  */
#endif /* defined (USB_OTG_FS) || defined (USB_OTG_HS) || defined (USB_DRD_FS) */
#endif /* HAL_PCD_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */


/**
  ******************************************************************************
  * @file    stm32h5xx_hal_pcd_ex.c
  * @author  MCD Application Team
  * @brief   PCD Extended HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the USB Peripheral Controller:
  *           + Extended features functions
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/** @addtogroup STM32H5xx_HAL_Driver
  * @{
  */

/** @defgroup PCDEx PCDEx
  * @brief PCD Extended HAL module driver
  * @{
  */

#ifdef HAL_PCD_MODULE_ENABLED

#if defined (USB_OTG_FS) || defined (USB_OTG_HS) || defined (USB_DRD_FS)
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup PCDEx_Exported_Functions PCDEx Exported Functions
  * @{
  */

/** @defgroup PCDEx_Exported_Functions_Group1 Peripheral Control functions
  * @brief    PCDEx control functions
  *
@verbatim
 ===============================================================================
                 ##### Extended features functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Update FIFO configuration

@endverbatim
  * @{
  */
#if defined (USB_DRD_FS)
/**
  * @brief  Configure PMA for EP
  * @param  hpcd  Device instance
  * @param  ep_addr endpoint address
  * @param  ep_kind endpoint Kind
  *                  USB_SNG_BUF: Single Buffer used
  *                  USB_DBL_BUF: Double Buffer used
  * @param  pmaadress: EP address in The PMA: In case of single buffer endpoint
  *                   this parameter is 16-bit value providing the address
  *                   in PMA allocated to endpoint.
  *                   In case of double buffer endpoint this parameter
  *                   is a 32-bit value providing the endpoint buffer 0 address
  *                   in the LSB part of 32-bit value and endpoint buffer 1 address
  *                   in the MSB part of 32-bit value.
  * @retval HAL status
  */

HAL_StatusTypeDef  HAL_PCDEx_PMAConfig(PCD_HandleTypeDef *hpcd, uint16_t ep_addr,
                                       uint16_t ep_kind, uint32_t pmaadress)
{
  PCD_EPTypeDef *ep;

  /* initialize ep structure*/
  if ((0x80U & ep_addr) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr];
  }

  /* Here we check if the endpoint is single or double Buffer*/
  if (ep_kind == PCD_SNG_BUF)
  {
    /* Single Buffer */
    ep->doublebuffer = 0U;
    /* Configure the PMA */
    ep->pmaadress = (uint16_t)pmaadress;
  }
#if (USE_USB_DOUBLE_BUFFER == 1U)
  else /* USB_DBL_BUF */
  {
    /* Double Buffer Endpoint */
    ep->doublebuffer = 1U;
    /* Configure the PMA */
    ep->pmaaddr0 = (uint16_t)(pmaadress & 0xFFFFU);
    ep->pmaaddr1 = (uint16_t)((pmaadress & 0xFFFF0000U) >> 16);
  }
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */

  return HAL_OK;
}

/**
  * @brief  Activate BatteryCharging feature.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_ActivateBCD(PCD_HandleTypeDef *hpcd)
{
  USB_DRD_TypeDef *USBx = hpcd->Instance;
  hpcd->battery_charging_active = 1U;

  /* Enable BCD feature */
  USBx->BCDR |= USB_BCDR_BCDEN;

  /* Enable DCD : Data Contact Detect */
  USBx->BCDR &= ~(USB_BCDR_PDEN);
  USBx->BCDR &= ~(USB_BCDR_SDEN);
  USBx->BCDR |= USB_BCDR_DCDEN;

  return HAL_OK;
}

/**
  * @brief  Deactivate BatteryCharging feature.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_DeActivateBCD(PCD_HandleTypeDef *hpcd)
{
  USB_DRD_TypeDef *USBx = hpcd->Instance;
  hpcd->battery_charging_active = 0U;

  /* Disable BCD feature */
  USBx->BCDR &= ~(USB_BCDR_BCDEN);

  return HAL_OK;
}

/**
  * @brief  Handle BatteryCharging Process.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
void HAL_PCDEx_BCD_VBUSDetect(PCD_HandleTypeDef *hpcd)
{
  USB_DRD_TypeDef *USBx = hpcd->Instance;
  uint32_t tickstart = HAL_GetTick();

  /* Wait for Min DCD Timeout */
  HAL_Delay(300U);

  /* Data Pin Contact ? Check Detect flag */
  if ((USBx->BCDR & USB_BCDR_DCDET) == USB_BCDR_DCDET)
  {
    HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_CONTACT_DETECTION);
  }
  /* Primary detection: checks if connected to Standard Downstream Port
  (without charging capability) */
  USBx->BCDR &= ~(USB_BCDR_DCDEN);
  HAL_Delay(50U);
  USBx->BCDR |= (USB_BCDR_PDEN);
  HAL_Delay(50U);

  /* If Charger detect ? */
  if ((USBx->BCDR & USB_BCDR_PDET) == USB_BCDR_PDET)
  {
    /* Start secondary detection to check connection to Charging Downstream
    Port or Dedicated Charging Port */
    USBx->BCDR &= ~(USB_BCDR_PDEN);
    HAL_Delay(50U);
    USBx->BCDR |= (USB_BCDR_SDEN);
    HAL_Delay(50U);

    /* If CDP ? */
    if ((USBx->BCDR & USB_BCDR_SDET) == USB_BCDR_SDET)
    {
      /* Dedicated Downstream Port DCP */
      HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_DEDICATED_CHARGING_PORT);
    }
    else
    {
      /* Charging Downstream Port CDP */
      HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_CHARGING_DOWNSTREAM_PORT);
    }
  }
  else /* NO */
  {
    /* Standard Downstream Port */
    HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_STD_DOWNSTREAM_PORT);
  }

  /* Battery Charging capability discovery finished Start Enumeration */
  (void)HAL_PCDEx_DeActivateBCD(hpcd);

  /* Check for the Timeout, else start USB Device */
  if ((HAL_GetTick() - tickstart) > 1000U)
  {
    HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_ERROR);
  }
  else
  {
    HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_DISCOVERY_COMPLETED);
  }
}

/**
  * @brief  Activate LPM feature.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_ActivateLPM(PCD_HandleTypeDef *hpcd)
{

  USB_DRD_TypeDef *USBx = hpcd->Instance;
  hpcd->lpm_active = 1U;
  hpcd->LPM_State = LPM_L0;

  USBx->LPMCSR |= USB_LPMCSR_LMPEN;
  USBx->LPMCSR |= USB_LPMCSR_LPMACK;

  return HAL_OK;
}

/**
  * @brief  Deactivate LPM feature.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_DeActivateLPM(PCD_HandleTypeDef *hpcd)
{
  USB_DRD_TypeDef *USBx = hpcd->Instance;

  hpcd->lpm_active = 0U;

  USBx->LPMCSR &= ~(USB_LPMCSR_LMPEN);
  USBx->LPMCSR &= ~(USB_LPMCSR_LPMACK);

  return HAL_OK;
}
#endif /* defined (USB_DRD_FS) */

/**
  * @brief  Send LPM message to user layer callback.
  * @param  hpcd PCD handle
  * @param  msg LPM message
  * @retval HAL status
  */
__weak void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(msg);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PCDEx_LPM_Callback could be implemented in the user file
   */
}

/**
  * @brief  Send BatteryCharging message to user layer callback.
  * @param  hpcd PCD handle
  * @param  msg LPM message
  * @retval HAL status
  */
__weak void HAL_PCDEx_BCD_Callback(PCD_HandleTypeDef *hpcd, PCD_BCD_MsgTypeDef msg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(msg);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PCDEx_BCD_Callback could be implemented in the user file
   */
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* defined (USB_OTG_FS) || defined (USB_OTG_HS) || defined (USB_DRD_FS) */
#endif /* HAL_PCD_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

}
