#include <halxx/memory_map.hpp>
#include <halxx/clocking.hpp>
#include <halxx/usart.hpp>

#include <stm32h5/gpio.hpp>

using namespace halxx::usart;

#ifdef HAL_UART_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup UART_Private_Constants UART Private Constants
  * @{
  */
#define USART_CR1_FIELDS  ((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE | \
                                      USART_CR1_OVER8 | USART_CR1_FIFOEN)) /*!< UART or USART CR1 fields of parameters set by UART_SetConfig API */

#define USART_CR3_FIELDS  ((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT | USART_CR3_TXFTCFG | \
                                      USART_CR3_RXFTCFG)) /*!< UART or USART CR3 fields of parameters set by UART_SetConfig API */

#define LPUART_BRR_MIN  0x00000300U  /* LPUART BRR minimum authorized value */
#define LPUART_BRR_MAX  0x000FFFFFU  /* LPUART BRR maximum authorized value */

#define UART_BRR_MIN    0x10U        /* UART BRR minimum authorized value */
#define UART_BRR_MAX    0x0000FFFFU  /* UART BRR maximum authorized value */

static void UART_EndRxTransfer(UART_HandleTypeDef *huart);
#if defined(HAL_DMA_MODULE_ENABLED)
static void UART_EndTxTransfer(UART_HandleTypeDef *huart);
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void UART_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void UART_DMAError(DMA_HandleTypeDef *hdma);
static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma);
static void UART_DMATxAbortCallback(DMA_HandleTypeDef *hdma);
static void UART_DMARxAbortCallback(DMA_HandleTypeDef *hdma);
static void UART_DMATxOnlyAbortCallback(DMA_HandleTypeDef *hdma);
static void UART_DMARxOnlyAbortCallback(DMA_HandleTypeDef *hdma);
#endif /* HAL_DMA_MODULE_ENABLED */
static void UART_TxISR_8BIT(UART_HandleTypeDef *huart);
static void UART_TxISR_16BIT(UART_HandleTypeDef *huart);
static void UART_TxISR_8BIT_FIFOEN(UART_HandleTypeDef *huart);
static void UART_TxISR_16BIT_FIFOEN(UART_HandleTypeDef *huart);
static void UART_EndTransmit_IT(UART_HandleTypeDef *huart);
static void UART_RxISR_8BIT(UART_HandleTypeDef *huart);
static void UART_RxISR_16BIT(UART_HandleTypeDef *huart);
static void UART_RxISR_8BIT_FIFOEN(UART_HandleTypeDef *huart);
static void UART_RxISR_16BIT_FIFOEN(UART_HandleTypeDef *huart);

const uint16_t UARTPrescTable[12] = {1U, 2U, 4U, 6U, 8U, 10U, 12U, 16U, 32U, 64U, 128U, 256U};


status USART::Init()
{
#if 0
  if (params.HwFlowCtl != UART_HWCONTROL_NONE)
  {
    /* Check the parameters */
    assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
  }
  else
  {
    /* Check the parameters */
    assert_param((IS_UART_INSTANCE(huart->Instance)) || (IS_LPUART_INSTANCE(huart->Instance)));
  }
#endif
  
  if (huart->gState == HAL_UART_STATE_RESET)
  {
    //huart->Lock = HAL_UNLOCKED;

    HAL_UART_MspInit(huart);
  }

  huart->gState = HAL_UART_STATE_BUSY;

  __HAL_UART_DISABLE(huart);

  /* Perform advanced settings configuration */
  /* For some items, configuration requires to be done prior TE and RE bits are set */
  if (huart->AdvancedInit.AdvFeatureInit != UART_ADVFEATURE_NO_INIT)
  {
    UART_AdvFeatureConfig(huart);
  }

  /* Set the UART Communication parameters */
  if (UART_SetConfig(huart) == HAL_ERROR)
  {
    return HAL_ERROR;
  }

  /* In asynchronous mode, the following bits must be kept cleared:
  - LINEN and CLKEN bits in the USART_CR2 register,
  - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  __HAL_UART_ENABLE(huart);

  /* TEACK and/or REACK to check before moving huart->gState and huart->RxState to Ready */
  return (UART_CheckIdleState(huart));
}

/**
  * @brief Initialize the half-duplex mode according to the specified
  *        parameters in the UART_InitTypeDef and creates the associated handle.
  * @param huart UART handle.
  * @retval HAL status
  */
status USART::HAL_HalfDuplex_Init()
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check UART instance */
  assert_param(IS_UART_HALFDUPLEX_INSTANCE(huart->Instance));

  if (huart->gState == HAL_UART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

    /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
    /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

  __HAL_UART_DISABLE(huart);

  /* Perform advanced settings configuration */
  /* For some items, configuration requires to be done prior TE and RE bits are set */
  if (huart->AdvancedInit.AdvFeatureInit != UART_ADVFEATURE_NO_INIT)
  {
    UART_AdvFeatureConfig(huart);
  }

  /* Set the UART Communication parameters */
  if (UART_SetConfig(huart) == HAL_ERROR)
  {
    return HAL_ERROR;
  }

  /* In half-duplex mode, the following bits must be kept cleared:
  - LINEN and CLKEN bits in the USART_CR2 register,
  - SCEN and IREN bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_IREN | USART_CR3_SCEN));

  /* Enable the Half-Duplex mode by setting the HDSEL bit in the CR3 register */
  SET_BIT(huart->Instance->CR3, USART_CR3_HDSEL);

  __HAL_UART_ENABLE(huart);

  /* TEACK and/or REACK to check before moving huart->gState and huart->RxState to Ready */
  return (UART_CheckIdleState(huart));
}


/**
  * @brief Initialize the LIN mode according to the specified
  *        parameters in the UART_InitTypeDef and creates the associated handle.
  * @param huart             UART handle.
  * @param BreakDetectLength Specifies the LIN break detection length.
  *        This parameter can be one of the following values:
  *          @arg @ref UART_LINBREAKDETECTLENGTH_10B 10-bit break detection
  *          @arg @ref UART_LINBREAKDETECTLENGTH_11B 11-bit break detection
  * @retval HAL status
  */
status USART::HAL_LIN_Init(uint32_t BreakDetectLength)
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the LIN UART instance */
  assert_param(IS_UART_LIN_INSTANCE(huart->Instance));
  /* Check the Break detection length parameter */
  assert_param(IS_UART_LIN_BREAK_DETECT_LENGTH(BreakDetectLength));

  /* LIN mode limited to 16-bit oversampling only */
  if (params.OverSampling == UART_OVERSAMPLING_8)
  {
    return HAL_ERROR;
  }
  /* LIN mode limited to 8-bit data length */
  if (params.WordLength != UART_WORDLENGTH_8B)
  {
    return HAL_ERROR;
  }

  if (huart->gState == HAL_UART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

    /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
    /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

  __HAL_UART_DISABLE(huart);

  /* Perform advanced settings configuration */
  /* For some items, configuration requires to be done prior TE and RE bits are set */
  if (huart->AdvancedInit.AdvFeatureInit != UART_ADVFEATURE_NO_INIT)
  {
    UART_AdvFeatureConfig(huart);
  }

  /* Set the UART Communication parameters */
  if (UART_SetConfig(huart) == HAL_ERROR)
  {
    return HAL_ERROR;
  }

  /* In LIN mode, the following bits must be kept cleared:
  - LINEN and CLKEN bits in the USART_CR2 register,
  - SCEN and IREN bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, USART_CR2_CLKEN);
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_HDSEL | USART_CR3_IREN | USART_CR3_SCEN));

  /* Enable the LIN mode by setting the LINEN bit in the CR2 register */
  SET_BIT(huart->Instance->CR2, USART_CR2_LINEN);

  /* Set the USART LIN Break detection length. */
  MODIFY_REG(huart->Instance->CR2, USART_CR2_LBDL, BreakDetectLength);

  __HAL_UART_ENABLE(huart);

  /* TEACK and/or REACK to check before moving huart->gState and huart->RxState to Ready */
  return (UART_CheckIdleState(huart));
}


/**
  * @brief Initialize the multiprocessor mode according to the specified
  *        parameters in the UART_InitTypeDef and initialize the associated handle.
  * @param huart        UART handle.
  * @param Address      UART node address (4-, 6-, 7- or 8-bit long).
  * @param WakeUpMethod Specifies the UART wakeup method.
  *        This parameter can be one of the following values:
  *          @arg @ref UART_WAKEUPMETHOD_IDLELINE WakeUp by an idle line detection
  *          @arg @ref UART_WAKEUPMETHOD_ADDRESSMARK WakeUp by an address mark
  * @note  If the user resorts to idle line detection wake up, the Address parameter
  *        is useless and ignored by the initialization function.
  * @note  If the user resorts to address mark wake up, the address length detection
  *        is configured by default to 4 bits only. For the UART to be able to
  *        manage 6-, 7- or 8-bit long addresses detection, the API
  *        HAL_MultiProcessorEx_AddressLength_Set() must be called after
  *        HAL_MultiProcessor_Init().
  * @retval HAL status
  */
status USART::HAL_MultiProcessor_Init(uint8_t Address, uint32_t WakeUpMethod)
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the wake up method parameter */
  assert_param(IS_UART_WAKEUPMETHOD(WakeUpMethod));

  if (huart->gState == HAL_UART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

    /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
    /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

  __HAL_UART_DISABLE(huart);

  /* Perform advanced settings configuration */
  /* For some items, configuration requires to be done prior TE and RE bits are set */
  if (huart->AdvancedInit.AdvFeatureInit != UART_ADVFEATURE_NO_INIT)
  {
    UART_AdvFeatureConfig(huart);
  }

  /* Set the UART Communication parameters */
  if (UART_SetConfig(huart) == HAL_ERROR)
  {
    return HAL_ERROR;
  }

  /* In multiprocessor mode, the following bits must be kept cleared:
  - LINEN and CLKEN bits in the USART_CR2 register,
  - SCEN, HDSEL and IREN  bits in the USART_CR3 register. */
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  if (WakeUpMethod == UART_WAKEUPMETHOD_ADDRESSMARK)
  {
    /* If address mark wake up method is chosen, set the USART address node */
    MODIFY_REG(huart->Instance->CR2, USART_CR2_ADD, ((uint32_t)Address << UART_CR2_ADDRESS_LSB_POS));
  }

  /* Set the wake up method by setting the WAKE bit in the CR1 register */
  MODIFY_REG(huart->Instance->CR1, USART_CR1_WAKE, WakeUpMethod);

  __HAL_UART_ENABLE(huart);

  /* TEACK and/or REACK to check before moving huart->gState and huart->RxState to Ready */
  return (UART_CheckIdleState(huart));
}


/**
  * @brief DeInitialize the UART peripheral.
  * @param huart UART handle.
  * @retval HAL status
  */
status USART::HAL_UART_DeInit()
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param((IS_UART_INSTANCE(huart->Instance)) || (IS_LPUART_INSTANCE(huart->Instance)));

  huart->gState = HAL_UART_STATE_BUSY;

  __HAL_UART_DISABLE(huart);

  huart->Instance->CR1 = 0x0U;
  huart->Instance->CR2 = 0x0U;
  huart->Instance->CR3 = 0x0U;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  if (huart->MspDeInitCallback == NULL)
  {
    huart->MspDeInitCallback = HAL_UART_MspDeInit;
  }
  /* DeInit the low level hardware */
  huart->MspDeInitCallback(huart);
#else
  /* DeInit the low level hardware */
  HAL_UART_MspDeInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_RESET;
  huart->RxState = HAL_UART_STATE_RESET;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief Send an amount of data in blocking mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the sent data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 provided through pData.
  * @note When FIFO mode is enabled, writing a data in the TDR register adds one
  *       data to the TXFIFO. Write operations to the TDR register are performed
  *       when TXFNF flag is set. From hardware perspective, TXFNF flag and
  *       TXE are mapped on the same bit-field.
  * @param huart   UART handle.
  * @param pData   Pointer to data buffer (u8 or u16 data elements).
  * @param Size    Amount of data elements (u8 or u16) to be sent.
  * @param Timeout Timeout duration.
  * @retval HAL status
  */
status USART::HAL_UART_Transmit(const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  const uint8_t  *pdata8bits;
  const uint16_t *pdata16bits;
  uint32_t tickstart;

  /* Check that a Tx process is not already ongoing */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

#if defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Tx request if enabled */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
    {
      CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);
    }

#endif /* USART_DMAREQUESTS_SW_WA */
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* Init tickstart for timeout management */
    tickstart = HAL_GetTick();

    huart->TxXferSize  = Size;
    huart->TxXferCount = Size;

    /* In case of 9bits/No Parity transfer, pData needs to be handled as a uint16_t pointer */
    if ((params.WordLength == UART_WORDLENGTH_9B) && (params.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (const uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }

    while (huart->TxXferCount > 0U)
    {
      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
      {

        huart->gState = HAL_UART_STATE_READY;

        return HAL_TIMEOUT;
      }
      if (pdata8bits == NULL)
      {
        huart->Instance->TDR = (uint16_t)(*pdata16bits & 0x01FFU);
        pdata16bits++;
      }
      else
      {
        huart->Instance->TDR = (uint8_t)(*pdata8bits & 0xFFU);
        pdata8bits++;
      }
      huart->TxXferCount--;
    }

    if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, tickstart, Timeout) != HAL_OK)
    {
      huart->gState = HAL_UART_STATE_READY;

      return HAL_TIMEOUT;
    }

    /* At end of Tx process, restore huart->gState to Ready */
    huart->gState = HAL_UART_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in blocking mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the received data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 available through pData.
  * @note When FIFO mode is enabled, the RXFNE flag is set as long as the RXFIFO
  *       is not empty. Read operations from the RDR register are performed when
  *       RXFNE flag is set. From hardware perspective, RXFNE flag and
  *       RXNE are mapped on the same bit-field.
  * @param huart   UART handle.
  * @param pData   Pointer to data buffer (u8 or u16 data elements).
  * @param Size    Amount of data elements (u8 or u16) to be received.
  * @param Timeout Timeout duration.
  * @retval HAL status
  */
