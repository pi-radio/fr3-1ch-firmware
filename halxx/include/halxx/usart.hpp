#pragma once

#include <stdexcept>

#include <halxx/pinout.hpp>
#include <halxx/memory_map.hpp>
#include <halxx/clocking.hpp>

namespace halxx {
  struct lock {
    bool take() { return true; }
    void release() { }
  };
  
  namespace usart
  {
    enum class status {
      OK = 0,
      
    };
    
    enum class rx_type {
      standard,
      toidle,
      torto,
      tocharmatch
    };
    
    enum class rx_event_type {
      TC,
      HT,
      IDLE
    };

    
  
    struct serial_params {
      uint32_t BaudRate;                /*!< This member configures the UART communication baud rate.
					  The baud rate register is computed using the following formula:
					  @note For LPUART :
					  Baud Rate Register = ((256 * lpuart_ker_ckpres) / ((huart->Init.BaudRate)))
					  where lpuart_ker_ck_pres is the UART input clock divided by a prescaler.
					  @note For UART :
                                         - If oversampling is 16 or in LIN mode,
					 Baud Rate Register = ((uart_ker_ckpres) / ((huart->Init.BaudRate)))
                                         - If oversampling is 8,
					 Baud Rate Register[15:4] = ((2 * uart_ker_ckpres) /
					 ((huart->Init.BaudRate)))[15:4]
					 Baud Rate Register[3] =  0
					 Baud Rate Register[2:0] =  (((2 * uart_ker_ckpres) /
					 ((huart->Init.BaudRate)))[3:0]) >> 1
                                         where uart_ker_ck_pres is the UART input clock divided by a prescaler */

      uint32_t WordLength;              /*!< Specifies the number of data bits transmitted or received in a frame.
					  This parameter can be a value of @ref UARTEx_Word_Length. */
      
      uint32_t StopBits;                /*!< Specifies the number of stop bits transmitted.
					  This parameter can be a value of @ref UART_Stop_Bits. */

      uint32_t Parity;                  /*!< Specifies the parity mode.
					  This parameter can be a value of @ref UART_Parity
					  @note When parity is enabled, the computed parity is inserted
					  at the MSB position of the transmitted data (9th bit when
					  the word length is set to 9 data bits; 8th bit when the
					  word length is set to 8 data bits). */
      
      uint32_t Mode;                    /*!< Specifies whether the Receive or Transmit mode is enabled or disabled.
					  This parameter can be a value of @ref UART_Mode. */
      
      uint32_t HwFlowCtl;               /*!< Specifies whether the hardware flow control mode is enabled
					  or disabled.
					  This parameter can be a value of @ref UART_Hardware_Flow_Control. */
      
      uint32_t OverSampling;            /*!< Specifies whether the Over sampling 8 is enabled or disabled,
					  to achieve higher speed (up to f_PCLK/8).
					  This parameter can be a value of @ref UART_Over_Sampling. */
      
      uint32_t OneBitSampling;          /*!< Specifies whether a single sample or three samples' majority vote is selected.
					  Selecting the single sample method increases the receiver tolerance to clock
                                         deviations. This parameter can be a value of @ref UART_OneBit_Sampling. */
      
      uint32_t ClockPrescaler;          /*!< Specifies the prescaler value used to divide the UART clock source.
					  This parameter can be a value of @ref UART_ClockPrescaler. */
    };
    
    struct advanced_features
    {
      uint32_t AdvFeatureInit;        /*!< Specifies which advanced UART features is initialized. Several
					Advanced Features may be initialized at the same time .
					This parameter can be a value of
					@ref UART_Advanced_Features_Initialization_Type. */
      
      uint32_t TxPinLevelInvert;      /*!< Specifies whether the TX pin active level is inverted.
				      This parameter can be a value of @ref UART_Tx_Inv. */
      
      uint32_t RxPinLevelInvert;      /*!< Specifies whether the RX pin active level is inverted.
					This parameter can be a value of @ref UART_Rx_Inv. */
      
      uint32_t DataInvert;            /*!< Specifies whether data are inverted (positive/direct logic
					vs negative/inverted logic).
					This parameter can be a value of @ref UART_Data_Inv. */
      
      uint32_t Swap;                  /*!< Specifies whether TX and RX pins are swapped.
					This parameter can be a value of @ref UART_Rx_Tx_Swap. */
      
      uint32_t OverrunDisable;        /*!< Specifies whether the reception overrun detection is disabled.
					This parameter can be a value of @ref UART_Overrun_Disable. */
      
#if defined(HAL_DMA_MODULE_ENABLED)
      uint32_t DMADisableonRxError;   /*!< Specifies whether the DMA is disabled in case of reception error.
					This parameter can be a value of @ref UART_DMA_Disable_on_Rx_Error. */
      
#endif /* HAL_DMA_MODULE_ENABLED */
      uint32_t AutoBaudRateEnable;    /*!< Specifies whether auto Baud rate detection is enabled.
					This parameter can be a value of @ref UART_AutoBaudRate_Enable. */
      
      uint32_t AutoBaudRateMode;      /*!< If auto Baud rate detection is enabled, specifies how the rate
					detection is carried out.
					This parameter can be a value of @ref UART_AutoBaud_Rate_Mode. */
      
      uint32_t MSBFirst;              /*!< Specifies whether MSB is sent first on UART line.
					This parameter can be a value of @ref UART_MSB_First. */
    };

