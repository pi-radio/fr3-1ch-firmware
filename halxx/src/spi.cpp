#include <stdexcept>
#include <format>

#include <stm32h5/spi.hpp>


SPI_TypeDef *bases[] = {
    NULL,
    SPI1,
    SPI2,
    SPI3,
    SPI4,
    SPI5,
    SPI6
};

halxx::SPI::Bus::Bus(int periph_no)
{
  uint32_t crc_length;
  uint32_t packet_length;

  hspi.Instance = bases[periph_no];
  hspi.Init.Mode = SPI_MODE_MASTER;
  hspi.Init.Direction = SPI_DIRECTION_2LINES;
  hspi.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi.Init.NSS = SPI_NSS_SOFT;
  hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi.Init.CRCPolynomial = 0x7;
  hspi.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;


  /* Check the parameters */
  assert_param(IS_SPI_ALL_INSTANCE(hspi.Instance));
  assert_param(IS_SPI_MODE(hspi.Init.Mode));
  assert_param(IS_SPI_DIRECTION(hspi.Init.Direction));

  if (IS_SPI_LIMITED_INSTANCE(hspi.Instance))
  {
    assert_param(IS_SPI_LIMITED_DATASIZE(hspi.Init.DataSize));
    assert_param(IS_SPI_LIMITED_FIFOTHRESHOLD(hspi.Init.FifoThreshold));
  }
  else
  {
    assert_param(IS_SPI_DATASIZE(hspi.Init.DataSize));
    assert_param(IS_SPI_FIFOTHRESHOLD(hspi.Init.FifoThreshold));
  }

  assert_param(IS_SPI_NSS(hspi.Init.NSS));
  assert_param(IS_SPI_NSSP(hspi.Init.NSSPMode));
  assert_param(IS_SPI_BAUDRATE_PRESCALER(hspi.Init.BaudRatePrescaler));
  assert_param(IS_SPI_FIRST_BIT(hspi.Init.FirstBit));
  assert_param(IS_SPI_TIMODE(hspi.Init.TIMode));

  if (hspi.Init.TIMode == SPI_TIMODE_DISABLE)
  {
    assert_param(IS_SPI_CPOL(hspi.Init.CLKPolarity));
    assert_param(IS_SPI_CPHA(hspi.Init.CLKPhase));
  }

  hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;

  assert_param(IS_SPI_RDY_MASTER_MANAGEMENT(hspi.Init.ReadyMasterManagement));
  assert_param(IS_SPI_RDY_POLARITY(hspi.Init.ReadyPolarity));
  assert_param(IS_SPI_MASTER_RX_AUTOSUSP(hspi.Init.MasterReceiverAutoSusp));

  /* Verify that the SPI instance supports Data Size higher than 16bits */
  if ((IS_SPI_LIMITED_INSTANCE(hspi.Instance)) && (hspi.Init.DataSize > SPI_DATASIZE_16BIT))
  {
    throw std::runtime_error(std::format("Datasize {} is too large", hspi.Init.DataSize));
  }

  /* Verify that the SPI instance supports requested data packing */
  packet_length = packet_size();

  if (((IS_SPI_LIMITED_INSTANCE(hspi.Instance)) && (packet_length > SPI_LOWEND_FIFO_SIZE)) ||
      ((IS_SPI_FULL_INSTANCE(hspi.Instance)) && (packet_length > SPI_HIGHEND_FIFO_SIZE)))
  {
    throw std::runtime_error("Packet length longer than FIFO size");
  }

  if (hspi.State == HAL_SPI_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hspi.Lock = HAL_UNLOCKED;

    InitGPIO();
    InitClock();
    InitInterrupt();
  }

  hspi.State = HAL_SPI_STATE_BUSY;

  /* Disable the selected SPI peripheral */
  __HAL_SPI_DISABLE(&hspi);

  /* Keep the default value of CRCSIZE in case of CRC is not used */
  crc_length = hspi.Instance->CFG1 & SPI_CFG1_CRCSIZE;

  /*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
  /* Configure : SPI Mode, Communication Mode, Clock polarity and phase, NSS management,
  Communication speed, First bit, CRC calculation state, CRC Length */

  /* SPIx NSS Software Management Configuration */
  if ((hspi.Init.NSS == SPI_NSS_SOFT) && (((hspi.Init.Mode == SPI_MODE_MASTER) &&  \
                                            (hspi.Init.NSSPolarity == SPI_NSS_POLARITY_LOW)) || \
                                           ((hspi.Init.Mode == SPI_MODE_SLAVE) && \
                                            (hspi.Init.NSSPolarity == SPI_NSS_POLARITY_HIGH))))
  {
    SET_BIT(hspi.Instance->CR1, SPI_CR1_SSI);
  }

  /* SPIx Master Rx Auto Suspend Configuration */
  if (((hspi.Init.Mode & SPI_MODE_MASTER) == SPI_MODE_MASTER) && (hspi.Init.DataSize >= SPI_DATASIZE_8BIT))
  {
    MODIFY_REG(hspi.Instance->CR1, SPI_CR1_MASRX, hspi.Init.MasterReceiverAutoSusp);
  }
  else
  {
    CLEAR_BIT(hspi.Instance->CR1, SPI_CR1_MASRX);
  }

  /* SPIx CFG1 Configuration */
  WRITE_REG(hspi.Instance->CFG1, (hspi.Init.BaudRatePrescaler | hspi.Init.CRCCalculation | crc_length |
                                   hspi.Init.FifoThreshold     | hspi.Init.DataSize));

  /* SPIx CFG2 Configuration */
  WRITE_REG(hspi.Instance->CFG2, (hspi.Init.NSSPMode                | hspi.Init.TIMode    |
                                   hspi.Init.NSSPolarity             | hspi.Init.NSS       |
                                   hspi.Init.CLKPolarity             | hspi.Init.CLKPhase  |
                                   hspi.Init.FirstBit                | hspi.Init.Mode      |
                                   hspi.Init.MasterInterDataIdleness | hspi.Init.Direction |
                                   hspi.Init.MasterSSIdleness        | hspi.Init.IOSwap    |
                                   hspi.Init.ReadyMasterManagement   | hspi.Init.ReadyPolarity));


  CLEAR_BIT(hspi.Instance->I2SCFGR, SPI_I2SCFGR_I2SMOD);

  /* Insure that AFCNTR is managed only by Master */
  if ((hspi.Init.Mode & SPI_MODE_MASTER) == SPI_MODE_MASTER)
  {
    /* Alternate function GPIOs control */
    MODIFY_REG(hspi.Instance->CFG2, SPI_CFG2_AFCNTR, (hspi.Init.MasterKeepIOState));
  }

  hspi.ErrorCode = HAL_SPI_ERROR_NONE;
  hspi.State     = HAL_SPI_STATE_READY;



}