status USART::HAL_UART_Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;
  uint16_t uhMask;
  uint32_t tickstart;

  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

#if defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Rx request if enabled */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
    {
      CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
    }

#endif /* USART_DMAREQUESTS_SW_WA */
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    /* Init tickstart for timeout management */
    tickstart = HAL_GetTick();

    huart->RxXferSize  = Size;
    huart->RxXferCount = Size;

    /* Computation of UART mask to apply to RDR register */
    UART_MASK_COMPUTATION(huart);
    uhMask = huart->Mask;

    /* In case of 9bits/No Parity transfer, pRxData needs to be handled as a uint16_t pointer */
    if ((params.WordLength == UART_WORDLENGTH_9B) && (params.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }

    /* as long as data have to be received */
    while (huart->RxXferCount > 0U)
    {
      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
      {
        huart->RxState = HAL_UART_STATE_READY;

        return HAL_TIMEOUT;
      }
      if (pdata8bits == NULL)
      {
        *pdata16bits = (uint16_t)(huart->Instance->RDR & uhMask);
        pdata16bits++;
      }
      else
      {
        *pdata8bits = (uint8_t)(huart->Instance->RDR & (uint8_t)uhMask);
        pdata8bits++;
      }
      huart->RxXferCount--;
    }

    /* At end of Rx process, restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

#if 0

/**
  * @brief Send an amount of data in interrupt mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the sent data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 provided through pData.
  * @param huart UART handle.
  * @param pData Pointer to data buffer (u8 or u16 data elements).
  * @param Size  Amount of data elements (u8 or u16) to be sent.
  * @retval HAL status
  */
status HAL_UART_Transmit_IT(const uint8_t *pData, uint16_t Size)
{
  /* Check that a Tx process is not already ongoing */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

#if defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Tx request if enabled */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
    {
      CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);
    }

#endif /* USART_DMAREQUESTS_SW_WA */
    huart->pTxBuffPtr  = pData;
    huart->TxXferSize  = Size;
    huart->TxXferCount = Size;
    huart->TxISR       = NULL;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* Configure Tx interrupt processing */
    if (huart->FifoMode == UART_FIFOMODE_ENABLE)
    {
      /* Set the Tx ISR function pointer according to the data word length */
      if ((params.WordLength == UART_WORDLENGTH_9B) && (params.Parity == UART_PARITY_NONE))
      {
        huart->TxISR = UART_TxISR_16BIT_FIFOEN;
      }
      else
      {
        huart->TxISR = UART_TxISR_8BIT_FIFOEN;
      }

      /* Enable the TX FIFO threshold interrupt */
      ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_TXFTIE);
    }
    else
    {
      /* Set the Tx ISR function pointer according to the data word length */
      if ((params.WordLength == UART_WORDLENGTH_9B) && (params.Parity == UART_PARITY_NONE))
      {
        huart->TxISR = UART_TxISR_16BIT;
      }
      else
      {
        huart->TxISR = UART_TxISR_8BIT;
      }

      /* Enable the Transmit Data Register Empty interrupt */
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in interrupt mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the received data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 available through pData.
  * @param huart UART handle.
  * @param pData Pointer to data buffer (u8 or u16 data elements).
  * @param Size  Amount of data elements (u8 or u16) to be received.
  * @retval HAL status
  */
status HAL_UART_Receive_IT(uint8_t *pData, uint16_t Size)
{
  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* Set Reception type to Standard reception */
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

#if defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Rx request if enabled */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
    {
      CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
    }

#endif /* USART_DMAREQUESTS_SW_WA */
    if (!(IS_LPUART_INSTANCE(huart->Instance)))
    {
      /* Check that USART RTOEN bit is set */
      if (READ_BIT(huart->Instance->CR2, USART_CR2_RTOEN) != 0U)
      {
        /* Enable the UART Receiver Timeout Interrupt */
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_RTOIE);
      }
    }

    return (UART_Start_Receive_IT(huart, pData, Size));
  }
  else
  {
    return HAL_BUSY;
  }
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief Send an amount of data in DMA mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the sent data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 provided through pData.
  * @param huart UART handle.
  * @param pData Pointer to data buffer (u8 or u16 data elements).
  * @param Size  Amount of data elements (u8 or u16) to be sent.
  * @retval HAL status
  */
status HAL_UART_Transmit_DMA(const uint8_t *pData, uint16_t Size)
{
  status status;
  uint16_t nbByte = Size;

  /* Check that a Tx process is not already ongoing */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    huart->pTxBuffPtr  = pData;
    huart->TxXferSize  = Size;
    huart->TxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

#if defined(USART_DMAREQUESTS_SW_WA)
    /* Clear the TC flag in the ICR register */
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_TCF);

#endif /* USART_DMAREQUESTS_SW_WA */
    if (huart->hdmatx != NULL)
    {
      /* Set the UART DMA transfer complete callback */
      huart->hdmatx->XferCpltCallback = UART_DMATransmitCplt;

      /* Set the UART DMA Half transfer complete callback */
      huart->hdmatx->XferHalfCpltCallback = UART_DMATxHalfCplt;

      /* Set the DMA error callback */
      huart->hdmatx->XferErrorCallback = UART_DMAError;

      /* Set the DMA abort callback */
      huart->hdmatx->XferAbortCallback = NULL;

      /* In case of 9bits/No Parity transfer, pData buffer provided as input parameter
         should be aligned on a u16 frontier, so nbByte should be equal to Size * 2 */
      if ((params.WordLength == UART_WORDLENGTH_9B) && (params.Parity == UART_PARITY_NONE))
      {
        nbByte = Size * 2U;
      }

      /* Check linked list mode */
      if ((huart->hdmatx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
      {
        if ((huart->hdmatx->LinkedListQueue != NULL) && (huart->hdmatx->LinkedListQueue->Head != NULL))
        {
          /* Set DMA data size */
          huart->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = nbByte;

          /* Set DMA source address */
          huart->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)huart->pTxBuffPtr;

          /* Set DMA destination address */
          huart->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] =
            (uint32_t)&huart->Instance->TDR;

          /* Enable the UART transmit DMA channel */
          status = HAL_DMAEx_List_Start_IT(huart->hdmatx);
        }
        else
        {
          /* Update status */
          status = HAL_ERROR;
        }
      }
      else
      {
        /* Enable the UART transmit DMA channel */
        status = HAL_DMA_Start_IT(huart->hdmatx, (uint32_t)huart->pTxBuffPtr, (uint32_t)&huart->Instance->TDR, nbByte);
      }

      if (status != HAL_OK)
      {
        /* Set error code to DMA */
        huart->ErrorCode = HAL_UART_ERROR_DMA;

        /* Restore huart->gState to ready */
        huart->gState = HAL_UART_STATE_READY;

        return HAL_ERROR;
      }
    }
#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Clear the TC flag in the ICR register */
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_TCF);

#endif /* USART_DMAREQUESTS_SW_WA */
    /* Enable the DMA transfer for transmit request by setting the DMAT bit
    in the UART CR3 register */
    ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in DMA mode.
  * @note   When the UART parity is enabled (PCE = 1), the received data contain
  *         the parity bit (MSB position).
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the received data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 available through pData.
  * @param huart UART handle.
  * @param pData Pointer to data buffer (u8 or u16 data elements).
  * @param Size  Amount of data elements (u8 or u16) to be received.
  * @retval HAL status
  */
status HAL_UART_Receive_DMA(uint8_t *pData, uint16_t Size)
{
  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* Set Reception type to Standard reception */
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    if (!(IS_LPUART_INSTANCE(huart->Instance)))
    {
      /* Check that USART RTOEN bit is set */
      if (READ_BIT(huart->Instance->CR2, USART_CR2_RTOEN) != 0U)
      {
        /* Enable the UART Receiver Timeout Interrupt */
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_RTOIE);
      }
    }

    return (UART_Start_Receive_DMA(huart, pData, Size));
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Pause the DMA Transfer.
  * @param huart UART handle.
  * @retval HAL status
  */
status HAL_UART_DMAPause()
{
  const HAL_UART_StateTypeDef gstate = huart->gState;
  const HAL_UART_StateTypeDef rxstate = huart->RxState;

  if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT)) &&
      (gstate == HAL_UART_STATE_BUSY_TX))
  {
    /* Suspend the UART DMA Tx channel : use blocking DMA Suspend API (no callback) */
    if (huart->hdmatx != NULL)
    {
      /* Set the UART DMA Suspend callback to Null.
         No call back execution at end of DMA Suspend procedure */
      huart->hdmatx->XferSuspendCallback = NULL;

      if (HAL_DMAEx_Suspend(huart->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmatx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }
  if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)) &&
      (rxstate == HAL_UART_STATE_BUSY_RX))
  {
    /* Suspend the UART DMA Rx channel : use blocking DMA Suspend API (no callback) */
    if (huart->hdmarx != NULL)
    {
      /* Disable PE and ERR (Frame error, noise error, overrun error) interrupts */
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
      ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

      /* Set the UART DMA Suspend callback to Null.
         No call back execution at end of DMA Suspend procedure */
      huart->hdmarx->XferSuspendCallback = NULL;

      if (HAL_DMAEx_Suspend(huart->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }

  return HAL_OK;
}

/**
  * @brief Resume the DMA Transfer.
  * @param huart UART handle.
  * @retval HAL status
  */
status HAL_UART_DMAResume()
{
  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    /* Resume the UART DMA Tx channel */
    if (huart->hdmatx != NULL)
    {
      if (HAL_DMAEx_Resume(huart->hdmatx) != HAL_OK)
      {
        /* Set error code to DMA */
        huart->ErrorCode = HAL_UART_ERROR_DMA;

        return HAL_ERROR;
      }
    }
  }
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    /* Clear the Overrun flag before resuming the Rx transfer */
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);

    /* Re-enable PE and ERR (Frame error, noise error, overrun error) interrupts */
    if (params.Parity != UART_PARITY_NONE)
    {
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    }
    ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

    /* Resume the UART DMA Rx channel */
    if (huart->hdmarx != NULL)
    {
      if (HAL_DMAEx_Resume(huart->hdmarx) != HAL_OK)
      {
        /* Set error code to DMA */
        huart->ErrorCode = HAL_UART_ERROR_DMA;

        return HAL_ERROR;
      }
    }
  }

  return HAL_OK;
}

/**
  * @brief Stop the DMA Transfer.
  * @param huart UART handle.
  * @retval HAL status
  */
status HAL_UART_DMAStop()
{
  /* The Lock is not implemented on this API to allow the user application
     to call the HAL UART API under callbacks HAL_UART_TxCpltCallback() / HAL_UART_RxCpltCallback() /
     HAL_UART_TxHalfCpltCallback / HAL_UART_RxHalfCpltCallback:
     indeed, when HAL_DMA_Abort() API is called, the DMA TX/RX Transfer or Half Transfer complete
     interrupt is generated if the DMA transfer interruption occurs at the middle or at the end of
     the stream and the corresponding call back is executed. */

  const HAL_UART_StateTypeDef gstate = huart->gState;
  const HAL_UART_StateTypeDef rxstate = huart->RxState;

  /* Stop UART DMA Tx request if ongoing */
  if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT)) &&
      (gstate == HAL_UART_STATE_BUSY_TX))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* Abort the UART DMA Tx channel */
    if (huart->hdmatx != NULL)
    {
      if (HAL_DMA_Abort(huart->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmatx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }

    UART_EndTxTransfer(huart);
  }

  /* Stop UART DMA Rx request if ongoing */
  if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)) &&
      (rxstate == HAL_UART_STATE_BUSY_RX))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* Abort the UART DMA Rx channel */
    if (huart->hdmarx != NULL)
    {
      if (HAL_DMA_Abort(huart->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }

    UART_EndRxTransfer(huart);
  }

  return HAL_OK;
}
#endif /* HAL_DMA_MODULE_ENABLED */

#endif

/**
  * @brief  Abort ongoing transfers (blocking mode).
  * @param  huart UART handle.
  * @note   This procedure could be used for aborting any ongoing transfer started in Interrupt or DMA mode.
  *         This procedure performs following operations :
  *           - Disable UART Interrupts (Tx and Rx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort (in case of transfer in DMA mode)
  *           - Set handle State to READY
  * @note   This procedure is executed in blocking mode : when exiting function, Abort is considered as completed.
  * @retval HAL status
  */
status HAL_UART_Abort()
{
  /* Disable TXE, TC, RXNE, PE, RXFT, TXFT and ERR (Frame error, noise error, overrun error) interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE |
                                          USART_CR1_TXEIE_TXFNFIE | USART_CR1_TCIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE | USART_CR3_RXFTIE | USART_CR3_TXFTIE);

  /* If Reception till IDLE event was ongoing, disable IDLEIE interrupt */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_IDLEIE));
  }

