#include <halxx/memory_map.hpp>
#include <halxx/clocking.hpp>
#include <halxx/usart.hpp>

#include <stm32h5/gpio.hpp>

using namespace usart;

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
}

#if 0
HAL_StatusTypeDef UART_SetConfig(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg;
  uint16_t brrtemp;
  uint32_t clocksource;
  uint32_t usartdiv;
  HAL_StatusTypeDef ret               = HAL_OK;
  uint32_t lpuart_ker_ck_pres;
  uint32_t pclk;

  /* Check the parameters */
  assert_param(IS_UART_BAUDRATE(huart->Init.BaudRate));
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
  if (UART_INSTANCE_LOWPOWER(huart))
  {
    assert_param(IS_LPUART_STOPBITS(huart->Init.StopBits));
  }
  else
  {
    assert_param(IS_UART_STOPBITS(huart->Init.StopBits));
    assert_param(IS_UART_ONE_BIT_SAMPLE(huart->Init.OneBitSampling));
  }

  assert_param(IS_UART_PARITY(huart->Init.Parity));
  assert_param(IS_UART_MODE(huart->Init.Mode));
  assert_param(IS_UART_HARDWARE_FLOW_CONTROL(huart->Init.HwFlowCtl));
  assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));
  assert_param(IS_UART_PRESCALER(huart->Init.ClockPrescaler));

  /*-------------------------- USART CR1 Configuration -----------------------*/
  /* Clear M, PCE, PS, TE, RE and OVER8 bits and configure
  *  the UART Word Length, Parity, Mode and oversampling:
  *  set the M bits according to huart->Init.WordLength value
  *  set PCE and PS bits according to huart->Init.Parity value
  *  set TE and RE bits according to huart->Init.Mode value
  *  set OVER8 bit according to huart->Init.OverSampling value */
  tmpreg = (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode | huart->Init.OverSampling ;
  MODIFY_REG(huart->Instance->CR1, USART_CR1_FIELDS, tmpreg);

  /*-------------------------- USART CR2 Configuration -----------------------*/
  /* Configure the UART Stop Bits: Set STOP[13:12] bits according
  * to huart->Init.StopBits value */
  MODIFY_REG(huart->Instance->CR2, USART_CR2_STOP, huart->Init.StopBits);

  /*-------------------------- USART CR3 Configuration -----------------------*/
  /* Configure
  * - UART HardWare Flow Control: set CTSE and RTSE bits according
  *   to huart->Init.HwFlowCtl value
  * - one-bit sampling method versus three samples' majority rule according
  *   to huart->Init.OneBitSampling (not applicable to LPUART) */
  tmpreg = (uint32_t)huart->Init.HwFlowCtl;

  if (!(UART_INSTANCE_LOWPOWER(huart)))
  {
    tmpreg |= huart->Init.OneBitSampling;
  }
  MODIFY_REG(huart->Instance->CR3, USART_CR3_FIELDS, tmpreg);

  /*-------------------------- USART PRESC Configuration -----------------------*/
  /* Configure
  * - UART Clock Prescaler : set PRESCALER according to huart->Init.ClockPrescaler value */
  MODIFY_REG(huart->Instance->PRESC, USART_PRESC_PRESCALER, huart->Init.ClockPrescaler);

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
      lpuart_ker_ck_pres = (pclk / UARTPrescTable[huart->Init.ClockPrescaler]);

      /* Ensure that Frequency clock is in the range [3 * baudrate, 4096 * baudrate] */
      if ((lpuart_ker_ck_pres < (3U * huart->Init.BaudRate)) ||
          (lpuart_ker_ck_pres > (4096U * huart->Init.BaudRate)))
      {
        ret = HAL_ERROR;
      }
      else
      {
        /* Check computed UsartDiv value is in allocated range
           (it is forbidden to write values lower than 0x300 in the LPUART_BRR register) */
        usartdiv = (uint32_t)(UART_DIV_LPUART(pclk, huart->Init.BaudRate, huart->Init.ClockPrescaler));
        if ((usartdiv >= LPUART_BRR_MIN) && (usartdiv <= LPUART_BRR_MAX))
        {
          huart->Instance->BRR = usartdiv;
        }
        else
        {
          ret = HAL_ERROR;
        }
      } /* if ( (lpuart_ker_ck_pres < (3 * huart->Init.BaudRate) ) ||
                (lpuart_ker_ck_pres > (4096 * huart->Init.BaudRate) )) */
    } /* if (pclk != 0) */
  }
  /* Check UART Over Sampling to set Baud Rate Register */
  else if (huart->Init.OverSampling == UART_OVERSAMPLING_8)
  {
    pclk = HAL_RCCEx_GetPeriphCLKFreq(clocksource);

    /* USARTDIV must be greater than or equal to 0d16 */
    if (pclk != 0U)
    {
      usartdiv = (uint32_t)(UART_DIV_SAMPLING8(pclk, huart->Init.BaudRate, huart->Init.ClockPrescaler));
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
      usartdiv = (uint32_t)(UART_DIV_SAMPLING16(pclk, huart->Init.BaudRate, huart->Init.ClockPrescaler));
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

#endif

#if 0
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 921600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB15     ------> USART1_RX
    PA9     ------> USART1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PB15     ------> USART1_RX
    PA9     ------> USART1_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
#endif