void halxx::SPI::Bus::InitGPIO()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOE_CLK_ENABLE();
  /**SPI4 GPIO Configuration
  PE2     ------> SPI4_SCK
  PE5     ------> SPI4_MISO
  PE6     ------> SPI4_MOSI
  */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}


void halxx::SPI::Bus::InitClock()
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI4;
  PeriphClkInitStruct.Spi4ClockSelection = RCC_SPI4CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    throw std::runtime_error("Unable to set up SPI clock");
  }

  /* SPI4 clock enable */
  __HAL_RCC_SPI4_CLK_ENABLE();
}


void halxx::SPI::Bus::InitInterrupt()
{
  HAL_NVIC_SetPriority(SPI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SPI4_IRQn);
}


uint32_t halxx::SPI::Bus::packet_size()
{
  uint32_t fifo_threashold = (hspi.Init.FifoThreshold >> SPI_CFG1_FTHLV_Pos) + 1UL;
  uint32_t data_size       = (hspi.Init.DataSize      >> SPI_CFG1_DSIZE_Pos) + 1UL;

    /* Convert data size to Byte */
  data_size = (data_size + 7UL) / 8UL;

  return data_size * fifo_threashold;
}

void halxx::SPI::Bus::close_transfer()
{
  uint32_t itflag = hspi.Instance->SR;

  __HAL_SPI_CLEAR_EOTFLAG(&hspi);
  __HAL_SPI_CLEAR_TXTFFLAG(&hspi);

  /* Disable SPI peripheral */
  __HAL_SPI_DISABLE(&hspi);

  /* Disable ITs */
  __HAL_SPI_DISABLE_IT(&hspi, (SPI_IT_EOT | SPI_IT_TXP | SPI_IT_RXP | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | \
                              SPI_IT_FRE | SPI_IT_MODF));

  /* Disable Tx DMA Request */
  CLEAR_BIT(hspi.Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

  /* Report UnderRun error for non RX Only communication */
  if (hspi.State != HAL_SPI_STATE_BUSY_RX)
  {
    if ((itflag & SPI_FLAG_UDR) != 0UL)
    {
      SET_BIT(hspi.ErrorCode, HAL_SPI_ERROR_UDR);
      __HAL_SPI_CLEAR_UDRFLAG(&hspi);
    }
  }

  /* Report OverRun error for non TX Only communication */
  if (hspi.State != HAL_SPI_STATE_BUSY_TX)
  {
    if ((itflag & SPI_FLAG_OVR) != 0UL)
    {
      SET_BIT(hspi.ErrorCode, HAL_SPI_ERROR_OVR);
      __HAL_SPI_CLEAR_OVRFLAG(&hspi);
    }

#if (USE_SPI_CRC != 0UL)
    /* Check if CRC error occurred */
    if (hspi.Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      if ((itflag & SPI_FLAG_CRCERR) != 0UL)
      {
        SET_BIT(hspi.ErrorCode, HAL_SPI_ERROR_CRC);
        __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
      }
    }
#endif /* USE_SPI_CRC */
  }

  /* SPI Mode Fault error interrupt occurred -------------------------------*/
  if ((itflag & SPI_FLAG_MODF) != 0UL)
  {
    SET_BIT(hspi.ErrorCode, HAL_SPI_ERROR_MODF);
    __HAL_SPI_CLEAR_MODFFLAG(&hspi);
  }

  /* SPI Frame error interrupt occurred ------------------------------------*/
  if ((itflag & SPI_FLAG_FRE) != 0UL)
  {
    SET_BIT(hspi.ErrorCode, HAL_SPI_ERROR_FRE);
    __HAL_SPI_CLEAR_FREFLAG(&hspi);
  }

  hspi.TxXferCount = (uint16_t)0UL;
  hspi.RxXferCount = (uint16_t)0UL;
}

HAL_StatusTypeDef halxx::SPI::Bus::transmit(const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
#if defined (__GNUC__)
  __IO uint16_t *ptxdr_16bits = (__IO uint16_t *)(&(hspi.Instance->TXDR));
#endif /* __GNUC__ */

  uint32_t tickstart;

  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_TXONLY(hspi.Init.Direction));

  /* Check transfer size parameter */
  if (IS_SPI_LIMITED_INSTANCE(hspi.Instance))
  {
    assert_param(IS_SPI_LIMITED_TRANSFER_SIZE(Size));
  }
  else
  {
    assert_param(IS_SPI_TRANSFER_SIZE(Size));
  }

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  if (hspi.State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if ((pData == NULL) || (Size == 0UL))
  {
    return HAL_ERROR;
  }

  /* Lock the process */
  __HAL_LOCK(&hspi);

  /* Set the transaction information */
  hspi.State       = HAL_SPI_STATE_BUSY_TX;
  hspi.ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi.pTxBuffPtr  = (const uint8_t *)pData;
  hspi.TxXferSize  = Size;
  hspi.TxXferCount = Size;

  /*Init field not used in handle to zero */
  hspi.pRxBuffPtr  = NULL;
  hspi.RxXferSize  = (uint16_t) 0UL;
  hspi.RxXferCount = (uint16_t) 0UL;
  hspi.TxISR       = NULL;
  hspi.RxISR       = NULL;

  /* Configure communication direction : 1Line */
  if (hspi.Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_TX(&hspi);
  }
  else
  {
    SPI_2LINES_TX(&hspi);
  }

  /* Set the number of data at current transfer */
  MODIFY_REG(hspi.Instance->CR2, SPI_CR2_TSIZE, Size);

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(&hspi);

  if (hspi.Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi.Instance->CR1, SPI_CR1_CSTART);
  }

  /* Transmit data in 32 Bit mode */
  if ((hspi.Init.DataSize > SPI_DATASIZE_16BIT) && (IS_SPI_FULL_INSTANCE(hspi.Instance)))
  {
    /* Transmit data in 32 Bit mode */
    while (hspi.TxXferCount > 0UL)
    {
      /* Wait until TXP flag is set to send data */
      if (__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_TXP))
      {
        *((__IO uint32_t *)&hspi.Instance->TXDR) = *((const uint32_t *)hspi.pTxBuffPtr);
        hspi.pTxBuffPtr += sizeof(uint32_t);
        hspi.TxXferCount -= 1;
      }
      else
      {
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
          /* Call standard close procedure with error check */
          close_transfer();

          SET_BIT(hspi.ErrorCode, HAL_SPI_ERROR_TIMEOUT);
          hspi.State = HAL_SPI_STATE_READY;

          /* Unlock the process */
          __HAL_UNLOCK(&hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  /* Transmit data in 16 Bit mode */
  else if (hspi.Init.DataSize > SPI_DATASIZE_8BIT)
  {
    /* Transmit data in 16 Bit mode */
    while (hspi.TxXferCount > 0UL)
    {
      /* Wait until TXP flag is set to send data */
      if (__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_TXP))
      {
        if ((hspi.TxXferCount > 1UL) && (hspi.Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
        {
          *((__IO uint32_t *)&hspi.Instance->TXDR) = *((const uint32_t *)hspi.pTxBuffPtr);
          hspi.pTxBuffPtr += sizeof(uint32_t);
          hspi.TxXferCount -= (uint16_t)2UL;
        }
        else
        {
#if defined (__GNUC__)
          *ptxdr_16bits = *((const uint16_t *)hspi.pTxBuffPtr);
#else
          *((__IO uint16_t *)&hspi.Instance->TXDR) = *((const uint16_t *)hspi.pTxBuffPtr);
#endif /* __GNUC__ */
          hspi.pTxBuffPtr += sizeof(uint16_t);
          hspi.TxXferCount -= 1;
        }
      }
      else
      {
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
          /* Call standard close procedure with error check */
          close_transfer();

          SET_BIT(hspi.ErrorCode, HAL_SPI_ERROR_TIMEOUT);
          hspi.State = HAL_SPI_STATE_READY;

          /* Unlock the process */
          __HAL_UNLOCK(&hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  /* Transmit data in 8 Bit mode */
  else
  {
    while (hspi.TxXferCount > 0UL)
    {
      /* Wait until TXP flag is set to send data */
      if (__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_TXP))
      {
        if ((hspi.TxXferCount > 3UL) && (hspi.Init.FifoThreshold > SPI_FIFO_THRESHOLD_03DATA))
        {
          *((__IO uint32_t *)&hspi.Instance->TXDR) = *((const uint32_t *)hspi.pTxBuffPtr);
          hspi.pTxBuffPtr += sizeof(uint32_t);
          hspi.TxXferCount -= (uint16_t)4UL;
        }
        else if ((hspi.TxXferCount > 1UL) && (hspi.Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
        {
#if defined (__GNUC__)
          *ptxdr_16bits = *((const uint16_t *)hspi.pTxBuffPtr);
#else
          *((__IO uint16_t *)&hspi.Instance->TXDR) = *((const uint16_t *)hspi.pTxBuffPtr);
#endif /* __GNUC__ */
          hspi.pTxBuffPtr += sizeof(uint16_t);
          hspi.TxXferCount -= (uint16_t)2UL;
        }
        else
        {
          *((__IO uint8_t *)&hspi.Instance->TXDR) = *((const uint8_t *)hspi.pTxBuffPtr);
          hspi.pTxBuffPtr += sizeof(uint8_t);
          hspi.TxXferCount -= 1;
        }
      }
      else
      {
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
          /* Call standard close procedure with error check */
          close_transfer();

          SET_BIT(hspi.ErrorCode, HAL_SPI_ERROR_TIMEOUT);
          hspi.State = HAL_SPI_STATE_READY;

          /* Unlock the process */
          __HAL_UNLOCK(&hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }

  //SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_EOT, RESET, Timeout, tickstart) != HAL_OK)
  /* Wait for Tx (and CRC) data to be sent */
  if (wait_on_flag_clear_timeout<SPI_FLAG_EOT>(Timeout, tickstart) != HAL_OK)
  {
    SET_BIT(hspi.ErrorCode, HAL_SPI_ERROR_FLAG);
  }

  /* Call standard close procedure with error check */
  close_transfer();

  hspi.State = HAL_SPI_STATE_READY;

  /* Unlock the process */
  __HAL_UNLOCK(&hspi);

  if (hspi.ErrorCode != HAL_SPI_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}