#if defined(HAL_DMA_MODULE_ENABLED)
  /* Abort the UART DMA Tx channel if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Tx request if enabled */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* Abort the UART DMA Tx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmatx != NULL)
    {
      /* Set the UART DMA Abort callback to Null.
         No call back execution at end of DMA abort procedure */
      huart->hdmatx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmatx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }

  /* Abort the UART DMA Rx channel if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Rx request if enabled */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* Abort the UART DMA Rx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmarx != NULL)
    {
      /* Set the UART DMA Abort callback to Null.
         No call back execution at end of DMA abort procedure */
      huart->hdmarx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* HAL_DMA_MODULE_ENABLED */

  /* Reset Tx and Rx transfer counters */
  huart->TxXferCount = 0U;
  huart->RxXferCount = 0U;

  /* Clear the Error flags in the ICR register */
  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);

  /* Flush the whole TX FIFO (if needed) */
  if (huart->FifoMode == UART_FIFOMODE_ENABLE)
  {
    __HAL_UART_SEND_REQ(huart, UART_TXDATA_FLUSH_REQUEST);
  }

  /* Discard the received data */
  __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);

  /* Restore huart->gState and huart->RxState to Ready */
  huart->gState  = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  huart->ErrorCode = HAL_UART_ERROR_NONE;

  return HAL_OK;
}

/**
  * @brief  Abort ongoing Transmit transfer (blocking mode).
  * @param  huart UART handle.
  * @note   This procedure could be used for aborting any ongoing Tx transfer started in Interrupt or DMA mode.
  *         This procedure performs following operations :
  *           - Disable UART Interrupts (Tx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort (in case of transfer in DMA mode)
  *           - Set handle State to READY
  * @note   This procedure is executed in blocking mode : when exiting function, Abort is considered as completed.
  * @retval HAL status
  */
status HAL_UART_AbortTransmit()
{
  /* Disable TCIE, TXEIE and TXFTIE interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TCIE | USART_CR1_TXEIE_TXFNFIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_TXFTIE);

#if defined(HAL_DMA_MODULE_ENABLED)
  /* Abort the UART DMA Tx channel if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Tx request if enabled */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* Abort the UART DMA Tx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmatx != NULL)
    {
      /* Set the UART DMA Abort callback to Null.
         No call back execution at end of DMA abort procedure */
      huart->hdmatx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmatx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* HAL_DMA_MODULE_ENABLED */

  /* Reset Tx transfer counter */
  huart->TxXferCount = 0U;

  /* Flush the whole TX FIFO (if needed) */
  if (huart->FifoMode == UART_FIFOMODE_ENABLE)
  {
    __HAL_UART_SEND_REQ(huart, UART_TXDATA_FLUSH_REQUEST);
  }

  /* Restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Abort ongoing Receive transfer (blocking mode).
  * @param  huart UART handle.
  * @note   This procedure could be used for aborting any ongoing Rx transfer started in Interrupt or DMA mode.
  *         This procedure performs following operations :
  *           - Disable UART Interrupts (Rx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort (in case of transfer in DMA mode)
  *           - Set handle State to READY
  * @note   This procedure is executed in blocking mode : when exiting function, Abort is considered as completed.
  * @retval HAL status
  */
status HAL_UART_AbortReceive()
{
  /* Disable PEIE, EIE, RXNEIE and RXFTIE interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE | USART_CR3_RXFTIE);

  /* If Reception till IDLE event was ongoing, disable IDLEIE interrupt */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_IDLEIE));
  }

#if defined(HAL_DMA_MODULE_ENABLED)
  /* Abort the UART DMA Rx channel if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Rx request if enabled */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* Abort the UART DMA Rx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmarx != NULL)
    {
      /* Set the UART DMA Abort callback to Null.
         No call back execution at end of DMA abort procedure */
      huart->hdmarx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* HAL_DMA_MODULE_ENABLED */

  /* Reset Rx transfer counter */
  huart->RxXferCount = 0U;

  /* Clear the Error flags in the ICR register */
  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);

  /* Discard the received data */
  __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);

  /* Restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  return HAL_OK;
}

/**
  * @brief  Abort ongoing transfers (Interrupt mode).
  * @param  huart UART handle.
  * @note   This procedure could be used for aborting any ongoing transfer started in Interrupt or DMA mode.
  *         This procedure performs following operations :
  *           - Disable UART Interrupts (Tx and Rx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort_IT (in case of transfer in DMA mode)
  *           - Set handle State to READY
  *           - At abort completion, call user abort complete callback
  * @note   This procedure is executed in Interrupt mode, meaning that abort procedure could be
  *         considered as completed only when user abort complete callback is executed (not when exiting function).
  * @retval HAL status
  */
status HAL_UART_Abort_IT()
{
  uint32_t abortcplt = 1U;

  /* Disable interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_PEIE | USART_CR1_TCIE | USART_CR1_RXNEIE_RXFNEIE |
                                          USART_CR1_TXEIE_TXFNFIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE | USART_CR3_TXFTIE));

  /* If Reception till IDLE event was ongoing, disable IDLEIE interrupt */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_IDLEIE));
  }

#if defined(HAL_DMA_MODULE_ENABLED)
  /* If DMA Tx and/or DMA Rx Handles are associated to UART Handle, DMA Abort complete callbacks should be initialised
     before any call to DMA Abort functions */
  /* DMA Tx Handle is valid */
  if (huart->hdmatx != NULL)
  {
    /* Set DMA Abort Complete callback if UART DMA Tx request if enabled.
       Otherwise, set it to NULL */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
    {
      huart->hdmatx->XferAbortCallback = UART_DMATxAbortCallback;
    }
    else
    {
      huart->hdmatx->XferAbortCallback = NULL;
    }
  }
  /* DMA Rx Handle is valid */
  if (huart->hdmarx != NULL)
  {
    /* Set DMA Abort Complete callback if UART DMA Rx request if enabled.
       Otherwise, set it to NULL */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
    {
      huart->hdmarx->XferAbortCallback = UART_DMARxAbortCallback;
    }
    else
    {
      huart->hdmarx->XferAbortCallback = NULL;
    }
  }

  /* Abort the UART DMA Tx channel if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable DMA Tx at UART level */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* Abort the UART DMA Tx channel : use non blocking DMA Abort API (callback) */
    if (huart->hdmatx != NULL)
    {
      /* UART Tx DMA Abort callback has already been initialised :
         will lead to call HAL_UART_AbortCpltCallback() at end of DMA abort procedure */

      /* Abort DMA TX */
      if (HAL_DMA_Abort_IT(huart->hdmatx) != HAL_OK)
      {
        huart->hdmatx->XferAbortCallback = NULL;
      }
      else
      {
        abortcplt = 0U;
      }
    }
  }

  /* Abort the UART DMA Rx channel if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Rx request if enabled */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* Abort the UART DMA Rx channel : use non blocking DMA Abort API (callback) */
    if (huart->hdmarx != NULL)
    {
      /* UART Rx DMA Abort callback has already been initialised :
         will lead to call HAL_UART_AbortCpltCallback() at end of DMA abort procedure */

      /* Abort DMA RX */
      if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
      {
        huart->hdmarx->XferAbortCallback = NULL;
        abortcplt = 1U;
      }
      else
      {
        abortcplt = 0U;
      }
    }
  }
#endif /* HAL_DMA_MODULE_ENABLED */

  /* if no DMA abort complete callback execution is required => call user Abort Complete callback */
  if (abortcplt == 1U)
  {
    /* Reset Tx and Rx transfer counters */
    huart->TxXferCount = 0U;
    huart->RxXferCount = 0U;

    /* Clear ISR function pointers */
    huart->RxISR = NULL;
    huart->TxISR = NULL;

    /* Reset errorCode */
    huart->ErrorCode = HAL_UART_ERROR_NONE;

    /* Clear the Error flags in the ICR register */
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);

    /* Flush the whole TX FIFO (if needed) */
    if (huart->FifoMode == UART_FIFOMODE_ENABLE)
    {
      __HAL_UART_SEND_REQ(huart, UART_TXDATA_FLUSH_REQUEST);
    }

    /* Discard the received data */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);

    /* Restore huart->gState and huart->RxState to Ready */
    huart->gState  = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* Call registered Abort complete callback */
    huart->AbortCpltCallback(huart);
#else
    /* Call legacy weak Abort complete callback */
    HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  Abort ongoing Transmit transfer (Interrupt mode).
  * @param  huart UART handle.
  * @note   This procedure could be used for aborting any ongoing Tx transfer started in Interrupt or DMA mode.
  *         This procedure performs following operations :
  *           - Disable UART Interrupts (Tx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort_IT (in case of transfer in DMA mode)
  *           - Set handle State to READY
  *           - At abort completion, call user abort complete callback
  * @note   This procedure is executed in Interrupt mode, meaning that abort procedure could be
  *         considered as completed only when user abort complete callback is executed (not when exiting function).
  * @retval HAL status
  */
status HAL_UART_AbortTransmit_IT()
{
  /* Disable interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TCIE | USART_CR1_TXEIE_TXFNFIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_TXFTIE);

#if defined(HAL_DMA_MODULE_ENABLED)
  /* Abort the UART DMA Tx channel if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Tx request if enabled */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* Abort the UART DMA Tx channel : use non blocking DMA Abort API (callback) */
    if (huart->hdmatx != NULL)
    {
      /* Set the UART DMA Abort callback :
         will lead to call HAL_UART_AbortCpltCallback() at end of DMA abort procedure */
      huart->hdmatx->XferAbortCallback = UART_DMATxOnlyAbortCallback;

      /* Abort DMA TX */
      if (HAL_DMA_Abort_IT(huart->hdmatx) != HAL_OK)
      {
        /* Call Directly huart->hdmatx->XferAbortCallback function in case of error */
        huart->hdmatx->XferAbortCallback(huart->hdmatx);
      }
    }
    else
    {
      /* Reset Tx transfer counter */
      huart->TxXferCount = 0U;

      /* Clear TxISR function pointers */
      huart->TxISR = NULL;

      /* Restore huart->gState to Ready */
      huart->gState = HAL_UART_STATE_READY;

      /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
      /* Call registered Abort Transmit Complete Callback */
      huart->AbortTransmitCpltCallback(huart);
#else
      /* Call legacy weak Abort Transmit Complete Callback */
      HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    }
  }
  else
#endif /* HAL_DMA_MODULE_ENABLED */
  {
    /* Reset Tx transfer counter */
    huart->TxXferCount = 0U;

    /* Clear TxISR function pointers */
    huart->TxISR = NULL;

    /* Flush the whole TX FIFO (if needed) */
    if (huart->FifoMode == UART_FIFOMODE_ENABLE)
    {
      __HAL_UART_SEND_REQ(huart, UART_TXDATA_FLUSH_REQUEST);
    }

    /* Restore huart->gState to Ready */
    huart->gState = HAL_UART_STATE_READY;

    /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* Call registered Abort Transmit Complete Callback */
    huart->AbortTransmitCpltCallback(huart);
#else
    /* Call legacy weak Abort Transmit Complete Callback */
    HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  Abort ongoing Receive transfer (Interrupt mode).
  * @param  huart UART handle.
  * @note   This procedure could be used for aborting any ongoing Rx transfer started in Interrupt or DMA mode.
  *         This procedure performs following operations :
  *           - Disable UART Interrupts (Rx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort_IT (in case of transfer in DMA mode)
  *           - Set handle State to READY
  *           - At abort completion, call user abort complete callback
  * @note   This procedure is executed in Interrupt mode, meaning that abort procedure could be
  *         considered as completed only when user abort complete callback is executed (not when exiting function).
  * @retval HAL status
  */
status HAL_UART_AbortReceive_IT()
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));

  /* If Reception till IDLE event was ongoing, disable IDLEIE interrupt */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_IDLEIE));
  }

#if defined(HAL_DMA_MODULE_ENABLED)
  /* Abort the UART DMA Rx channel if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the UART DMA Rx request if enabled */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* Abort the UART DMA Rx channel : use non blocking DMA Abort API (callback) */
    if (huart->hdmarx != NULL)
    {
      /* Set the UART DMA Abort callback :
         will lead to call HAL_UART_AbortCpltCallback() at end of DMA abort procedure */
      huart->hdmarx->XferAbortCallback = UART_DMARxOnlyAbortCallback;

      /* Abort DMA RX */
      if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
      {
        /* Call Directly huart->hdmarx->XferAbortCallback function in case of error */
        huart->hdmarx->XferAbortCallback(huart->hdmarx);
      }
    }
    else
    {
      /* Reset Rx transfer counter */
      huart->RxXferCount = 0U;

      /* Clear RxISR function pointer */
      huart->pRxBuffPtr = NULL;

      /* Clear the Error flags in the ICR register */
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);

      /* Discard the received data */
      __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);

      /* Restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;
      huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

      /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
      /* Call registered Abort Receive Complete Callback */
      huart->AbortReceiveCpltCallback(huart);
#else
      /* Call legacy weak Abort Receive Complete Callback */
      HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    }
  }
  else
#endif /* HAL_DMA_MODULE_ENABLED */
  {
    /* Reset Rx transfer counter */
    huart->RxXferCount = 0U;

    /* Clear RxISR function pointer */
    huart->pRxBuffPtr = NULL;

    /* Clear the Error flags in the ICR register */
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);

    /* Restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* Call registered Abort Receive Complete Callback */
    huart->AbortReceiveCpltCallback(huart);
#else
    /* Call legacy weak Abort Receive Complete Callback */
    HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief Handle UART interrupt request.
  * @param huart UART handle.
  * @retval None
  */
