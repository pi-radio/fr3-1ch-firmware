#pragma once


#include "stm32h5xx_hal.h"


namespace halxx
{
  namespace SPI
  {
    class Bus
    {
      SPI_HandleTypeDef hspi;

    protected:
      virtual void InitGPIO();
      virtual void InitClock();
      virtual void InitInterrupt();

      virtual void OnTXComplete() {};
      virtual void OnRXComplete() {};
      virtual void OnTXRXComplete() {};
      virtual void OnTXHalfComplete() {};
      virtual void OnRXHalfComplete() {};
      virtual void OnTXRXHalfComplete() {};
      virtual void OnError() {};
      virtual void OnAbortComplete() {};
      virtual void OnSuspend() {};

      void close_transfer();

      template <uint32_t flag>
      uint32_t wait_on_flag_clear_timeout(uint32_t timeout, uint32_t start)
      {
        while (hspi.Instance->SR & flag) //  (__HAL_SPI_GET_FLAG(hspi, Flag) ? SET : RESET) == Status)
        {
          /* Check for the Timeout */
          if ((((HAL_GetTick() - start) >=  timeout) && (timeout != HAL_MAX_DELAY)) || (timeout == 0U))
          {
            return HAL_TIMEOUT;
          }
        }
        return HAL_OK;
      }

    public:
      Bus(int periph);

      uint32_t packet_size();

      HAL_StatusTypeDef transmit(const uint8_t *pData, uint16_t Size, uint32_t Timeout);
    };

    class Device
    {

    };
  }
}