    struct state
    {
      uint32_t tx_error : 2;
      uint32_t tx_inited : 1;
      uint32_t tx_rsrvd1 : 2;
      uint32_t tx_pbusy : 1;
      uint32_t tx_rsrvd2 : 1;
      uint32_t tx_busy : 1;
      
      uint32_t rx_rsrvd1 : 2;
      uint32_t rx_inited : 1;
      uint32_t rx_rsrvd2 : 3;
      uint32_t rx_busy : 1;
      uint32_t rx_rsrvd3 : 1;
    };
    
    class USART
    {
      CSR::usart        *csr;
      
      serial_params     params;
      
      
      advanced_features AdvancedInit;           /*!< UART Advanced Features initialization parameters */
      
      const uint8_t            *pTxBuffPtr;              /*!< Pointer to UART Tx transfer Buffer */
      
      uint16_t                 TxXferSize;               /*!< UART Tx Transfer size              */
      
      volatile uint16_t            TxXferCount;              /*!< UART Tx Transfer Counter           */

      uint8_t                  *pRxBuffPtr;              /*!< Pointer to UART Rx transfer Buffer */
      
      uint16_t                 RxXferSize;               /*!< UART Rx Transfer size              */
      
      volatile uint16_t            RxXferCount;              /*!< UART Rx Transfer Counter           */
      
      uint16_t                 Mask;                     /*!< UART Rx RDR register mask          */
      
      uint32_t                 FifoMode;                 /*!< Specifies if the FIFO mode is being used.
							   This parameter can be a value of @ref UARTEx_FIFO_mode. */
      
      uint16_t                 NbRxDataToProcess;        /*!< Number of data to process during RX ISR execution */
      
      uint16_t                 NbTxDataToProcess;        /*!< Number of data to process during TX ISR execution */
      
      volatile rx_type  ReceptionType;         /*!< Type of ongoing reception          */
      
      volatile rx_event_type RxEventType;      /*!< Type of Rx Event                   */
      
      void (*RxISR)(struct __UART_HandleTypeDef *huart); /*!< Function pointer on Rx IRQ handler */
      
      void (*TxISR)(struct __UART_HandleTypeDef *huart); /*!< Function pointer on Tx IRQ handler */
      
#if defined(HAL_DMA_MODULE_ENABLED)
      DMA_HandleTypeDef        *hdmatx;                  /*!< UART Tx DMA Handle parameters      */
      
      DMA_HandleTypeDef        *hdmarx;                  /*!< UART Rx DMA Handle parameters      */
      
#endif /* HAL_DMA_MODULE_ENABLED */
      lock           Lock;                    /*!< Locking object                     */
      
      volatile state    gState;              /*!< UART state information related to global Handle management
					       and also related to Tx operations. This parameter
					       can be a value of @ref HAL_UART_StateTypeDef */
      
      volatile state    RxState;             /*!< UART state information related to Rx operations. This
							       parameter can be a value of @ref HAL_UART_StateTypeDef */
      
      volatile uint32_t  ErrorCode;           /*!< UART Error code                    */
    };
  }  
}

#if 0

namespace usart
{
  enum class wordlength {
    SEVEN,
    EIGHT,
    NINE
  };

  enum class stopbits {
    ONE,
    ONE_POINT_FIVE,
    TWO
  };

  enum class parity {
    NONE,
    EVEN,
    ODD
  };

  enum class pinmode {
    TX,
    RX,
    TX_RX
  };
  
  enum class flow_control {
    NONE,
    HW
  };

  class USARTBase
  {
  protected:
    volatile CSR::usart *csr;
    void (*set_clk_sel)(uint32_t v);
    void (*set_clk_en)(bool b);
    uint32_t br;
    wordlength wl;
    parity par;
    stopbits sb;
    pinmode mode;
    flow_control fc;

    void update_serial();
    
  public:
    USARTBase(volatile CSR::usart *_csr,
              void (*_set_clk_sel)(uint32_t v),
              void (*_set_clk_en)(bool b)) :
      csr(_csr),
      set_clk_sel(_set_clk_sel),
      set_clk_en(_set_clk_en)
    {
    }

    void init(uint32_t _baud_rate,
              wordlength _wl = wordlength::EIGHT,
	      parity _par = parity::NONE,
              stopbits _sb = stopbits::ONE,
              pinmode _mode = pinmode::TX_RX,
              flow_control _fc = flow_control::NONE,
              uint32_t oversampling = 16,
              bool one_bit_sampling = false,
              uint32_t divider = 1,
              bool advanced = false);

    void _enable();
    void _disable();
  
    parity get_parity() const;
    void set_parity(parity);
    
    wordlength get_wordlength() const;
    void set_wordlength(wordlength);
    
    stopbits get_stopbits() const;
    void set_parity(stopbits);
    
    flow_control get_hw_flow_control();
    void set_hw_flow_control(flow_control);
  };
  
  template <int n>
  class USART : public USARTBase
  {
  protected:
    volatile CSR::usart *csr;
    
  public:
    USART() : USARTBase(std::get<n>(CSR::periph_nsec.usarts).ptr(),
                        clocking::usart_clk_sel<n>::set,
                        std::get<n>(clocking::enables::usart).set) {}
  };

  extern USART<1> usart1;
};

#endif