void HAL_UART_IRQHandler()
{
  uint32_t isrflags   = READ_REG(huart->Instance->ISR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);

  uint32_t errorflags;
  uint32_t errorcode;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_RTOF));
  if (errorflags == 0U)
  {
    /* UART in mode Receiver ---------------------------------------------------*/
    if (((isrflags & USART_ISR_RXNE_RXFNE) != 0U)
        && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U)
            || ((cr3its & USART_CR3_RXFTIE) != 0U)))
    {
      if (huart->RxISR != NULL)
      {
        huart->RxISR(huart);
      }
      return;
    }
  }

  /* If some errors occur */
  if ((errorflags != 0U)
      && ((((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != 0U)
           || ((cr1its & (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE | USART_CR1_RTOIE)) != 0U))))
  {
    /* UART parity error interrupt occurred -------------------------------------*/
    if (((isrflags & USART_ISR_PE) != 0U) && ((cr1its & USART_CR1_PEIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);

      huart->ErrorCode |= HAL_UART_ERROR_PE;
    }

    /* UART frame error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_FE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);

      huart->ErrorCode |= HAL_UART_ERROR_FE;
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_NE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);

      huart->ErrorCode |= HAL_UART_ERROR_NE;
    }

    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if (((isrflags & USART_ISR_ORE) != 0U)
        && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
            ((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != 0U)))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);

      huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }

    /* UART Receiver Timeout interrupt occurred ---------------------------------*/
    if (((isrflags & USART_ISR_RTOF) != 0U) && ((cr1its & USART_CR1_RTOIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_RTOF);

      huart->ErrorCode |= HAL_UART_ERROR_RTO;
    }

    /* Call UART Error Call back function if need be ----------------------------*/
    if (huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
      /* UART in mode Receiver --------------------------------------------------*/
      if (((isrflags & USART_ISR_RXNE_RXFNE) != 0U)
          && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U)
              || ((cr3its & USART_CR3_RXFTIE) != 0U)))
      {
        if (huart->RxISR != NULL)
        {
          huart->RxISR(huart);
        }
      }

      /* If Error is to be considered as blocking :
          - Receiver Timeout error in Reception
          - Overrun error in Reception
          - any error occurs in DMA mode reception
      */
      errorcode = huart->ErrorCode;
      if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)) ||
          ((errorcode & (HAL_UART_ERROR_RTO | HAL_UART_ERROR_ORE)) != 0U))
      {
        /* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
        UART_EndRxTransfer(huart);

#if defined(HAL_DMA_MODULE_ENABLED)
        /* Abort the UART DMA Rx channel if enabled */
        if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
        {
#if !defined(USART_DMAREQUESTS_SW_WA)
          /* Disable the UART DMA Rx request if enabled */
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

#endif /* !USART_DMAREQUESTS_SW_WA */
          /* Abort the UART DMA Rx channel */
          if (huart->hdmarx != NULL)
          {
            /* Set the UART DMA Abort callback :
               will lead to call HAL_UART_ErrorCallback() at end of DMA abort procedure */
            huart->hdmarx->XferAbortCallback = UART_DMAAbortOnError;

            /* Abort DMA RX */
            if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
            {
              /* Call Directly huart->hdmarx->XferAbortCallback function in case of error */
              huart->hdmarx->XferAbortCallback(huart->hdmarx);
            }
          }
          else
          {
            /* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
            /*Call registered error callback*/
            huart->ErrorCallback(huart);
#else
            /*Call legacy weak error callback*/
            HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

          }
        }
        else
#endif /* HAL_DMA_MODULE_ENABLED */
        {
          /* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /*Call registered error callback*/
          huart->ErrorCallback(huart);
#else
          /*Call legacy weak error callback*/
          HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
        }
      }
      else
      {
        /* Non Blocking error : transfer could go on.
           Error is notified to user through user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered error callback*/
        huart->ErrorCallback(huart);
#else
        /*Call legacy weak error callback*/
        HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
        huart->ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;

  } /* End if some error occurs */

  /* Check current reception Mode :
     If Reception till IDLE event has been selected : */
  if ((huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      && ((isrflags & USART_ISR_IDLE) != 0U)
      && ((cr1its & USART_ISR_IDLE) != 0U))
  {
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_IDLEF);

#if defined(HAL_DMA_MODULE_ENABLED)
    /* Check if DMA mode is enabled in UART */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
    {
      /* DMA mode enabled */
      /* Check received length : If all expected data are received, do nothing,
         (DMA cplt callback will be called).
         Otherwise, if at least one data has already been received, IDLE event is to be notified to user */
      uint16_t nb_remaining_rx_data = (uint16_t) __HAL_DMA_GET_COUNTER(huart->hdmarx);
      if ((nb_remaining_rx_data > 0U)
          && (nb_remaining_rx_data < huart->RxXferSize))
      {
        /* Reception is not complete */
        huart->RxXferCount = nb_remaining_rx_data;

        /* In Normal mode, end DMA xfer and HAL UART Rx process*/
        if (huart->hdmarx->Mode != DMA_LINKEDLIST_CIRCULAR)
        {
          /* Disable PE and ERR (Frame error, noise error, overrun error) interrupts */
          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

#if !defined(USART_DMAREQUESTS_SW_WA)
          /* Disable the DMA transfer for the receiver request by resetting the DMAR bit
             in the UART CR3 register */
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

#endif /* !USART_DMAREQUESTS_SW_WA */
          /* At end of Rx process, restore huart->RxState to Ready */
          huart->RxState = HAL_UART_STATE_READY;
          huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

          /* Last bytes received, so no need as the abort is immediate */
          (void)HAL_DMA_Abort(huart->hdmarx);
        }

        /* Initialize type of RxEvent that correspond to RxEvent callback execution;
           In this case, Rx Event type is Idle Event */
        huart->RxEventType = HAL_UART_RXEVENT_IDLE;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx Event callback*/
        huart->RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#else
        /*Call legacy weak Rx Event callback*/
        HAL_UARTEx_RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
      }
      else
      {
        /* If DMA is in Circular mode, Idle event is to be reported to user
           even if occurring after a Transfer Complete event from DMA */
        if (nb_remaining_rx_data == huart->RxXferSize)
        {
          if (huart->hdmarx->Mode == DMA_LINKEDLIST_CIRCULAR)
          {
            /* Initialize type of RxEvent that correspond to RxEvent callback execution;
               In this case, Rx Event type is Idle Event */
            huart->RxEventType = HAL_UART_RXEVENT_IDLE;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
            /*Call registered Rx Event callback*/
            huart->RxEventCallback(huart, huart->RxXferSize);
#else
            /*Call legacy weak Rx Event callback*/
            HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
          }
        }
      }
      return;
    }
    else
    {
#endif /* HAL_DMA_MODULE_ENABLED */
      /* DMA mode not enabled */
      /* Check received length : If all expected data are received, do nothing.
         Otherwise, if at least one data has already been received, IDLE event is to be notified to user */
      uint16_t nb_rx_data = huart->RxXferSize - huart->RxXferCount;
      if ((huart->RxXferCount > 0U)
          && (nb_rx_data > 0U))
      {
        /* Disable the UART Parity Error Interrupt and RXNE interrupts */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));

        /* Disable the UART Error Interrupt:(Frame error, noise error, overrun error) and RX FIFO Threshold interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));

        /* Rx process is completed, restore huart->RxState to Ready */
        huart->RxState = HAL_UART_STATE_READY;
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        /* Clear RxISR function pointer */
        huart->RxISR = NULL;

        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        /* Initialize type of RxEvent that correspond to RxEvent callback execution;
           In this case, Rx Event type is Idle Event */
        huart->RxEventType = HAL_UART_RXEVENT_IDLE;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx complete callback*/
        huart->RxEventCallback(huart, nb_rx_data);
#else
        /*Call legacy weak Rx Event callback*/
        HAL_UARTEx_RxEventCallback(huart, nb_rx_data);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
      }
      return;
#if defined(HAL_DMA_MODULE_ENABLED)
    }
#endif /* HAL_DMA_MODULE_ENABLED */
  }

  /* UART wakeup from Stop mode interrupt occurred ---------------------------*/
  if (((isrflags & USART_ISR_WUF) != 0U) && ((cr3its & USART_CR3_WUFIE) != 0U))
  {
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_WUF);

    /* UART Rx state is not reset as a reception process might be ongoing.
       If UART handle state fields need to be reset to READY, this could be done in Wakeup callback */

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* Call registered Wakeup Callback */
    huart->WakeupCallback(huart);
#else
    /* Call legacy weak Wakeup Callback */
    HAL_UARTEx_WakeupCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    return;
  }

  /* UART in mode Transmitter ------------------------------------------------*/
  if (((isrflags & USART_ISR_TXE_TXFNF) != 0U)
      && (((cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U)
          || ((cr3its & USART_CR3_TXFTIE) != 0U)))
  {
    if (huart->TxISR != NULL)
    {
      huart->TxISR(huart);
    }
    return;
  }

  /* UART in mode Transmitter (transmission end) -----------------------------*/
  if (((isrflags & USART_ISR_TC) != 0U) && ((cr1its & USART_CR1_TCIE) != 0U))
  {
    UART_EndTransmit_IT(huart);
    return;
  }

  /* UART TX Fifo Empty occurred ----------------------------------------------*/
  if (((isrflags & USART_ISR_TXFE) != 0U) && ((cr1its & USART_CR1_TXFEIE) != 0U))
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* Call registered Tx Fifo Empty Callback */
    huart->TxFifoEmptyCallback(huart);
#else
    /* Call legacy weak Tx Fifo Empty Callback */
    HAL_UARTEx_TxFifoEmptyCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    return;
  }

  /* UART RX Fifo Full occurred ----------------------------------------------*/
  if (((isrflags & USART_ISR_RXFF) != 0U) && ((cr1its & USART_CR1_RXFFIE) != 0U))
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* Call registered Rx Fifo Full Callback */
    huart->RxFifoFullCallback(huart);
#else
    /* Call legacy weak Rx Fifo Full Callback */
    HAL_UARTEx_RxFifoFullCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    return;
  }
}

/**
  * @brief Tx Transfer completed callback.
  * @param huart UART handle.
  * @retval None
  */
__weak void HAL_UART_TxCpltCallback()
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_TxCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  Tx Half Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_TxHalfCpltCallback()
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxHalfCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_RxCpltCallback()
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  Rx Half Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_RxHalfCpltCallback()
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxHalfCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  UART error callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_ErrorCallback()
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_ErrorCallback can be implemented in the user file.
   */
}

/**
  * @brief  UART Abort Complete callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_AbortCpltCallback()
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_AbortCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  UART Abort Complete callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_AbortTransmitCpltCallback()
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_AbortTransmitCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  UART Abort Receive Complete callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_AbortReceiveCpltCallback()
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_AbortReceiveCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
  * @param  huart UART handle
  * @param  Size  Number of data available in application reception buffer (indicates a position in
  *               reception buffer until which, data are available)
  * @retval None
  */
__weak void HAL_UARTEx_RxEventCallback(uint16_t Size)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  UNUSED(Size);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UARTEx_RxEventCallback can be implemented in the user file.
   */
}

/**
  * @}
  */

/** @defgroup UART_Exported_Functions_Group3 Peripheral Control functions
  *  @brief   UART control functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the UART.
     (+) HAL_UART_ReceiverTimeout_Config() API allows to configure the receiver timeout value on the fly
     (+) HAL_UART_EnableReceiverTimeout() API enables the receiver timeout feature
     (+) HAL_UART_DisableReceiverTimeout() API disables the receiver timeout feature
     (+) HAL_MultiProcessor_EnableMuteMode() API enables mute mode
     (+) HAL_MultiProcessor_DisableMuteMode() API disables mute mode
     (+) HAL_MultiProcessor_EnterMuteMode() API enters mute mode
     (+) UART_SetConfig() API configures the UART peripheral
     (+) UART_AdvFeatureConfig() API optionally configures the UART advanced features
     (+) UART_CheckIdleState() API ensures that TEACK and/or REACK are set after initialization
     (+) HAL_HalfDuplex_EnableTransmitter() API disables receiver and enables transmitter
     (+) HAL_HalfDuplex_EnableReceiver() API disables transmitter and enables receiver
     (+) HAL_LIN_SendBreak() API transmits the break characters
@endverbatim
  * @{
  */

/**
  * @brief  Update on the fly the receiver timeout value in RTOR register.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *                    the configuration information for the specified UART module.
  * @param  TimeoutValue receiver timeout value in number of baud blocks. The timeout
  *                     value must be less or equal to 0x0FFFFFFFF.
  * @retval None
  */
void HAL_UART_ReceiverTimeout_Config(uint32_t TimeoutValue)
{
  if (!(IS_LPUART_INSTANCE(huart->Instance)))
  {
    assert_param(IS_UART_RECEIVER_TIMEOUT_VALUE(TimeoutValue));
    MODIFY_REG(huart->Instance->RTOR, USART_RTOR_RTO, TimeoutValue);
  }
}

/**
  * @brief  Enable the UART receiver timeout feature.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *                    the configuration information for the specified UART module.
  * @retval HAL status
  */
status HAL_UART_EnableReceiverTimeout()
{
  if (!(IS_LPUART_INSTANCE(huart->Instance)))
  {
    if (huart->gState == HAL_UART_STATE_READY)
    {
      /* Process Locked */
      __HAL_LOCK(huart);

      huart->gState = HAL_UART_STATE_BUSY;

      /* Set the USART RTOEN bit */
      SET_BIT(huart->Instance->CR2, USART_CR2_RTOEN);

      huart->gState = HAL_UART_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(huart);

      return HAL_OK;
    }
    else
    {
      return HAL_BUSY;
    }
  }
  else
  {
    return HAL_ERROR;
  }
}

/**
  * @brief  Disable the UART receiver timeout feature.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *                    the configuration information for the specified UART module.
  * @retval HAL status
  */
status HAL_UART_DisableReceiverTimeout()
{
  if (!(IS_LPUART_INSTANCE(huart->Instance)))
  {
    if (huart->gState == HAL_UART_STATE_READY)
    {
      /* Process Locked */
      __HAL_LOCK(huart);

      huart->gState = HAL_UART_STATE_BUSY;

      /* Clear the USART RTOEN bit */
      CLEAR_BIT(huart->Instance->CR2, USART_CR2_RTOEN);

      huart->gState = HAL_UART_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(huart);

      return HAL_OK;
    }
    else
    {
      return HAL_BUSY;
    }
  }
  else
  {
    return HAL_ERROR;
  }
}

/**
  * @brief  Enable UART in mute mode (does not mean UART enters mute mode;
  *         to enter mute mode, HAL_MultiProcessor_EnterMuteMode() API must be called).
  * @param  huart UART handle.
  * @retval HAL status
  */
status HAL_MultiProcessor_EnableMuteMode()
{
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Enable USART mute mode by setting the MME bit in the CR1 register */
  ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_MME);

  huart->gState = HAL_UART_STATE_READY;

  return (UART_CheckIdleState(huart));
}

/**
  * @brief  Disable UART mute mode (does not mean the UART actually exits mute mode
  *         as it may not have been in mute mode at this very moment).
  * @param  huart UART handle.
  * @retval HAL status
  */
status HAL_MultiProcessor_DisableMuteMode()
{
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable USART mute mode by clearing the MME bit in the CR1 register */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_MME);

  huart->gState = HAL_UART_STATE_READY;

  return (UART_CheckIdleState(huart));
}

/**
  * @brief Enter UART mute mode (means UART actually enters mute mode).
  * @note  To exit from mute mode, HAL_MultiProcessor_DisableMuteMode() API must be called.
  * @param huart UART handle.
  * @retval None
  */
void HAL_MultiProcessor_EnterMuteMode()
{
  __HAL_UART_SEND_REQ(huart, UART_MUTE_MODE_REQUEST);
}

/**
  * @brief  Enable the UART transmitter and disable the UART receiver.
  * @param  huart UART handle.
  * @retval HAL status
  */
status HAL_HalfDuplex_EnableTransmitter()
{
  __HAL_LOCK(huart);
  huart->gState = HAL_UART_STATE_BUSY;

  /* Clear TE and RE bits */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TE | USART_CR1_RE));

  /* Enable the USART's transmit interface by setting the TE bit in the USART CR1 register */
  ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TE);

  huart->gState = HAL_UART_STATE_READY;

  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  Enable the UART receiver and disable the UART transmitter.
  * @param  huart UART handle.
  * @retval HAL status.
  */
status HAL_HalfDuplex_EnableReceiver()
{
  __HAL_LOCK(huart);
  huart->gState = HAL_UART_STATE_BUSY;

  /* Clear TE and RE bits */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TE | USART_CR1_RE));

  /* Enable the USART's receive interface by setting the RE bit in the USART CR1 register */
  ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_RE);

  huart->gState = HAL_UART_STATE_READY;

  __HAL_UNLOCK(huart);

  return HAL_OK;
}


/**
  * @brief  Transmit break characters.
  * @param  huart UART handle.
  * @retval HAL status
  */
status HAL_LIN_SendBreak()
{
  /* Check the parameters */
  assert_param(IS_UART_LIN_INSTANCE(huart->Instance));

  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Send break characters */
  __HAL_UART_SEND_REQ(huart, UART_SENDBREAK_REQUEST);

  huart->gState = HAL_UART_STATE_READY;

  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup UART_Exported_Functions_Group4 Peripheral State and Error functions
  *  @brief   UART Peripheral State functions
  *
@verbatim
  ==============================================================================
            ##### Peripheral State and Error functions #####
  ==============================================================================
    [..]
    This subsection provides functions allowing to :
      (+) Return the UART handle state.
      (+) Return the UART handle error code

@endverbatim
  * @{
  */

/**
  * @brief Return the UART handle state.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *               the configuration information for the specified UART.
  * @retval HAL state
  */
HAL_UART_StateTypeDef HAL_UART_GetState(const UART_HandleTypeDef *huart)
{
  uint32_t temp1;
  uint32_t temp2;
  temp1 = huart->gState;
  temp2 = huart->RxState;

  return (HAL_UART_StateTypeDef)(temp1 | temp2);
}

/**
  * @brief  Return the UART handle error code.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *               the configuration information for the specified UART.
  * @retval UART Error Code
  */
uint32_t HAL_UART_GetError(const UART_HandleTypeDef *huart)
{
  return huart->ErrorCode;
}
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup UART_Private_Functions UART Private Functions
  * @{
  */

/**
  * @brief  Initialize the callbacks to their default values.
  * @param  huart UART handle.
  * @retval none
  */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
void UART_InitCallbacksToDefault()
{
  /* Init the UART Callback settings */
  huart->TxHalfCpltCallback        = HAL_UART_TxHalfCpltCallback;        /* Legacy weak TxHalfCpltCallback        */
  huart->TxCpltCallback            = HAL_UART_TxCpltCallback;            /* Legacy weak TxCpltCallback            */
  huart->RxHalfCpltCallback        = HAL_UART_RxHalfCpltCallback;        /* Legacy weak RxHalfCpltCallback        */
  huart->RxCpltCallback            = HAL_UART_RxCpltCallback;            /* Legacy weak RxCpltCallback            */
  huart->ErrorCallback             = HAL_UART_ErrorCallback;             /* Legacy weak ErrorCallback             */
  huart->AbortCpltCallback         = HAL_UART_AbortCpltCallback;         /* Legacy weak AbortCpltCallback         */
  huart->AbortTransmitCpltCallback = HAL_UART_AbortTransmitCpltCallback; /* Legacy weak AbortTransmitCpltCallback */
  huart->AbortReceiveCpltCallback  = HAL_UART_AbortReceiveCpltCallback;  /* Legacy weak AbortReceiveCpltCallback  */
  huart->WakeupCallback            = HAL_UARTEx_WakeupCallback;          /* Legacy weak WakeupCallback            */
  huart->RxFifoFullCallback        = HAL_UARTEx_RxFifoFullCallback;      /* Legacy weak RxFifoFullCallback        */
  huart->TxFifoEmptyCallback       = HAL_UARTEx_TxFifoEmptyCallback;     /* Legacy weak TxFifoEmptyCallback       */
  huart->RxEventCallback           = HAL_UARTEx_RxEventCallback;         /* Legacy weak RxEventCallback           */

}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

/**
  * @brief Configure the UART peripheral.
  * @param huart UART handle.
  * @retval HAL status
  */
status UART_SetConfig()
{
  uint32_t tmpreg;
  uint16_t brrtemp;
  uint32_t clocksource;
  uint32_t usartdiv;
  status ret               = HAL_OK;
  uint32_t lpuart_ker_ck_pres;
  uint32_t pclk;

  /* Check the parameters */
  assert_param(IS_UART_BAUDRATE(params.BaudRate));
  assert_param(IS_UART_WORD_LENGTH(params.WordLength));
  if (UART_INSTANCE_LOWPOWER(huart))
  {
    assert_param(IS_LPUART_STOPBITS(params.StopBits));
  }
  else
  {
    assert_param(IS_UART_STOPBITS(params.StopBits));
    assert_param(IS_UART_ONE_BIT_SAMPLE(params.OneBitSampling));
  }

  assert_param(IS_UART_PARITY(params.Parity));
  assert_param(IS_UART_MODE(params.Mode));
  assert_param(IS_UART_HARDWARE_FLOW_CONTROL(params.HwFlowCtl));
  assert_param(IS_UART_OVERSAMPLING(params.OverSampling));
  assert_param(IS_UART_PRESCALER(params.ClockPrescaler));

  /*-------------------------- USART CR1 Configuration -----------------------*/
  /* Clear M, PCE, PS, TE, RE and OVER8 bits and configure
  *  the UART Word Length, Parity, Mode and oversampling:
  *  set the M bits according to params.WordLength value
  *  set PCE and PS bits according to params.Parity value
  *  set TE and RE bits according to params.Mode value
  *  set OVER8 bit according to params.OverSampling value */
  tmpreg = (uint32_t)params.WordLength | params.Parity | params.Mode | params.OverSampling ;
  MODIFY_REG(huart->Instance->CR1, USART_CR1_FIELDS, tmpreg);

  /*-------------------------- USART CR2 Configuration -----------------------*/
  /* Configure the UART Stop Bits: Set STOP[13:12] bits according
  * to params.StopBits value */
  MODIFY_REG(huart->Instance->CR2, USART_CR2_STOP, params.StopBits);

  /*-------------------------- USART CR3 Configuration -----------------------*/
  /* Configure
  * - UART HardWare Flow Control: set CTSE and RTSE bits according
  *   to params.HwFlowCtl value
  * - one-bit sampling method versus three samples' majority rule according
  *   to params.OneBitSampling (not applicable to LPUART) */
  tmpreg = (uint32_t)params.HwFlowCtl;

  if (!(UART_INSTANCE_LOWPOWER(huart)))
  {
    tmpreg |= params.OneBitSampling;
  }
  MODIFY_REG(huart->Instance->CR3, USART_CR3_FIELDS, tmpreg);

  /*-------------------------- USART PRESC Configuration -----------------------*/
  /* Configure
  * - UART Clock Prescaler : set PRESCALER according to params.ClockPrescaler value */
  MODIFY_REG(huart->Instance->PRESC, USART_PRESC_PRESCALER, params.ClockPrescaler);

  /*-------------------------- USART BRR Configuration -----------------------*/
  UART_GETCLOCKSOURCE(huart, clocksource);

  /* Check LPUART instance */
  if (UART_INSTANCE_LOWPOWER(huart))
  {
    /* Retrieve frequency clock */
    pclk = HAL_RCCEx_GetPeriphCLKFreq(clocksource);

    /* If proper clock source reported */
    if (pclk != 0U)
    {
      /* Compute clock after Prescaler */
      lpuart_ker_ck_pres = (pclk / UARTPrescTable[params.ClockPrescaler]);

      /* Ensure that Frequency clock is in the range [3 * baudrate, 4096 * baudrate] */
      if ((lpuart_ker_ck_pres < (3U * params.BaudRate)) ||
          (lpuart_ker_ck_pres > (4096U * params.BaudRate)))
      {
        ret = HAL_ERROR;
      }
      else
      {
        /* Check computed UsartDiv value is in allocated range
           (it is forbidden to write values lower than 0x300 in the LPUART_BRR register) */
        usartdiv = (uint32_t)(UART_DIV_LPUART(pclk, params.BaudRate, params.ClockPrescaler));
        if ((usartdiv >= LPUART_BRR_MIN) && (usartdiv <= LPUART_BRR_MAX))
        {
          huart->Instance->BRR = usartdiv;
        }
        else
        {
          ret = HAL_ERROR;
        }
      } /* if ( (lpuart_ker_ck_pres < (3 * params.BaudRate) ) ||
                (lpuart_ker_ck_pres > (4096 * params.BaudRate) )) */
    } /* if (pclk != 0) */
  }
  /* Check UART Over Sampling to set Baud Rate Register */
  else if (params.OverSampling == UART_OVERSAMPLING_8)
  {
    pclk = HAL_RCCEx_GetPeriphCLKFreq(clocksource);

    /* USARTDIV must be greater than or equal to 0d16 */
    if (pclk != 0U)
    {
      usartdiv = (uint32_t)(UART_DIV_SAMPLING8(pclk, params.BaudRate, params.ClockPrescaler));
      if ((usartdiv >= UART_BRR_MIN) && (usartdiv <= UART_BRR_MAX))
      {
        brrtemp = (uint16_t)(usartdiv & 0xFFF0U);
        brrtemp |= (uint16_t)((usartdiv & (uint16_t)0x000FU) >> 1U);
        huart->Instance->BRR = brrtemp;
      }
      else
      {
        ret = HAL_ERROR;
      }
    }
  }
  else
  {
    pclk = HAL_RCCEx_GetPeriphCLKFreq(clocksource);

    if (pclk != 0U)
    {
      /* USARTDIV must be greater than or equal to 0d16 */
      usartdiv = (uint32_t)(UART_DIV_SAMPLING16(pclk, params.BaudRate, params.ClockPrescaler));
      if ((usartdiv >= UART_BRR_MIN) && (usartdiv <= UART_BRR_MAX))
      {
        huart->Instance->BRR = (uint16_t)usartdiv;
      }
      else
      {
        ret = HAL_ERROR;
      }
    }
  }

  /* Initialize the number of data to process during RX/TX ISR execution */
  huart->NbTxDataToProcess = 1;
  huart->NbRxDataToProcess = 1;

  /* Clear ISR function pointers */
  huart->RxISR = NULL;
  huart->TxISR = NULL;

  return ret;
}

/**
  * @brief Configure the UART peripheral advanced features.
  * @param huart UART handle.
  * @retval None
  */
void UART_AdvFeatureConfig()
{
  /* Check whether the set of advanced features to configure is properly set */
  assert_param(IS_UART_ADVFEATURE_INIT(huart->AdvancedInit.AdvFeatureInit));

  /* if required, configure RX/TX pins swap */
  if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_SWAP_INIT))
  {
    assert_param(IS_UART_ADVFEATURE_SWAP(huart->AdvancedInit.Swap));
    MODIFY_REG(huart->Instance->CR2, USART_CR2_SWAP, huart->AdvancedInit.Swap);
  }

  /* if required, configure TX pin active level inversion */
  if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_TXINVERT_INIT))
  {
    assert_param(IS_UART_ADVFEATURE_TXINV(huart->AdvancedInit.TxPinLevelInvert));
    MODIFY_REG(huart->Instance->CR2, USART_CR2_TXINV, huart->AdvancedInit.TxPinLevelInvert);
  }

  /* if required, configure RX pin active level inversion */
  if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_RXINVERT_INIT))
  {
    assert_param(IS_UART_ADVFEATURE_RXINV(huart->AdvancedInit.RxPinLevelInvert));
    MODIFY_REG(huart->Instance->CR2, USART_CR2_RXINV, huart->AdvancedInit.RxPinLevelInvert);
  }

  /* if required, configure data inversion */
  if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_DATAINVERT_INIT))
  {
    assert_param(IS_UART_ADVFEATURE_DATAINV(huart->AdvancedInit.DataInvert));
    MODIFY_REG(huart->Instance->CR2, USART_CR2_DATAINV, huart->AdvancedInit.DataInvert);
  }

  /* if required, configure RX overrun detection disabling */
  if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_RXOVERRUNDISABLE_INIT))
  {
    assert_param(IS_UART_OVERRUN(huart->AdvancedInit.OverrunDisable));
    MODIFY_REG(huart->Instance->CR3, USART_CR3_OVRDIS, huart->AdvancedInit.OverrunDisable);
  }

#if defined(HAL_DMA_MODULE_ENABLED)
  /* if required, configure DMA disabling on reception error */
  if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_DMADISABLEONERROR_INIT))
  {
    assert_param(IS_UART_ADVFEATURE_DMAONRXERROR(huart->AdvancedInit.DMADisableonRxError));
    MODIFY_REG(huart->Instance->CR3, USART_CR3_DDRE, huart->AdvancedInit.DMADisableonRxError);
  }
#endif /* HAL_DMA_MODULE_ENABLED */

  /* if required, configure auto Baud rate detection scheme */
  if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_AUTOBAUDRATE_INIT))
  {
    assert_param(IS_USART_AUTOBAUDRATE_DETECTION_INSTANCE(huart->Instance));
    assert_param(IS_UART_ADVFEATURE_AUTOBAUDRATE(huart->AdvancedInit.AutoBaudRateEnable));
    MODIFY_REG(huart->Instance->CR2, USART_CR2_ABREN, huart->AdvancedInit.AutoBaudRateEnable);
    /* set auto Baudrate detection parameters if detection is enabled */
    if (huart->AdvancedInit.AutoBaudRateEnable == UART_ADVFEATURE_AUTOBAUDRATE_ENABLE)
    {
      assert_param(IS_UART_ADVFEATURE_AUTOBAUDRATEMODE(huart->AdvancedInit.AutoBaudRateMode));
      MODIFY_REG(huart->Instance->CR2, USART_CR2_ABRMODE, huart->AdvancedInit.AutoBaudRateMode);
    }
  }

  /* if required, configure MSB first on communication line */
  if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_MSBFIRST_INIT))
  {
    assert_param(IS_UART_ADVFEATURE_MSBFIRST(huart->AdvancedInit.MSBFirst));
    MODIFY_REG(huart->Instance->CR2, USART_CR2_MSBFIRST, huart->AdvancedInit.MSBFirst);
  }
}

/**
  * @brief Check the UART Idle State.
  * @param huart UART handle.
  * @retval HAL status
  */
status UART_CheckIdleState()
{
  uint32_t tickstart;

  /* Initialize the UART ErrorCode */
  huart->ErrorCode = HAL_UART_ERROR_NONE;

  /* Init tickstart for timeout management */
  tickstart = HAL_GetTick();

  /* Check if the Transmitter is enabled */
  if ((huart->Instance->CR1 & USART_CR1_TE) == USART_CR1_TE)
  {
    /* Wait until TEACK flag is set */
    if (UART_WaitOnFlagUntilTimeout(huart, USART_ISR_TEACK, RESET, tickstart, HAL_UART_TIMEOUT_VALUE) != HAL_OK)
    {
      /* Disable TXE interrupt for the interrupt process */
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE_TXFNFIE));

      huart->gState = HAL_UART_STATE_READY;

      __HAL_UNLOCK(huart);

      /* Timeout occurred */
      return HAL_TIMEOUT;
    }
  }

  /* Check if the Receiver is enabled */
  if ((huart->Instance->CR1 & USART_CR1_RE) == USART_CR1_RE)
  {
    /* Wait until REACK flag is set */
    if (UART_WaitOnFlagUntilTimeout(huart, USART_ISR_REACK, RESET, tickstart, HAL_UART_TIMEOUT_VALUE) != HAL_OK)
    {
      /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error)
      interrupts for the interrupt process */
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
      ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

      huart->RxState = HAL_UART_STATE_READY;

      __HAL_UNLOCK(huart);

      /* Timeout occurred */
      return HAL_TIMEOUT;
    }
  }

  /* Initialize the UART State */
  huart->gState = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  This function handles UART Communication Timeout. It waits
  *                  until a flag is no longer in the specified status.
  * @param huart     UART handle.
  * @param Flag      Specifies the UART flag to check
  * @param Status    The actual Flag status (SET or RESET)
  * @param Tickstart Tick start value
  * @param Timeout   Timeout duration
  * @retval HAL status
  */
status UART_WaitOnFlagUntilTimeout(uint32_t Flag, FlagStatus Status,
                                              uint32_t Tickstart, uint32_t Timeout)
{
  /* Wait until flag is set */
  while ((__HAL_UART_GET_FLAG(huart, Flag) ? SET : RESET) == Status)
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {

        return HAL_TIMEOUT;
      }

      if ((READ_BIT(huart->Instance->CR1, USART_CR1_RE) != 0U) && (Flag != UART_FLAG_TXE) && (Flag != UART_FLAG_TC))
      {
        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) == SET)
        {
          /* Clear Overrun Error flag*/
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);

          /* Blocking error : transfer is aborted
          Set the UART state ready to be able to start again the process,
          Disable Rx Interrupts if ongoing */
          UART_EndRxTransfer(huart);

          huart->ErrorCode = HAL_UART_ERROR_ORE;

          /* Process Unlocked */
          __HAL_UNLOCK(huart);

          return HAL_ERROR;
        }
        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RTOF) == SET)
        {
          /* Clear Receiver Timeout flag*/
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_RTOF);

          /* Blocking error : transfer is aborted
          Set the UART state ready to be able to start again the process,
          Disable Rx Interrupts if ongoing */
          UART_EndRxTransfer(huart);

          huart->ErrorCode = HAL_UART_ERROR_RTO;

          /* Process Unlocked */
          __HAL_UNLOCK(huart);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Start Receive operation in interrupt mode.
  * @note   This function could be called by all HAL UART API providing reception in Interrupt mode.
  * @note   When calling this function, parameters validity is considered as already checked,
  *         i.e. Rx State, buffer address, ...
  *         UART Handle is assumed as Locked.
  * @param  huart UART handle.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be received.
  * @retval HAL status
  */
status UART_Start_Receive_IT(uint8_t *pData, uint16_t Size)
{
  huart->pRxBuffPtr  = pData;
  huart->RxXferSize  = Size;
  huart->RxXferCount = Size;
  huart->RxISR       = NULL;

  /* Computation of UART mask to apply to RDR register */
  UART_MASK_COMPUTATION(huart);

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->RxState = HAL_UART_STATE_BUSY_RX;

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* Configure Rx interrupt processing */
  if ((huart->FifoMode == UART_FIFOMODE_ENABLE) && (Size >= huart->NbRxDataToProcess))
  {
    /* Set the Rx ISR function pointer according to the data word length */
    if ((params.WordLength == UART_WORDLENGTH_9B) && (params.Parity == UART_PARITY_NONE))
    {
      huart->RxISR = UART_RxISR_16BIT_FIFOEN;
    }
    else
    {
      huart->RxISR = UART_RxISR_8BIT_FIFOEN;
    }

    /* Enable the UART Parity Error interrupt and RX FIFO Threshold interrupt */
    if (params.Parity != UART_PARITY_NONE)
    {
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    }
    ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_RXFTIE);
  }
  else
  {
    /* Set the Rx ISR function pointer according to the data word length */
    if ((params.WordLength == UART_WORDLENGTH_9B) && (params.Parity == UART_PARITY_NONE))
    {
      huart->RxISR = UART_RxISR_16BIT;
    }
    else
    {
      huart->RxISR = UART_RxISR_8BIT;
    }

    /* Enable the UART Parity Error interrupt and Data Register Not Empty interrupt */
    if (params.Parity != UART_PARITY_NONE)
    {
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);
    }
    else
    {
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_RXNEIE_RXFNEIE);
    }
  }
  return HAL_OK;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Start Receive operation in DMA mode.
  * @note   This function could be called by all HAL UART API providing reception in DMA mode.
  * @note   When calling this function, parameters validity is considered as already checked,
  *         i.e. Rx State, buffer address, ...
  *         UART Handle is assumed as Locked.
  * @param  huart UART handle.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be received.
  * @retval HAL status
  */
status UART_Start_Receive_DMA(uint8_t *pData, uint16_t Size)
{
  status status;
  uint16_t nbByte = Size;

  huart->pRxBuffPtr = pData;
  huart->RxXferSize = Size;

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->RxState = HAL_UART_STATE_BUSY_RX;

  if (huart->hdmarx != NULL)
  {
    /* Set the UART DMA transfer complete callback */
    huart->hdmarx->XferCpltCallback = UART_DMAReceiveCplt;

    /* Set the UART DMA Half transfer complete callback */
    huart->hdmarx->XferHalfCpltCallback = UART_DMARxHalfCplt;

    /* Set the DMA error callback */
    huart->hdmarx->XferErrorCallback = UART_DMAError;

    /* Set the DMA abort callback */
    huart->hdmarx->XferAbortCallback = NULL;

    /* In case of 9bits/No Parity transfer, pData buffer provided as input parameter
       should be aligned on a u16 frontier, so nbByte should be equal to Size * 2 */
    if ((params.WordLength == UART_WORDLENGTH_9B) && (params.Parity == UART_PARITY_NONE))
    {
      nbByte = Size * 2U;
    }

    /* Check linked list mode */
    if ((huart->hdmarx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      if ((huart->hdmarx->LinkedListQueue != NULL) && (huart->hdmarx->LinkedListQueue->Head != NULL))
      {
        /* Set DMA data size */
        huart->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = nbByte;

        /* Set DMA source address */
        huart->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] =
          (uint32_t)&huart->Instance->RDR;

        /* Set DMA destination address */
        huart->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)huart->pRxBuffPtr;

        /* Enable the UART receive DMA channel */
        status = HAL_DMAEx_List_Start_IT(huart->hdmarx);
      }
      else
      {
        /* Update status */
        status = HAL_ERROR;
      }
    }
    else
    {
      /* Enable the UART receive DMA channel */
      status = HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->RDR, (uint32_t)huart->pRxBuffPtr, nbByte);
    }

    if (status != HAL_OK)
    {
      /* Set error code to DMA */
      huart->ErrorCode = HAL_UART_ERROR_DMA;

      /* Restore huart->RxState to ready */
      huart->RxState = HAL_UART_STATE_READY;

      return HAL_ERROR;
    }
  }

  /* Enable the UART Parity Error Interrupt */
  if (params.Parity != UART_PARITY_NONE)
  {
    ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
  }

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* Enable the DMA transfer for the receiver request by setting the DMAR bit
  in the UART CR3 register */
  ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

  return HAL_OK;
}


/**
  * @brief  End ongoing Tx transfer on UART peripheral (following error detection or Transmit completion).
  * @param  huart UART handle.
  * @retval None
  */
static void UART_EndTxTransfer()
{
  /* Disable TXEIE, TCIE, TXFT interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE_TXFNFIE | USART_CR1_TCIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, (USART_CR3_TXFTIE));

  /* At end of Tx process, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;
}
#endif /* HAL_DMA_MODULE_ENABLED */


/**
  * @brief  End ongoing Rx transfer on UART peripheral (following error detection or Reception completion).
  * @param  huart UART handle.
  * @retval None
  */
static void UART_EndRxTransfer()
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));

  /* In case of reception waiting for IDLE event, disable also the IDLE IE interrupt source */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
  }

  /* At end of Rx process, restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  /* Reset RxIsr function pointer */
  huart->RxISR = NULL;
}


#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief DMA UART transmit process complete callback.
  * @param hdma DMA handle.
  * @retval None
  */
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma->Parent);

  /* Check if DMA in circular mode */
  if (hdma->Mode != DMA_LINKEDLIST_CIRCULAR)
  {
    huart->TxXferCount = 0U;

#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the DMA transfer for transmit request by resetting the DMAT bit
       in the UART CR3 register */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* Enable the UART Transmit Complete Interrupt */
    ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
  }
  /* DMA Circular mode */
  else
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Tx complete callback*/
    huart->TxCpltCallback(huart);
#else
    /*Call legacy weak Tx complete callback*/
    HAL_UART_TxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
}

/**
  * @brief DMA UART transmit process half complete callback.
  * @param hdma DMA handle.
  * @retval None
  */
static void UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma->Parent);

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered Tx Half complete callback*/
  huart->TxHalfCpltCallback(huart);
#else
  /*Call legacy weak Tx Half complete callback*/
  HAL_UART_TxHalfCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief DMA UART receive process complete callback.
  * @param hdma DMA handle.
  * @retval None
  */
static void UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma->Parent);

  /* Check if DMA in circular mode */
  if (hdma->Mode != DMA_LINKEDLIST_CIRCULAR)
  {
    huart->RxXferCount = 0U;

    /* Disable PE and ERR (Frame error, noise error, overrun error) interrupts */
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

#if !defined(USART_DMAREQUESTS_SW_WA)
    /* Disable the DMA transfer for the receiver request by resetting the DMAR bit
       in the UART CR3 register */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

#endif /* !USART_DMAREQUESTS_SW_WA */
    /* At end of Rx process, restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;

    /* If Reception till IDLE event has been selected, Disable IDLE Interrupt */
    if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
    {
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
    }
  }

  /* Initialize type of RxEvent that correspond to RxEvent callback execution;
     In this case, Rx Event type is Transfer Complete */
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  /* Check current reception Mode :
     If Reception till IDLE event has been selected : use Rx Event callback */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    huart->RxXferCount = 0;

    /* Check current nb of data still to be received on DMA side.
       DMA Normal mode, remaining nb of data will be 0
       DMA Circular mode, remaining nb of data is reset to RxXferSize */
    uint16_t nb_remaining_rx_data = (uint16_t) __HAL_DMA_GET_COUNTER(hdma);
    if (nb_remaining_rx_data < huart->RxXferSize)
    {
      /* Update nb of remaining data */
      huart->RxXferCount = nb_remaining_rx_data;
    }

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Rx Event callback*/
    huart->RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#else
    /*Call legacy weak Rx Event callback*/
    HAL_UARTEx_RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
  else
  {
    /* In other cases : use Rx Complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Rx complete callback*/
    huart->RxCpltCallback(huart);
#else
    /*Call legacy weak Rx complete callback*/
    HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
}

/**
  * @brief DMA UART receive process half complete callback.
  * @param hdma DMA handle.
  * @retval None
  */
static void UART_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma->Parent);

  /* Initialize type of RxEvent that correspond to RxEvent callback execution;
     In this case, Rx Event type is Half Transfer */
  huart->RxEventType = HAL_UART_RXEVENT_HT;

  /* Check current reception Mode :
     If Reception till IDLE event has been selected : use Rx Event callback */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    huart->RxXferCount = huart->RxXferSize / 2U;

    /* Check current nb of data still to be received on DMA side. */
    uint16_t nb_remaining_rx_data = (uint16_t) __HAL_DMA_GET_COUNTER(hdma);
    if (nb_remaining_rx_data <= huart->RxXferSize)
    {
      /* Update nb of remaining data */
      huart->RxXferCount = nb_remaining_rx_data;
    }

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Rx Event callback*/
    huart->RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#else
    /*Call legacy weak Rx Event callback*/
    HAL_UARTEx_RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
  else
  {
    /* In other cases : use Rx Half Complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Rx Half complete callback*/
    huart->RxHalfCpltCallback(huart);
#else
    /*Call legacy weak Rx Half complete callback*/
    HAL_UART_RxHalfCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
}

/**
  * @brief DMA UART communication error callback.
  * @param hdma DMA handle.
  * @retval None
  */
static void UART_DMAError(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma->Parent);

  const HAL_UART_StateTypeDef gstate = huart->gState;
  const HAL_UART_StateTypeDef rxstate = huart->RxState;

  /* Stop UART DMA Tx request if ongoing */
  if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT)) &&
      (gstate == HAL_UART_STATE_BUSY_TX))
  {
    huart->TxXferCount = 0U;
    UART_EndTxTransfer(huart);
  }

  /* Stop UART DMA Rx request if ongoing */
  if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)) &&
      (rxstate == HAL_UART_STATE_BUSY_RX))
  {
    huart->RxXferCount = 0U;
    UART_EndRxTransfer(huart);
  }

  huart->ErrorCode |= HAL_UART_ERROR_DMA;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered error callback*/
  huart->ErrorCallback(huart);
#else
  /*Call legacy weak error callback*/
  HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA UART communication abort callback, when initiated by HAL services on Error
  *         (To be called at end of DMA Abort procedure following error occurrence).
  * @param  hdma DMA handle.
  * @retval None
  */
static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma->Parent);
  huart->RxXferCount = 0U;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered error callback*/
  huart->ErrorCallback(huart);
#else
  /*Call legacy weak error callback*/
  HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA UART Tx communication abort callback, when initiated by user
  *         (To be called at end of DMA Tx Abort procedure following user abort request).
  * @note   When this callback is executed, User Abort complete call back is called only if no
  *         Abort still ongoing for Rx DMA Handle.
  * @param  hdma DMA handle.
  * @retval None
  */
static void UART_DMATxAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma->Parent);

  huart->hdmatx->XferAbortCallback = NULL;

  /* Check if an Abort process is still ongoing */
  if (huart->hdmarx != NULL)
  {
    if (huart->hdmarx->XferAbortCallback != NULL)
    {
      return;
    }
  }

  /* No Abort process still ongoing : All DMA channels are aborted, call user Abort Complete callback */
  huart->TxXferCount = 0U;
  huart->RxXferCount = 0U;

  /* Reset errorCode */
  huart->ErrorCode = HAL_UART_ERROR_NONE;

  /* Clear the Error flags in the ICR register */
  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);

  /* Flush the whole TX FIFO (if needed) */
  if (huart->FifoMode == UART_FIFOMODE_ENABLE)
  {
    __HAL_UART_SEND_REQ(huart, UART_TXDATA_FLUSH_REQUEST);
  }

  /* Restore huart->gState and huart->RxState to Ready */
  huart->gState  = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /* Call registered Abort complete callback */
  huart->AbortCpltCallback(huart);
#else
  /* Call legacy weak Abort complete callback */
  HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}


/**
  * @brief  DMA UART Rx communication abort callback, when initiated by user
  *         (To be called at end of DMA Rx Abort procedure following user abort request).
  * @note   When this callback is executed, User Abort complete call back is called only if no
  *         Abort still ongoing for Tx DMA Handle.
  * @param  hdma DMA handle.
  * @retval None
  */
static void UART_DMARxAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma->Parent);

  huart->hdmarx->XferAbortCallback = NULL;

  /* Check if an Abort process is still ongoing */
  if (huart->hdmatx != NULL)
  {
    if (huart->hdmatx->XferAbortCallback != NULL)
    {
      return;
    }
  }

  /* No Abort process still ongoing : All DMA channels are aborted, call user Abort Complete callback */
  huart->TxXferCount = 0U;
  huart->RxXferCount = 0U;

  /* Reset errorCode */
  huart->ErrorCode = HAL_UART_ERROR_NONE;

  /* Clear the Error flags in the ICR register */
  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);

  /* Discard the received data */
  __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);

  /* Restore huart->gState and huart->RxState to Ready */
  huart->gState  = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /* Call registered Abort complete callback */
  huart->AbortCpltCallback(huart);
#else
  /* Call legacy weak Abort complete callback */
  HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}


/**
  * @brief  DMA UART Tx communication abort callback, when initiated by user by a call to
  *         HAL_UART_AbortTransmit_IT API (Abort only Tx transfer)
  *         (This callback is executed at end of DMA Tx Abort procedure following user abort request,
  *         and leads to user Tx Abort Complete callback execution).
  * @param  hdma DMA handle.
  * @retval None
  */
static void UART_DMATxOnlyAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma->Parent);

  huart->TxXferCount = 0U;

  /* Flush the whole TX FIFO (if needed) */
  if (huart->FifoMode == UART_FIFOMODE_ENABLE)
  {
    __HAL_UART_SEND_REQ(huart, UART_TXDATA_FLUSH_REQUEST);
  }

  /* Restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;

  /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /* Call registered Abort Transmit Complete Callback */
  huart->AbortTransmitCpltCallback(huart);
#else
  /* Call legacy weak Abort Transmit Complete Callback */
  HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA UART Rx communication abort callback, when initiated by user by a call to
  *         HAL_UART_AbortReceive_IT API (Abort only Rx transfer)
  *         (This callback is executed at end of DMA Rx Abort procedure following user abort request,
  *         and leads to user Rx Abort Complete callback execution).
  * @param  hdma DMA handle.
  * @retval None
  */
static void UART_DMARxOnlyAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  huart->RxXferCount = 0U;

  /* Clear the Error flags in the ICR register */
  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);

  /* Discard the received data */
  __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);

  /* Restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /* Call registered Abort Receive Complete Callback */
  huart->AbortReceiveCpltCallback(huart);
#else
  /* Call legacy weak Abort Receive Complete Callback */
  HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief TX interrupt handler for 7 or 8 bits data word length .
  * @note   Function is called under interruption only, once
  *         interruptions have been enabled by HAL_UART_Transmit_IT().
  * @param huart UART handle.
  * @retval None
  */
static void UART_TxISR_8BIT()
{
  /* Check that a Tx process is ongoing */
  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    if (huart->TxXferCount == 0U)
    {
      /* Disable the UART Transmit Data Register Empty Interrupt */
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);

      /* Enable the UART Transmit Complete Interrupt */
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
    }
    else
    {
      huart->Instance->TDR = (uint8_t)(*huart->pTxBuffPtr & (uint8_t)0xFF);
      huart->pTxBuffPtr++;
      huart->TxXferCount--;
    }
  }
}

/**
  * @brief TX interrupt handler for 9 bits data word length.
  * @note   Function is called under interruption only, once
  *         interruptions have been enabled by HAL_UART_Transmit_IT().
  * @param huart UART handle.
  * @retval None
  */
static void UART_TxISR_16BIT()
{
  const uint16_t *tmp;

  /* Check that a Tx process is ongoing */
  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    if (huart->TxXferCount == 0U)
    {
      /* Disable the UART Transmit Data Register Empty Interrupt */
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);

      /* Enable the UART Transmit Complete Interrupt */
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
    }
    else
    {
      tmp = (const uint16_t *) huart->pTxBuffPtr;
      huart->Instance->TDR = (((uint32_t)(*tmp)) & 0x01FFUL);
      huart->pTxBuffPtr += 2U;
      huart->TxXferCount--;
    }
  }
}

/**
  * @brief TX interrupt handler for 7 or 8 bits data word length and FIFO mode is enabled.
  * @note   Function is called under interruption only, once
  *         interruptions have been enabled by HAL_UART_Transmit_IT().
  * @param huart UART handle.
  * @retval None
  */
static void UART_TxISR_8BIT_FIFOEN()
{
  uint16_t  nb_tx_data;

  /* Check that a Tx process is ongoing */
  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    for (nb_tx_data = huart->NbTxDataToProcess ; nb_tx_data > 0U ; nb_tx_data--)
    {
      if (huart->TxXferCount == 0U)
      {
        /* Disable the TX FIFO threshold interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_TXFTIE);

        /* Enable the UART Transmit Complete Interrupt */
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);

        break; /* force exit loop */
      }
      else if (READ_BIT(huart->Instance->ISR, USART_ISR_TXE_TXFNF) != 0U)
      {
        huart->Instance->TDR = (uint8_t)(*huart->pTxBuffPtr & (uint8_t)0xFF);
        huart->pTxBuffPtr++;
        huart->TxXferCount--;
      }
      else
      {
        /* Nothing to do */
      }
    }
  }
}

/**
  * @brief TX interrupt handler for 9 bits data word length and FIFO mode is enabled.
  * @note   Function is called under interruption only, once
  *         interruptions have been enabled by HAL_UART_Transmit_IT().
  * @param huart UART handle.
  * @retval None
  */
static void UART_TxISR_16BIT_FIFOEN()
{
  const uint16_t *tmp;
  uint16_t  nb_tx_data;

  /* Check that a Tx process is ongoing */
  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    for (nb_tx_data = huart->NbTxDataToProcess ; nb_tx_data > 0U ; nb_tx_data--)
    {
      if (huart->TxXferCount == 0U)
      {
        /* Disable the TX FIFO threshold interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_TXFTIE);

        /* Enable the UART Transmit Complete Interrupt */
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);

        break; /* force exit loop */
      }
      else if (READ_BIT(huart->Instance->ISR, USART_ISR_TXE_TXFNF) != 0U)
      {
        tmp = (const uint16_t *) huart->pTxBuffPtr;
        huart->Instance->TDR = (((uint32_t)(*tmp)) & 0x01FFUL);
        huart->pTxBuffPtr += 2U;
        huart->TxXferCount--;
      }
      else
      {
        /* Nothing to do */
      }
    }
  }
}

/**
  * @brief  Wrap up transmission in non-blocking mode.
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
static void UART_EndTransmit_IT()
{
  /* Disable the UART Transmit Complete Interrupt */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_TCIE);

  /* Tx process is ended, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;

  /* Cleat TxISR function pointer */
  huart->TxISR = NULL;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered Tx complete callback*/
  huart->TxCpltCallback(huart);
#else
  /*Call legacy weak Tx complete callback*/
  HAL_UART_TxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief RX interrupt handler for 7 or 8 bits data word length .
  * @param huart UART handle.
  * @retval None
  */
static void UART_RxISR_8BIT()
{
  uint16_t uhMask = huart->Mask;
  uint16_t  uhdata;

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
    *huart->pRxBuffPtr = (uint8_t)(uhdata & (uint8_t)uhMask);
    huart->pRxBuffPtr++;
    huart->RxXferCount--;

    if (huart->RxXferCount == 0U)
    {
      /* Disable the UART Parity Error Interrupt and RXNE interrupts */
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

      /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;

      /* Clear RxISR function pointer */
      huart->RxISR = NULL;

      /* Initialize type of RxEvent to Transfer Complete */
      huart->RxEventType = HAL_UART_RXEVENT_TC;

      if (!(IS_LPUART_INSTANCE(huart->Instance)))
      {
        /* Check that USART RTOEN bit is set */
        if (READ_BIT(huart->Instance->CR2, USART_CR2_RTOEN) != 0U)
        {
          /* Enable the UART Receiver Timeout Interrupt */
          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_RTOIE);
        }
      }

      /* Check current reception Mode :
         If Reception till IDLE event has been selected : */
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        /* Set reception type to Standard */
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        /* Disable IDLE interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) == SET)
        {
          /* Clear IDLE Flag */
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_IDLEF);
        }

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx Event callback*/
        huart->RxEventCallback(huart, huart->RxXferSize);
#else
        /*Call legacy weak Rx Event callback*/
        HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
      }
      else
      {
        /* Standard reception API called */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx complete callback*/
        huart->RxCpltCallback(huart);
#else
        /*Call legacy weak Rx complete callback*/
        HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
    }
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
  }
}

/**
  * @brief RX interrupt handler for 9 bits data word length .
  * @note   Function is called under interruption only, once
  *         interruptions have been enabled by HAL_UART_Receive_IT()
  * @param huart UART handle.
  * @retval None
  */
static void UART_RxISR_16BIT()
{
  uint16_t *tmp;
  uint16_t uhMask = huart->Mask;
  uint16_t  uhdata;

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
    tmp = (uint16_t *) huart->pRxBuffPtr ;
    *tmp = (uint16_t)(uhdata & uhMask);
    huart->pRxBuffPtr += 2U;
    huart->RxXferCount--;

    if (huart->RxXferCount == 0U)
    {
      /* Disable the UART Parity Error Interrupt and RXNE interrupt*/
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

      /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;

      /* Clear RxISR function pointer */
      huart->RxISR = NULL;

      /* Initialize type of RxEvent to Transfer Complete */
      huart->RxEventType = HAL_UART_RXEVENT_TC;

      if (!(IS_LPUART_INSTANCE(huart->Instance)))
      {
        /* Check that USART RTOEN bit is set */
        if (READ_BIT(huart->Instance->CR2, USART_CR2_RTOEN) != 0U)
        {
          /* Enable the UART Receiver Timeout Interrupt */
          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_RTOIE);
        }
      }

      /* Check current reception Mode :
         If Reception till IDLE event has been selected : */
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        /* Set reception type to Standard */
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        /* Disable IDLE interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) == SET)
        {
          /* Clear IDLE Flag */
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_IDLEF);
        }

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx Event callback*/
        huart->RxEventCallback(huart, huart->RxXferSize);
#else
        /*Call legacy weak Rx Event callback*/
        HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
      }
      else
      {
        /* Standard reception API called */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx complete callback*/
        huart->RxCpltCallback(huart);
#else
        /*Call legacy weak Rx complete callback*/
        HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
    }
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
  }
}

/**
  * @brief RX interrupt handler for 7 or 8  bits data word length and FIFO mode is enabled.
  * @note   Function is called under interruption only, once
  *         interruptions have been enabled by HAL_UART_Receive_IT()
  * @param huart UART handle.
  * @retval None
  */
static void UART_RxISR_8BIT_FIFOEN()
{
  uint16_t  uhMask = huart->Mask;
  uint16_t  uhdata;
  uint16_t  nb_rx_data;
  uint16_t  rxdatacount;
  uint32_t  isrflags = READ_REG(huart->Instance->ISR);
  uint32_t  cr1its   = READ_REG(huart->Instance->CR1);
  uint32_t  cr3its   = READ_REG(huart->Instance->CR3);

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    nb_rx_data = huart->NbRxDataToProcess;
    while ((nb_rx_data > 0U) && ((isrflags & USART_ISR_RXNE_RXFNE) != 0U))
    {
      uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
      *huart->pRxBuffPtr = (uint8_t)(uhdata & (uint8_t)uhMask);
      huart->pRxBuffPtr++;
      huart->RxXferCount--;
      isrflags = READ_REG(huart->Instance->ISR);

      /* If some non blocking errors occurred */
      if ((isrflags & (USART_ISR_PE | USART_ISR_FE | USART_ISR_NE)) != 0U)
      {
        /* UART parity error interrupt occurred -------------------------------------*/
        if (((isrflags & USART_ISR_PE) != 0U) && ((cr1its & USART_CR1_PEIE) != 0U))
        {
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);

          huart->ErrorCode |= HAL_UART_ERROR_PE;
        }

        /* UART frame error interrupt occurred --------------------------------------*/
        if (((isrflags & USART_ISR_FE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
        {
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);

          huart->ErrorCode |= HAL_UART_ERROR_FE;
        }

        /* UART noise error interrupt occurred --------------------------------------*/
        if (((isrflags & USART_ISR_NE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
        {
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);

          huart->ErrorCode |= HAL_UART_ERROR_NE;
        }

        /* Call UART Error Call back function if need be ----------------------------*/
        if (huart->ErrorCode != HAL_UART_ERROR_NONE)
        {
          /* Non Blocking error : transfer could go on.
          Error is notified to user through user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /*Call registered error callback*/
          huart->ErrorCallback(huart);
#else
          /*Call legacy weak error callback*/
          HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
          huart->ErrorCode = HAL_UART_ERROR_NONE;
        }
      }

      if (huart->RxXferCount == 0U)
      {
        /* Disable the UART Parity Error Interrupt and RXFT interrupt*/
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);

        /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error)
           and RX FIFO Threshold interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));

        /* Rx process is completed, restore huart->RxState to Ready */
        huart->RxState = HAL_UART_STATE_READY;

        /* Clear RxISR function pointer */
        huart->RxISR = NULL;

        /* Initialize type of RxEvent to Transfer Complete */
        huart->RxEventType = HAL_UART_RXEVENT_TC;

        if (!(IS_LPUART_INSTANCE(huart->Instance)))
        {
          /* Check that USART RTOEN bit is set */
          if (READ_BIT(huart->Instance->CR2, USART_CR2_RTOEN) != 0U)
          {
            /* Enable the UART Receiver Timeout Interrupt */
            ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_RTOIE);
          }
        }

        /* Check current reception Mode :
           If Reception till IDLE event has been selected : */
        if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
        {
          /* Set reception type to Standard */
          huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

          /* Disable IDLE interrupt */
          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

          if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) == SET)
          {
            /* Clear IDLE Flag */
            __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_IDLEF);
          }

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /*Call registered Rx Event callback*/
          huart->RxEventCallback(huart, huart->RxXferSize);
#else
          /*Call legacy weak Rx Event callback*/
          HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
        }
        else
        {
          /* Standard reception API called */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /*Call registered Rx complete callback*/
          huart->RxCpltCallback(huart);
#else
          /*Call legacy weak Rx complete callback*/
          HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
        }
        break;
      }
    }

    /* When remaining number of bytes to receive is less than the RX FIFO
    threshold, next incoming frames are processed as if FIFO mode was
    disabled (i.e. one interrupt per received frame).
    */
    rxdatacount = huart->RxXferCount;
    if ((rxdatacount != 0U) && (rxdatacount < huart->NbRxDataToProcess))
    {
      /* Disable the UART RXFT interrupt*/
      ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_RXFTIE);

      /* Update the RxISR function pointer */
      huart->RxISR = UART_RxISR_8BIT;

      /* Enable the UART Data Register Not Empty interrupt */
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_RXNEIE_RXFNEIE);
    }
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
  }
}

/**
  * @brief RX interrupt handler for 9 bits data word length and FIFO mode is enabled.
  * @note   Function is called under interruption only, once
  *         interruptions have been enabled by HAL_UART_Receive_IT()
  * @param huart UART handle.
  * @retval None
  */
static void UART_RxISR_16BIT_FIFOEN()
{
  uint16_t *tmp;
  uint16_t  uhMask = huart->Mask;
  uint16_t  uhdata;
  uint16_t  nb_rx_data;
  uint16_t  rxdatacount;
  uint32_t  isrflags = READ_REG(huart->Instance->ISR);
  uint32_t  cr1its   = READ_REG(huart->Instance->CR1);
  uint32_t  cr3its   = READ_REG(huart->Instance->CR3);

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    nb_rx_data = huart->NbRxDataToProcess;
    while ((nb_rx_data > 0U) && ((isrflags & USART_ISR_RXNE_RXFNE) != 0U))
    {
      uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
      tmp = (uint16_t *) huart->pRxBuffPtr ;
      *tmp = (uint16_t)(uhdata & uhMask);
      huart->pRxBuffPtr += 2U;
      huart->RxXferCount--;
      isrflags = READ_REG(huart->Instance->ISR);

      /* If some non blocking errors occurred */
      if ((isrflags & (USART_ISR_PE | USART_ISR_FE | USART_ISR_NE)) != 0U)
      {
        /* UART parity error interrupt occurred -------------------------------------*/
        if (((isrflags & USART_ISR_PE) != 0U) && ((cr1its & USART_CR1_PEIE) != 0U))
        {
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);

          huart->ErrorCode |= HAL_UART_ERROR_PE;
        }

        /* UART frame error interrupt occurred --------------------------------------*/
        if (((isrflags & USART_ISR_FE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
        {
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);

          huart->ErrorCode |= HAL_UART_ERROR_FE;
        }

        /* UART noise error interrupt occurred --------------------------------------*/
        if (((isrflags & USART_ISR_NE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
        {
          __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);

          huart->ErrorCode |= HAL_UART_ERROR_NE;
        }

        /* Call UART Error Call back function if need be ----------------------------*/
        if (huart->ErrorCode != HAL_UART_ERROR_NONE)
        {
          /* Non Blocking error : transfer could go on.
          Error is notified to user through user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /*Call registered error callback*/
          huart->ErrorCallback(huart);
#else
          /*Call legacy weak error callback*/
          HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
          huart->ErrorCode = HAL_UART_ERROR_NONE;
        }
      }

      if (huart->RxXferCount == 0U)
      {
        /* Disable the UART Parity Error Interrupt and RXFT interrupt*/
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);

        /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error)
           and RX FIFO Threshold interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));

        /* Rx process is completed, restore huart->RxState to Ready */
        huart->RxState = HAL_UART_STATE_READY;

        /* Clear RxISR function pointer */
        huart->RxISR = NULL;

        /* Initialize type of RxEvent to Transfer Complete */
        huart->RxEventType = HAL_UART_RXEVENT_TC;

        if (!(IS_LPUART_INSTANCE(huart->Instance)))
        {
          /* Check that USART RTOEN bit is set */
          if (READ_BIT(huart->Instance->CR2, USART_CR2_RTOEN) != 0U)
          {
            /* Enable the UART Receiver Timeout Interrupt */
            ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_RTOIE);
          }
        }

        /* Check current reception Mode :
           If Reception till IDLE event has been selected : */
        if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
        {
          /* Set reception type to Standard */
          huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

          /* Disable IDLE interrupt */
          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

          if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) == SET)
          {
            /* Clear IDLE Flag */
            __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_IDLEF);
          }

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /*Call registered Rx Event callback*/
          huart->RxEventCallback(huart, huart->RxXferSize);
#else
          /*Call legacy weak Rx Event callback*/
          HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
        }
        else
        {
          /* Standard reception API called */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /*Call registered Rx complete callback*/
          huart->RxCpltCallback(huart);
#else
          /*Call legacy weak Rx complete callback*/
          HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
        }
        break;
      }
    }

    /* When remaining number of bytes to receive is less than the RX FIFO
    threshold, next incoming frames are processed as if FIFO mode was
    disabled (i.e. one interrupt per received frame).
    */
    rxdatacount = huart->RxXferCount;
    if ((rxdatacount != 0U) && (rxdatacount < huart->NbRxDataToProcess))
    {
      /* Disable the UART RXFT interrupt*/
      ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_RXFTIE);

      /* Update the RxISR function pointer */
      huart->RxISR = UART_RxISR_16BIT;

      /* Enable the UART Data Register Not Empty interrupt */
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_RXNEIE_RXFNEIE);
    }
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
  }
}

/**
  * @}
  */

#endif /* HAL_UART_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */




#if 0

static constexpr uint32_t USART_CR1_ENABLE = 1;

USART<1> usart1;


void USARTBase::_enable()
{
  csr->CR1 |= USART_CR1_ENABLE;
}


void USARTBase::_disable()
{
  csr->CR1 &= ~USART_CR1_ENABLE;
}

void USARTBase::init(uint32_t _baud_rate,
                     wordlength _wl,
		     parity _par,
                     stopbits _sb,
                     pinmode _mode,
                     flow_control _fc,
                     uint32_t oversampling,
                     bool one_bit_sampling,
                     uint32_t divider,
                     bool advanced)
{
  br = _baud_rate;
  wl = _wl;
  par = _par;
  sb = _sb;
  mode = _mode;
  fc = _fc;
  
  // Hardwired clock sel for now
  set_clk_sel(0);

  // hardwired gpio for now

  auto pin1 = gpio::GPIOB[15];
  
  pin1.enable();
  pin1.set_open_drain(false);
  pin1.set_pull(gpio::pinpull::NONE);
  pin1.set_speed(gpio::pinspeed::LOW);
  pin1.set_alternate_function(4);
  
  auto pin2 = gpio::GPIOA[9];
  
  pin2.enable();
  pin2.set_open_drain(false);
  pin2.set_pull(gpio::pinpull::NONE);
  pin2.set_speed(gpio::pinspeed::LOW);
  pin2.set_alternate_function(4);
  
  // TODO: Advanced Feature Config if needed
    
  // Ensure the clocks are initialized

#if 0  
  huart->gState = HAL_UART_STATE_BUSY;

  _disable();

  /* Perform advanced settings configuration */
  /* For some items, configuration requires to be done prior TE and RE bits are set */
  if (huart->AdvancedInit.AdvFeatureInit != UART_ADVFEATURE_NO_INIT)
  {
    UART_AdvFeatureConfig(huart);
  }

  /* Set the UART Communication parameters */
  if (UART_SetConfig(huart) == HAL_ERROR)
  {
    return HAL_ERROR;
  }

  /* In asynchronous mode, the following bits must be kept cleared:
  - LINEN and CLKEN bits in the USART_CR2 register,
  - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  __HAL_UART_ENABLE(huart);

  /* TEACK and/or REACK to check before moving huart->gState and huart->RxState to Ready */
  return (UART_CheckIdleState(huart));
#endif

  update_serial();
}

void USARTBase::update_serial()
{
  uint32_t mask = (1 << 28) | (1 << 12)
    | (1 << 10) | (1 << 9);
  
  bool m0 = false, m1 = false;
  bool pce = false, ps = false;

  switch(wl) {
  case wordlength::SEVEN:
    m1 = true;
    break;
  case wordlength::NINE:
    m0 = true;
    break;
  }
  
  switch(par) {
  case parity::EVEN:
    pce = true;
    break;
  case parity::ODD:
    pce = true;
    ps = true;
    break;
  }
  
}
#endif

