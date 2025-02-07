#if 1	//new
#include "csp.h"

/**
  * @brief  config csp as uart mode.
  * @param  ulBase is the base address of the csp module.
  * @param  ulPclk is the peripheral clock.
  * @param  ulBaudrate is the uart baudrate.
  * @param  ucDatabits is the uart frame data bits.
  * @param  ucParityCheck is the uart parity check:
  *   This parameter can be one of the following values:
  *        CSP_UART_PARITYCHECK_None
  *        CSP_UART_PARITYCHECK_Even
  *        CSP_UART_PARITYCHECK_Odd
  * @param  ucStopbits is the uart frame stop bits.
  * @retval None
  */
void CSP_UARTModeSet(CSP_TypeDef* CSPx, uint32_t ulPclk, uint32_t ulBaudrate, uint8_t ucDatabits, uint32_t ulParityCheck, uint8_t ucStopbits)
{
	CSP_OperationModeSet(CSPx, CSP_MODE1_ASYN);

	CSP_EndianModeSet(CSPx, CSP_MODE1_ENDIAN_LSBF);

	CSP_ClockSet(CSPx, CSP_MODE_UART, ulPclk, ulBaudrate);

    if (CSP_UART_PARITYCHECK_None == ulParityCheck) {
        CSP_TxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1, ucDatabits + 1 + ucStopbits);

	    CSP_RxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1 + ucStopbits);
    } else {
        CSP_TxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1, ucDatabits + 2 + ucStopbits);

        CSP_RxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 2 + ucStopbits);
    }

    CSPx->INT_STATUS = CSP_INT_ALL;

    CSPx->TX_DMA_IO_CTRL = CSP_TX_DMA_IO_CTRL_IO_MODE;

    CSPx->RX_DMA_IO_CTRL = CSP_RX_DMA_IO_CTRL_IO_MODE;

    CSPx->TX_DMA_IO_LEN = 0;

    CSPx->RX_DMA_IO_LEN = 0;

    CSPx->TX_FIFO_CTRL = CSP_TX_FIFO_CTRL_WIDTH_BYTE;
    CSPx->RX_FIFO_CTRL = CSP_TX_FIFO_CTRL_WIDTH_BYTE;

    //CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~CSP_AYSNC_PARAM_UART_PARITYCHECK_Msk) | (ulParityCheck);
    CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~(CSP_AYSNC_PARAM_UART_PARITYCHECK_Msk | CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Msk)) | \
    (ulParityCheck | CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Msk);//配置奇偶校验，配置超时机制在RXFIFO非空且RX_IDLE时有效

    // CSP FIFO Enable
    CSPx->TX_RX_ENABLE = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_RESET;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_START;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_START;

    // CSP Enable
    CSPx->MODE1 |= CSP_MODE1_CSP_EN;
}

/**
  * @brief  config csp as uart mode, disbale tx and rx.
  * @param  ulBase is the base address of the csp module.
  * @param  ulPclk is the peripheral clock.
  * @param  ulBaudrate is the uart baudrate.
  * @param  ucDatabits is the uart frame data bits.
  * @param  ucParityCheck is the uart parity check:
  *   This parameter can be one of the following values:
  *        CSP_UART_PARITYCHECK_None
  *        CSP_UART_PARITYCHECK_Even
  *        CSP_UART_PARITYCHECK_Odd
  * @param  ucStopbits is the uart frame stop bits.
  * @retval None
  */
void CSP_UARTModeSet2(CSP_TypeDef* CSPx, uint32_t ulPclk, uint32_t ulBaudrate, uint8_t ucDatabits, uint32_t ulParityCheck, uint8_t ucStopbits)
{
	CSP_OperationModeSet(CSPx, CSP_MODE1_ASYN);

	CSP_EndianModeSet(CSPx, CSP_MODE1_ENDIAN_LSBF);

	CSP_ClockSet(CSPx, CSP_MODE_UART, ulPclk, ulBaudrate);

    if (CSP_UART_PARITYCHECK_None == ulParityCheck) {
        CSP_TxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1, ucDatabits + 1 + ucStopbits);

	    CSP_RxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1 + ucStopbits);
    } else {
        CSP_TxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1, ucDatabits + 2 + ucStopbits);

        CSP_RxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 2 + ucStopbits);
    }

    CSPx->INT_STATUS = CSP_INT_ALL;

    CSPx->TX_DMA_IO_CTRL = CSP_TX_DMA_IO_CTRL_IO_MODE;

    CSPx->RX_DMA_IO_CTRL = CSP_RX_DMA_IO_CTRL_IO_MODE;

    CSPx->TX_DMA_IO_LEN = 0;

    CSPx->RX_DMA_IO_LEN = 0;

    CSPx->TX_FIFO_CTRL = CSP_TX_FIFO_CTRL_WIDTH_BYTE;
    CSPx->RX_FIFO_CTRL = CSP_TX_FIFO_CTRL_WIDTH_BYTE;

    //CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~CSP_AYSNC_PARAM_UART_PARITYCHECK_Msk) | (ulParityCheck);
    CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~(CSP_AYSNC_PARAM_UART_PARITYCHECK_Msk | CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Msk)) | \
    (ulParityCheck | CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Msk);//配置奇偶校验，配置超时机制在RXFIFO非空且RX_IDLE时有效

    // CSP FIFO Enable
    //CSPx->TX_RX_ENABLE = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_RESET;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_START;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_START;

    // CSP Enable
    CSPx->MODE1 |= CSP_MODE1_CSP_EN;
}

void CSP_UARTAutoBaudrate(CSP_TypeDef* CSPx, uint32_t ulCount, uint8_t ucDatabits,  uint32_t ulParityCheck, uint8_t ucStopbits)
{
	uint32_t ulSampleDiv;
    uint32_t ulCSPClkDiv = 0;
    uint32_t diff;
    uint32_t minSampleDiv = 0;
    uint32_t minDiff = 0x0FFFFFFF;
    uint32_t ulCountCal;

	CSP_OperationModeSet(CSPx, CSP_MODE1_ASYN);
	CSP_EndianModeSet(CSPx, CSP_MODE1_ENDIAN_LSBF);

	for(ulSampleDiv = 0x40; ulSampleDiv > 3; ulSampleDiv--)
	{
		ulCSPClkDiv = (2* ulCount / ulSampleDiv + 1)/2 - 1 ;

		if(ulCSPClkDiv == 0 || ulCSPClkDiv >= 0x1000 || (ulCSPClkDiv + 1)*ulSampleDiv < 16)
		{
			continue;
		}

		ulCountCal = ulSampleDiv * (ulCSPClkDiv + 1);
		if(ulCount >= ulCountCal)
		{
			diff = ulCount - ulCountCal;
		}
		else
		{
			diff = ulCountCal - ulCount;
		}

		if(diff < minDiff)
		{
			minDiff = diff;
			minSampleDiv = ulSampleDiv;
		}

		if(diff == 0)
		{
			break;
		}

	}

	ulSampleDiv = minSampleDiv;
	ulCSPClkDiv = (2 * ulCount / ulSampleDiv + 1) / 2 - 1;
	CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~CSP_AYSNC_PARAM_DIV2_Msk) | ((ulSampleDiv - 1) << CSP_AYSNC_PARAM_DIV2_Pos);
	CSPx->CLOCK_DIVISOR = ulCSPClkDiv;

    if (CSP_UART_PARITYCHECK_None == ulParityCheck) {
        CSP_TxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1, ucDatabits + 1 + ucStopbits);

	    CSP_RxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1 + ucStopbits);
    } else {
        CSP_TxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1, ucDatabits + 2 + ucStopbits);

        CSP_RxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 2 + ucStopbits);
    }
    CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~CSP_AYSNC_PARAM_UART_PARITYCHECK_Msk) | (ulParityCheck);

    CSPx->INT_STATUS = CSP_INT_ALL;

    CSPx->TX_DMA_IO_CTRL = CSP_TX_DMA_IO_CTRL_IO_MODE;
    CSPx->RX_DMA_IO_CTRL = CSP_RX_DMA_IO_CTRL_IO_MODE;

    CSPx->TX_DMA_IO_LEN = 0;
    CSPx->RX_DMA_IO_LEN = 0;

    CSPx->TX_FIFO_CTRL = CSP_TX_FIFO_CTRL_WIDTH_BYTE;
    CSPx->RX_FIFO_CTRL = CSP_TX_FIFO_CTRL_WIDTH_BYTE;

    // CSP FIFO Enable
    CSPx->TX_RX_ENABLE = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_RESET;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_START;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_START;

    // CSP Enable
    CSPx->MODE1 |= CSP_MODE1_CSP_EN;
}


#if 1
void CSPIrdaMode(CSP_TypeDef* CSPx, uint32_t ulPclk, uint32_t ulBaudrate, uint8_t ucDatabits, uint8_t ucStopbits, uint32_t ulIdleLevel, uint32_t ulWidth)
{
    CSP_OperationModeSet(CSPx, CSP_MODE1_ASYN);

    CSP_EndianModeSet(CSPx, CSP_MODE1_ENDIAN_LSBF);

    CSP_ClockSet(CSPx, CSP_MODE_IRDA, ulPclk, ulBaudrate);

    CSP_IrdaIdleLevelSet(CSPx, ulIdleLevel);

    CSP_IrdaPulseWidthSet(CSPx, ulWidth);

    CSP_IrdaEnable(CSPx, CSP_MODE1_HPSIR_EN);

    CSP_TxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1, ucDatabits + 1 + ucStopbits);

    CSP_RxFrameSet(CSPx, 1, ucDatabits, ucDatabits + 1 + ucStopbits);

    CSPx->INT_STATUS = CSP_INT_ALL;

    CSPx->TX_DMA_IO_CTRL = CSP_TX_DMA_IO_CTRL_IO_MODE;

    CSPx->RX_DMA_IO_CTRL = CSP_RX_DMA_IO_CTRL_IO_MODE;

    CSPx->TX_DMA_IO_LEN = 0;

    CSPx->RX_DMA_IO_LEN = 0;

    // CSP FIFO Enable
    CSPx->TX_RX_ENABLE = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_RESET;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_START;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_START;

    // CSP Enable
    CSPx->MODE1 |= CSP_MODE1_CSP_EN;
}
#endif

void CSP_SPIMode(CSP_TypeDef* CSPx, uint32_t ulMode1, uint32_t ulMode2, uint32_t ulTxFrameCtl, uint32_t ulRxFrameCtl)
{
    // clear all pending uart interrupts
    CSPx->INT_STATUS = CSP_INT_ALL;

    CSPx->MODE1 = ulMode1;

    CSPx->MODE2 = ulMode2;

    CSPx->TX_DMA_IO_CTRL = CSP_TX_DMA_IO_CTRL_IO_MODE;

    CSPx->RX_DMA_IO_CTRL = CSP_RX_DMA_IO_CTRL_IO_MODE;

    CSPx->TX_DMA_IO_LEN = 0;

    CSPx->RX_DMA_IO_LEN = 0;

    CSPx->TX_FRAME_CTRL = ulTxFrameCtl;

    CSPx->RX_FRAME_CTRL = ulRxFrameCtl;

    // CSP FIFO Enable
    CSPx->TX_RX_ENABLE = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_RESET;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_START;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_START;

    // CSP Enable
    CSPx->MODE1 |= CSP_MODE1_CSP_EN;
}

/**
  * @brief  config csp fifo level.
  * @param  ulBase is the base address of the csp module.
  * @param  ulTxFifo is the tx fifo level.
  * @param  ulRxFifo is the rx fifo level.
  * @retval None
  */
void CSP_FIFOLevelSet(CSP_TypeDef* CSPx, uint32_t ulTxFifo, uint32_t ulRxFifo)
{
    CSPx->TX_FIFO_CTRL = ulTxFifo;
    CSPx->RX_FIFO_CTRL = ulRxFifo;
}

/**
  * @brief  config csp rxfifo level.
  * @param  ulBase is the base address of the csp module.
  * @param  ulRxFifo is the rx fifo level.
  * @retval None
  * @note 大于寄存器值触发中断，即(ulRxFifo)
  */
void CSP_RXFIFO_LevelSet(CSP_TypeDef* CSPx, uint32_t ulRxFifo)
{
    CSPx->RX_FIFO_CTRL = (CSPx->RX_FIFO_CTRL & ~CSP_RX_FIFO_CTRL_THD_Msk) | (ulRxFifo << 2);
}

/**
  * @brief  config csp txfifo level.
  * @param  ulBase is the base address of the csp module.
  * @param  ulTxFifo is the tx fifo level.
  * @retval None
  * @note 小于寄存器值触发中断，即(ulTxFifo)
  */
void CSP_TXFIFO_LevelSet(CSP_TypeDef* CSPx, uint32_t ulTxFifo)
{
    CSPx->TX_FIFO_CTRL = (CSPx->TX_FIFO_CTRL & ~CSP_TX_FIFO_CTRL_THD_Msk) | (ulTxFifo << 2);
}

/**
  * @brief  get a data.
  * @param  ulBase is the base address of the csp module.
  * @retval a data
  */
uint32_t CSP_CharGet(CSP_TypeDef* CSPx)
{
    while(CSPx->RX_FIFO_STATUS & CSP_RX_FIFO_STATUS_EMPTY_Msk)
    {
    }

    return(CSPx->RX_FIFO_DATA);
}

/**
  * @brief  put a data.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSP_CharPut(CSP_TypeDef* CSPx, uint32_t ucData)
{
    while(CSPx->TX_FIFO_STATUS & CSP_TX_FIFO_STATUS_FULL_Msk)
    {
    }

    CSPx->TX_FIFO_DATA = ucData;
}

/**
  * @brief Gets the RX FIFO status of csp.
  * @note This function is getting the current Receiver FIFO status.
  * @param ulBase: the base address of g_at_lpuart module.
  * @param  ucFlagType specifies the flag to check.
  *   This parameter can be one of the following values:
  *   @arg CSP_FIFO_EMPTY: UART Fifo empty flag
  *   @arg CSP_FIFO_FULL: UART Fifo full flag
  *   @arg CSP_FIFO_LEVEL: The Fifo bytes data number left;when fifo is full, this is 0
  * @retval The current fifo state
  */
uint8_t CSP_RxFifoStatusGet(CSP_TypeDef* CSPx, uint8_t ucFlagType)
{
	if(ucFlagType == CSP_FIFO_EMPTY)
	{
		return ((CSPx->RX_FIFO_STATUS & CSP_RX_FIFO_STATUS_EMPTY_Msk) ? 1 : 0);
	}
	else if(ucFlagType == CSP_FIFO_FULL)
	{
		return ((CSPx->RX_FIFO_STATUS & CSP_RX_FIFO_STATUS_FULL_Msk) ? 1 : 0);
	}
	else
	{
		return ((CSPx->RX_FIFO_STATUS & CSP_RX_FIFO_STATUS_LEVEL_Msk));
	}
}

/**
  * @brief Gets the TX FIFO status of csp.
  * @note This function is getting the current Transmitter FIFO status.
  * @param ulBase: the base address of g_at_lpuart module.
  * @param  ucFlagType specifies the flag to check.
  *   This parameter can be one of the following values:
  *   @arg CSP_FIFO_EMPTY: UART Fifo empty flag
  *   @arg CSP_FIFO_FULL: UART Fifo full flag
  *   @arg CSP_FIFO_LEVEL: The Fifo bytes data number left;when fifo is full, this is 0
  * @retval The current fifo state
  */
uint8_t CSP_TxFifoStatusGet(CSP_TypeDef* CSPx, uint8_t ucFlagType)
{
	if(ucFlagType == CSP_FIFO_EMPTY)
	{
		return ((CSPx->TX_FIFO_STATUS & CSP_TX_FIFO_STATUS_EMPTY_Msk) ? 1 : 0);
	}
	else if(ucFlagType == CSP_FIFO_FULL)
	{
		return ((CSPx->TX_FIFO_STATUS & CSP_TX_FIFO_STATUS_FULL_Msk) ? 1 : 0);
	}
	else
	{
		return ((CSPx->TX_FIFO_STATUS & CSP_TX_FIFO_STATUS_LEVEL_Msk));
	}
}

/**
  * @brief  get fifo data no blocking.
  * @param  ulBase is the base address of the csp module.
  * @retval the fifo data if fifo not empty , else -1: error
  */
int32_t CSP_CharGetNonBlocking(CSP_TypeDef* CSPx)
{
    if(!(CSPx->RX_FIFO_STATUS & CSP_RX_FIFO_STATUS_EMPTY_Msk))
    {
        return(CSPx->RX_FIFO_DATA);
    }
    else
    {
        return (-1);
    }
}

/**
  * @brief  put fifo data no blocking.
  * @param  ulBase is the base address of the csp module.
  * @param  ucData is the data need putting into txfifo.
  * @retval true if fifo not full , else false if fifo full.
  */
int32_t CSP_CharPutNonBlocking(CSP_TypeDef* CSPx, uint8_t ucData)
{
    if(!(CSPx->TX_FIFO_STATUS & CSP_TX_FIFO_STATUS_FULL_Msk))
    {
        CSPx->TX_FIFO_DATA = ucData;

        return (true);
    }
    else
    {
        return (false);
    }
}

/**
  * @brief  config csp fifo level.
  * @param  ulBase is the base address of the csp module.
  * @param  ulTxFifo is the tx fifo level.
  * @param  ulRxFifo is the rx fifo level.
  * @retval None
  */
void CSP_OperationModeSet(CSP_TypeDef* CSPx, uint32_t ulOperationMode)
{
    CSPx->MODE1 = (CSPx->MODE1 & ~CSP_MODE1_SYNC_MODE_Msk) | ulOperationMode;
}

/**
  * @brief  config csp clock mode.
  * @param  ulBase is the base address of the csp module.
  * @param  ulClockMode is the clock mode as master or slave:
  *         CSP_MODE1_CLOCK_MODE_Master
  *         CSP_MODE1_CLOCK_MODE_Slave
  * @retval None
  */
void CSP_ClockModeSet(CSP_TypeDef* CSPx, uint32_t ulClockMode)
{
    CSPx->MODE1 = (CSPx->MODE1 & ~CSP_MODE1_CLOCK_MODE_Msk) | ulClockMode;
}

void CSP_IrdaEnable(CSP_TypeDef* CSPx, uint32_t ulIrdaEn)
{
    CSPx->MODE1 = (CSPx->MODE1 & ~CSP_MODE1_HPSIR_EN_Msk) | ulIrdaEn;
}

/**
  * @brief  config csp endian mode.
  * @param  ulBase is the base address of the csp module.
  * @param  ulEndianMode is the endian mode as one of follows:
  *         CSP_MODE1_ENDIAN_MSBF
  *         CSP_MODE1_ENDIAN_LSBF
  * @retval None
  */
void CSP_EndianModeSet(CSP_TypeDef* CSPx, uint32_t ulEndianMode)
{
    CSPx->MODE1 = (CSPx->MODE1 & ~CSP_MODE1_ENDIAN_CTRL_Msk) | ulEndianMode;
}

/**
  * @brief  enable csp module.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSP_Enable(CSP_TypeDef* CSPx)
{
    CSPx->MODE1 |= CSP_MODE1_CSP_EN;
}

/**
  * @brief  disable csp module.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSP_Disable(CSP_TypeDef* CSPx)
{
    CSPx->MODE1 &= (~CSP_MODE1_CSP_EN);
}

/**
  * @brief  config csp rx and tx driven edge.
  * @param  ulBase is the base address of the csp module.
  * @param  ulRxEdge is the rx driven edge as one of follows:
  *         CSP_MODE1_RXD_ACT_RISE
  *         CSP_MODE1_RXD_ACT_FALL
  * @param  ulTxEdge is the tx driven edge as one of follows:
  *         CSP_MODE1_TXD_ACT_RISE
  *         CSP_MODE1_TXD_ACT_FALL
  * @retval None
  */
void CSP_DrivenEdgeSet(CSP_TypeDef* CSPx, uint32_t ulRxEdge, uint32_t ulTxEdge)
{
    CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_RXD_ACT_EDGE_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk)) | (ulRxEdge | ulTxEdge);
}

void CSP_SyncValidLevelSet(CSP_TypeDef* CSPx, uint32_t ulRFS, uint32_t ulTFS)
{
    CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_RFS_ACT_LEVEL_Msk | CSP_MODE1_TFS_ACT_LEVEL_Msk)) | (ulRFS | ulTFS);
}

void CSP_ClockIdleModeSet(CSP_TypeDef* CSPx, uint32_t ulClkIdleMode)
{
    CSPx->MODE1 = (CSPx->MODE1 & ~CSP_MODE1_CLK_IDLE_MODE_Msk) | ulClkIdleMode;
}

void CSP_ClockIdleLevelSet(CSP_TypeDef* CSPx, uint32_t ulClkIdleLevel)
{
    CSPx->MODE1 = (CSPx->MODE1 & ~CSP_MODE1_CLK_IDLE_LEVEL_Msk) | ulClkIdleLevel;
}

void CSP_PinModeSet(CSP_TypeDef* CSPx, uint32_t ulCSPPin, uint32_t ulPinMode, uint32_t ulPinDirection)
{
    if(ulCSPPin != CSP_PIN_CLK && ulCSPPin != CSP_PIN_RFS && ulCSPPin != CSP_PIN_TFS && ulCSPPin != CSP_PIN_RXD && ulCSPPin != CSP_PIN_TXD)
    {
        return ;
    }

    CSPx->MODE1 = (CSPx->MODE1 & ~((1 << ulCSPPin) | (1 << (ulCSPPin+5)))) | (ulPinMode << ulCSPPin) | (ulPinDirection << (ulCSPPin+5));
}

void CSP_IrdaPulseWidthSet(CSP_TypeDef* CSPx, uint32_t ulWidth)
{
    CSPx->MODE2 = (CSPx->MODE2 & (~CSP_MODE2_IRDA_DATA_WIDTH_Msk)) | ulWidth;
}

void CSP_IrdaIdleLevelSet(CSP_TypeDef* CSPx, uint32_t ulIrdaIdleLevel)
{
    CSPx->MODE1 = (CSPx->MODE1 & ~CSP_MODE1_IrDA_IDLE_LEVEL_Msk) | ulIrdaIdleLevel;
}

/**
  * @brief  config csp peripheral clock and baudrate.
  * @param  ulBase is the base address of the csp module.
  * @param  ulCSPMode is the csp mode as one of follows:
  *         CSP_MODE_UART
  *         CSP_MODE_IRDA
  *         CSP_MODE_SPI_Master
  *         CSP_MODE_SPI_Slave
  *         CSP_MODE_SIM
  * @param  ulPclk is the peripheral clock.
  * @param  ulClock is the baudrate.
  * @retval None
  */
void CSP_ClockSet(CSP_TypeDef* CSPx, uint32_t ulCSPMode, uint32_t ulPclk, uint32_t ulClock)
{
    uint32_t ulSampleDiv;
    uint32_t ulCSPClkDiv = 0;
    uint32_t ulIrdaWidthDiv;

    uint32_t diff;
    uint32_t minSampleDiv = 16;
    uint32_t minDiff = 0x0FFFFFFF;
    uint32_t ulPclkCalculate;

    if(ulCSPMode == CSP_MODE_UART)
    {
        //
        // Calculate the best ulSampleDiv and ulCSPClkDiv.
        //
        for(ulSampleDiv = 0x40; ulSampleDiv > 3; ulSampleDiv--)
        {
            ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulClock + 1) / 2 - 1;

            if(ulCSPClkDiv >= 0x1000 || (ulCSPClkDiv + 1)*ulSampleDiv < 16)
            {
                continue;
            }

			ulPclkCalculate = ulClock * ulSampleDiv * (ulCSPClkDiv + 1);
			if(ulPclk >= ulPclkCalculate)
			{
				diff = ulPclk - ulPclkCalculate;
			}
			else
			{
				diff = ulPclkCalculate - ulPclk;
			}

            if(diff < minDiff)
            {
                minDiff = diff;
                minSampleDiv = ulSampleDiv;
            }

			if(diff == 0)
			{
				break;
			}

        }

        ulSampleDiv = minSampleDiv;

        ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulClock + 1) / 2 - 1;

        CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~CSP_AYSNC_PARAM_DIV2_Msk) | ((ulSampleDiv - 1) << CSP_AYSNC_PARAM_DIV2_Pos);
    }
    else if(ulCSPMode == CSP_MODE_IRDA)
    {
        ulSampleDiv = 16;

        ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulClock + 1) / 2 - 1;

        ulIrdaWidthDiv = 16 * ulPclk / 10000000 + 1;

        CSPx->MODE1 = (CSPx->MODE1 & ~CSP_MODE1_IRDA_WIDTH_DIV_Msk) | (ulIrdaWidthDiv << CSP_MODE1_IRDA_WIDTH_DIV_Pos);

        CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~CSP_AYSNC_PARAM_DIV2_Msk) | ((ulSampleDiv - 1) << CSP_AYSNC_PARAM_DIV2_Pos);
    }
    else if(ulCSPMode == CSP_MODE_SPI_Master)
    {
        ulCSPClkDiv = ulPclk / ulClock / 2 - 1;
    }
	else if(ulCSPMode == CSP_MODE_SPI_Slave)
	{
		ulCSPClkDiv = 4;
	}
    else if(ulCSPMode == CSP_MODE_SIM)
    {
        // ulCSPClkDiv = ;
    }

    CSPx->CLOCK_DIVISOR = ulCSPClkDiv;
}


void CSP_HwDetectClockSet(CSP_TypeDef* CSPx, uint32_t ulPclk, uint32_t ulBaudrate)
{
    uint32_t ulSampleDiv;
    uint32_t ulCSPClkDiv;

    uint32_t diff;
    uint32_t minSampleDiv = 16;
    uint32_t minDiff = 0x0FFFFFFF;
    uint32_t ulPclkCalculate;

	//
	// Calculate the best ulSampleDiv and ulCSPClkDiv.
	//
	for(ulSampleDiv = 8; ulSampleDiv > 3; ulSampleDiv--)
	{
		ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulBaudrate + 1) / 2 - 1;

		if(ulCSPClkDiv >= 0x1000 || (ulCSPClkDiv + 1)*ulSampleDiv < 16)
		{
			continue;
		}

		ulPclkCalculate = ulBaudrate * ulSampleDiv * (ulCSPClkDiv + 1);

		if(ulPclk >= ulPclkCalculate)
		{
			diff = ulPclk - ulPclkCalculate;
		}
		else
		{
			diff = ulPclkCalculate - ulPclk;
		}

		if(diff < minDiff)
		{
			minDiff = diff;
			minSampleDiv = ulSampleDiv;
		}

		if(diff == 0)
		{
			break;
		}

	}

	ulSampleDiv = minSampleDiv;

	ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulBaudrate + 1) / 2 - 1;

	CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~CSP_AYSNC_PARAM_DIV2_Msk) | ((ulSampleDiv - 1) << CSP_AYSNC_PARAM_DIV2_Pos);

	CSPx->CLOCK_DIVISOR = ulCSPClkDiv;
}

/**
  * @brief  config csp tx frame parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  ulDelay is the tx frame delay
  * @param  ulDataLen is the transmitted data length in one frame.
  * @param  ulSyncLen is the valid length of transmitted sync signal (TFS) in one frame.
  *   This parameter can be one of the following values:
  *         CSP_TX_FRAME_CTRL_TX_SYNC_BITS_8
  *         CSP_TX_FRAME_CTRL_TX_SYNC_BITS_9
  * @param  ulFrameLen is the transmit frame length include active state and idle state.
  * @retval None
  */
void CSP_TxFrameSet(CSP_TypeDef* CSPx, uint32_t ulDelay, uint32_t ulDataLen, uint32_t ulSyncLen, uint32_t ulFrameLen)
{
    CSPx->MODE2 = (CSPx->MODE2 & ~CSP_MODE2_TXD_DELAY_LEN_Msk) | (ulDelay << CSP_MODE2_TXD_DELAY_LEN_Pos);
    CSPx->TX_FRAME_CTRL = (CSPx->TX_FRAME_CTRL & 0xE0000000) | (ulDataLen - 1) | (ulSyncLen - 1) << CSP_TX_FRAME_CTRL_TX_SYNC_LEN_Pos | (ulFrameLen - 1) << CSP_TX_FRAME_CTRL_TX_FRAME_LEN_Pos | (ulDataLen - 1) << CSP_TX_FRAME_CTRL_TX_SHIFTER_LEN_Pos;
}

/**
  * @brief  config csp rx frame parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  ulDelay is the rx frame delay
  * @param  ulDataLen is the received data length in one frame.
  * @param  ulFrameLen is the received frame length include active and idle state.
  * @retval None
  */
void CSP_RxFrameSet(CSP_TypeDef* CSPx, uint32_t ulDelay, uint32_t ulDataLen, uint32_t ulFrameLen)
{
    CSPx->MODE2 = (CSPx->MODE2 & ~CSP_MODE2_RXD_DELAY_LEN_Msk) | (ulDelay << CSP_MODE2_RXD_DELAY_LEN_Pos);
    CSPx->RX_FRAME_CTRL = (CSPx->RX_FRAME_CTRL & 0xFFE00000) | (ulDataLen - 1) | (ulFrameLen - 1) << CSP_RX_FRAME_CTRL_RX_FRAME_LEN_Pos | (ulDataLen - 1) << CSP_RX_FRAME_CTRL_RX_SHIFTER_LEN_Pos;
}

void CSP_RxEnable(CSP_TypeDef* CSPx)
{
    CSPx->TX_RX_ENABLE |= CSP_TX_RX_ENABLE_RX_ENA;
}

void CSP_RxDisable(CSP_TypeDef* CSPx)
{
    CSPx->TX_RX_ENABLE &= ~CSP_TX_RX_ENABLE_RX_ENA;
}

uint8_t CSP_RxStatusGet(CSP_TypeDef* CSPx)
{
    return (CSPx->TX_RX_ENABLE & CSP_TX_RX_ENABLE_RX_ENA);
}

void CSP_TxEnable(CSP_TypeDef* CSPx)
{
    CSPx->TX_RX_ENABLE |= CSP_TX_RX_ENABLE_TX_ENA;
}

void CSP_TxDisable(CSP_TypeDef* CSPx)
{
    CSPx->TX_RX_ENABLE &= ~CSP_TX_RX_ENABLE_TX_ENA;
}


void CSP_IntRegister(uint32_t ulIntChannel, uint32_t *g_pRAMVectors, void(*pfnHandler)(void))
{

	IntRegister(ulIntChannel, g_pRAMVectors, pfnHandler);

	IntEnable(ulIntChannel);
}

void CSP_IntUnregister(uint32_t ulIntChannel, uint32_t *g_pRAMVectors)
{
	IntDisable(ulIntChannel);

	IntUnregister(ulIntChannel, g_pRAMVectors);
}

void CSP_IntEnable(CSP_TypeDef* CSPx, uint32_t ulIntFlags)
{
    CSPx->INT_ENABLE |= ulIntFlags;
}

void CSP_IntDisable(CSP_TypeDef* CSPx, uint32_t ulIntFlags)
{
    CSPx->INT_ENABLE &= (~ulIntFlags);
}

uint32_t CSP_IntStatus(CSP_TypeDef* CSPx)
{
    return CSPx->INT_STATUS;
}

void CSP_IntClear(CSP_TypeDef* CSPx, uint32_t ulIntFlags)
{
    CSPx->INT_STATUS = ulIntFlags;
    // 读一次中断状态寄存器，保证清中断操作立即生效
    CSPx->INT_STATUS;
}


void CSP_SPIConfigSetExpClk(CSP_TypeDef* CSPx, uint32_t ulPclk, uint32_t ulClock, uint8_t ulMode, uint8_t ucClkPol, uint8_t ucClkPha, uint8_t dataWidth)
{
	uint8_t tx_data_len = 0, tx_sync_len = 0, tx_frm_len = 0, tx_shifter_len = 0;
	uint8_t rx_data_len = 0, rx_frm_len = 0, rx_shifter_len = 0;
	uint32_t ulTmp = 0;

	if(ulMode == CSP_MODE1_CLOCK_MODE_Master)
	{
		CSP_ClockSet(CSPx, CSP_MODE_SPI_Master, ulPclk, ulClock);

		if(ucClkPol == 0 && ucClkPha == 0) {
			ulTmp = CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL | CSP_MODE1_CLK_IDLE_LOGIC0;
		} else if(ucClkPol == 0 && ucClkPha == 1) {
			ulTmp = CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE | CSP_MODE1_CLK_IDLE_LOGIC0;
		} else if(ucClkPol == 1 && ucClkPha == 0) {
			ulTmp = CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE | CSP_MODE1_CLK_IDLE_LOGIC1;
		} else if(ucClkPol == 1 && ucClkPha == 1) {
			ulTmp = CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL | CSP_MODE1_CLK_IDLE_LOGIC1;
		}

		CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_SYNC_MODE_Msk | CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk | CSP_MODE1_CLK_IDLE_LEVEL_Msk)) | ulTmp;

		tx_data_len = dataWidth - 1;  //actual data bits - 1
		tx_shifter_len = tx_data_len;
		tx_sync_len = dataWidth + 1;  //tx_data_len + 2: add 1 setup bit and 1 hold bit
		tx_frm_len = dataWidth + 2;   //tx_data_len + 3: tx_sync_len + 1 idle bit

		rx_data_len = tx_data_len;
		rx_shifter_len = rx_data_len;
		rx_frm_len = tx_frm_len;

		//tx delay = 1; rx delay = 1
		CSPx->MODE2 = (CSPx->MODE2 & ~(CSP_MODE2_RXD_DELAY_LEN_Msk | CSP_MODE2_TXD_DELAY_LEN_Msk)) | ((1 << CSP_MODE2_RXD_DELAY_LEN_Pos) | (1 << CSP_MODE2_TXD_DELAY_LEN_Pos));

	}
	else if(ulMode == CSP_MODE1_CLOCK_MODE_Slave)
	{
		CSP_ClockSet(CSPx, CSP_MODE_SPI_Slave, ulPclk, ulClock);

		if(ucClkPol == 0 && ucClkPha == 0) {
			ulTmp = CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL;
			tx_frm_len = dataWidth + 2; //actual frame length - 1
		} else if(ucClkPol == 0 && ucClkPha == 1) {
			ulTmp = CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE;
			tx_frm_len = dataWidth; //tx_shitft_len + 1;
		} else if(ucClkPol == 1 && ucClkPha == 0) {
			ulTmp = CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE;
			tx_frm_len = dataWidth + 2; //actual frame length - 1
		} else if(ucClkPol == 1 && ucClkPha == 1) {
			ulTmp = CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL;
			tx_frm_len = dataWidth; //tx_shitft_len + 1;
		}

		CSPx->MODE1 = (CSPx->MODE1 &~(CSP_MODE1_SYNC_MODE_Msk | CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk)) | ulTmp;

		tx_data_len = dataWidth - 1;
		tx_sync_len = 0; // any value
		tx_shifter_len = tx_data_len; //The same as the data length

		rx_data_len = tx_data_len;
		rx_shifter_len = rx_data_len; //The same as the data length
		rx_frm_len = dataWidth + 1;   //tx_data_len + 2: add 1 idle bit

		/* tx delay = 0, rx delay = 1 */
		CSPx->MODE2 = (CSPx->MODE2 & ~(CSP_MODE2_RXD_DELAY_LEN_Msk | CSP_MODE2_TXD_DELAY_LEN_Msk)) | ((1 << CSP_MODE2_RXD_DELAY_LEN_Pos) | (0 << CSP_MODE2_TXD_DELAY_LEN_Pos));
		CSPx->MODE2 |= (CSP_MODE2_RFS_MS_MODE_Msk | CSP_MODE2_TFS_MS_MODE_Msk); //slave mode

		CSPx->SYNC_SLAVE_CFG = (1 << CSP_SYNC_SLAVE_CFG_RFSIN2TFSIN_Pos) | (1<<CSP_SYNC_SLAVE_CFG_IDLE_CLK_NUM_CFG_Pos);
		if((ucClkPol == 0 && ucClkPha == 0) || (ucClkPol == 1 && ucClkPha == 0))
		{
			CSPx->SYNC_SLAVE_CFG |= (1 << CSP_SYNC_SLAVE_CFG_FS_START_PLSGEN_Pos);
		}

		CSPx->TX_FRAME_CTRL |= CSP_TX_FRAME_CTRL_SLAVE_CLK_SAMPLE_Msk; //filter for glitch of slave clock
	}

	CSPx->TX_FRAME_CTRL = (CSPx->TX_FRAME_CTRL & ~(CSP_TX_FRAME_CTRL_TX_DATA_LEN_Msk | CSP_TX_FRAME_CTRL_TX_SYNC_LEN_Msk | CSP_TX_FRAME_CTRL_TX_FRAME_LEN_Msk | CSP_TX_FRAME_CTRL_TX_SHIFTER_LEN_Msk)) |
										(tx_data_len | (tx_sync_len << 8) | (tx_frm_len << 16) | (tx_shifter_len << 24));


	CSPx->RX_FRAME_CTRL = (CSPx->RX_FRAME_CTRL & ~(CSP_RX_FRAME_CTRL_RX_DATA_LEN_Msk | CSP_RX_FRAME_CTRL_RX_FRAME_LEN_Msk | CSP_RX_FRAME_CTRL_RX_SHIFTER_LEN_Msk)) |
										(rx_data_len | (rx_frm_len << 8) | (rx_shifter_len << 16));

    CSPx->TX_DMA_IO_CTRL |= CSP_TX_DMA_IO_CTRL_IO_MODE;
    CSPx->RX_DMA_IO_CTRL |= CSP_RX_DMA_IO_CTRL_IO_MODE;
    CSPx->TX_DMA_IO_LEN = 0;
    CSPx->RX_DMA_IO_LEN = 0;

	if(dataWidth == 8){
		CSPx->TX_FIFO_CTRL = (CSPx->TX_FIFO_CTRL & ~CSP_TX_FIFO_CTRL_WIDTH_Msk) | CSP_TX_FIFO_CTRL_WIDTH_BYTE;
		CSPx->RX_FIFO_CTRL = (CSPx->RX_FIFO_CTRL & ~CSP_RX_FIFO_CTRL_WIDTH_Msk) | CSP_RX_FIFO_CTRL_WIDTH_BYTE;
	}
	else if(dataWidth == 16){
		CSPx->TX_FIFO_CTRL = (CSPx->TX_FIFO_CTRL & ~CSP_TX_FIFO_CTRL_WIDTH_Msk) | CSP_TX_FIFO_CTRL_WIDTH_HWORD;
		CSPx->RX_FIFO_CTRL = (CSPx->RX_FIFO_CTRL & ~CSP_RX_FIFO_CTRL_WIDTH_Msk) | CSP_RX_FIFO_CTRL_WIDTH_HWORD;
	}
	else if(dataWidth == 32){
		CSPx->TX_FIFO_CTRL = (CSPx->TX_FIFO_CTRL & ~CSP_TX_FIFO_CTRL_WIDTH_Msk) | CSP_TX_FIFO_CTRL_WIDTH_WORD;
		CSPx->RX_FIFO_CTRL = (CSPx->RX_FIFO_CTRL & ~CSP_RX_FIFO_CTRL_WIDTH_Msk) | CSP_RX_FIFO_CTRL_WIDTH_WORD;
	}
}
#if 0 //�ɵ� zhoudao
void CSP_SPIConfigSetExpClk(CSP_TypeDef* CSPx, uint8_t ulMode, uint32_t ulPclk, uint32_t ulClock, uint8_t CPOL, uint8_t CPHA)
{
	uint32_t tx_data_len, tx_sync_len, tx_frm_len, tx_shifter_len;
	uint32_t rx_data_len, rx_frm_len, rx_shifter_len;

	if(ulMode == CSP_MODE1_CLOCK_MODE_Master) {
		if(CPOL == 0 && CPHA == 0) {
		    CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL);
			tx_frm_len = 11;
			rx_frm_len = 11;
		} else if(CPOL == 0 && CPHA == 1) {  //ok
		    CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE);
			tx_frm_len = 11;
			rx_frm_len = 11;
		} else if(CPOL == 1 && CPHA == 0) {
		    CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE | CSP_MODE1_CLK_IDLE_LEVEL_Msk);
			tx_frm_len = 11;
			rx_frm_len = 11;
		} else if(CPOL == 1 && CPHA == 1) {
		    CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL | CSP_MODE1_CLK_IDLE_LEVEL_Msk);
			tx_frm_len = 11;
			rx_frm_len = 11;
		}
		tx_data_len = 7; // 8 bit - 1
		tx_sync_len = 9; // add 1 setup bit and 1 hold bit
		tx_shifter_len = 7; //The same as the data length

		rx_data_len = 7;// 8 bit data
		rx_shifter_len = 7; //The same as the data length

		CSPx->MODE2 = (CSPx->MODE2 & ~(CSP_MODE2_RXD_DELAY_LEN_Msk | CSP_MODE2_TXD_DELAY_LEN_Msk)) |
		                            (1 << CSP_MODE2_RXD_DELAY_LEN_Pos) | (1 << CSP_MODE2_TXD_DELAY_LEN_Pos);

		CSP_ClockSet(CSPx, CSP_MODE_SPI_Master, ulPclk, ulClock);
	}
	else if(ulMode == CSP_MODE1_CLOCK_MODE_Slave)
	{
		if(CPOL == 0 && CPHA == 0) {
			CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL);
			tx_frm_len = 9;
			rx_frm_len = 9;
			CSPx->SYNC_SLAVE_CFG |= (1<<CSP_SYNC_SLAVE_CFG_FS_START_PLSGEN_Pos);
		} else if(CPOL == 0 && CPHA == 1) {
			CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE);
			tx_frm_len = 8;
			rx_frm_len = 9;
		} else if(CPOL == 1 && CPHA == 0) {
			CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE);
			tx_frm_len = 9;//9;
			rx_frm_len = 9;//8;
			CSPx->SYNC_SLAVE_CFG |= (1<<CSP_SYNC_SLAVE_CFG_FS_START_PLSGEN_Pos);
		} else if(CPOL == 1 && CPHA == 1) {
			CSPx->MODE1 = (CSPx->MODE1 & ~(CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL);
			tx_frm_len = 8;
			rx_frm_len = 9;
		}

		tx_data_len = 7; // 8 bit - 1
		tx_sync_len = 0; // any value
		tx_shifter_len = 7; //The same as the data length

		rx_data_len = 7;// 8 bit data
		rx_shifter_len = 7; //The same as the data length

		CSPx->MODE2 = (CSPx->MODE2& ~(CSP_MODE2_RXD_DELAY_LEN_Msk | CSP_MODE2_TXD_DELAY_LEN_Msk)) |
                           		    (1 << CSP_MODE2_RXD_DELAY_LEN_Pos) | (0 << CSP_MODE2_TXD_DELAY_LEN_Pos) |
		                            CSP_MODE2_RFS_MS_MODE_Msk | CSP_MODE2_TFS_MS_MODE_Msk ;

		CSPx->SYNC_SLAVE_CFG =(CSPx->SYNC_SLAVE_CFG & ~CSP_SYNC_SLAVE_CFG_IDLE_CLK_NUM_Msk) |
		                                    CSP_SYNC_SLAVE_CFG_RFSIN2TFSIN_Msk | (1<<CSP_SYNC_SLAVE_CFG_IDLE_CLK_NUM_Pos);

		CSPx->TX_FRAME_CTRL |= CSP_TX_FRAME_CTRL_SLAVE_CLK_SAMPLE_Msk;

	    CSP_ClockSet(CSPx, CSP_MODE_SPI_Slave, ulPclk, ulClock);
	}

//	tx_data_len = 7;   // 8 bit data
//	tx_sync_len = 9;   // add 1 setup bit and 1 hold bit
//	tx_shifter_len = 7; //The same as the data length
//
//	rx_data_len = 7;  // 8 bit data
//	rx_shifter_len = 7; //The same as the data length

    CSPx->TX_FRAME_CTRL = (CSPx->TX_FRAME_CTRL &
	                                   ~(CSP_TX_FRAME_CTRL_TX_DATA_LEN_Msk | CSP_TX_FRAME_CTRL_TX_SYNC_LEN_Msk | CSP_TX_FRAME_CTRL_TX_FRAME_LEN_Msk | CSP_TX_FRAME_CTRL_TX_SHIFTER_LEN_Msk)) |
	                                    (tx_data_len | (tx_sync_len << 8) | (tx_frm_len << 16) | (tx_shifter_len << 24));

    CSPx->RX_FRAME_CTRL = (CSPx->RX_FRAME_CTRL &
                                       ~(CSP_RX_FRAME_CTRL_RX_DATA_LEN_Msk | CSP_RX_FRAME_CTRL_RX_FRAME_LEN_Msk | CSP_RX_FRAME_CTRL_RX_SHIFTER_LEN_Msk)) |
	                                    (rx_data_len | (rx_frm_len << 8) | (rx_shifter_len << 16));

    CSPx->TX_DMA_IO_CTRL |= CSP_TX_DMA_IO_CTRL_IO_MODE;
    CSPx->RX_DMA_IO_CTRL |= CSP_RX_DMA_IO_CTRL_IO_MODE;
    CSPx->TX_DMA_IO_LEN = 0;
    CSPx->RX_DMA_IO_LEN = 0;

	CSPx->TX_FIFO_CTRL = (CSPx->TX_FIFO_CTRL & ~CSP_TX_FIFO_CTRL_WIDTH_Msk) | CSP_TX_FIFO_CTRL_WIDTH_BYTE;
	CSPx->RX_FIFO_CTRL = (CSPx->RX_FIFO_CTRL & ~CSP_RX_FIFO_CTRL_WIDTH_Msk) | CSP_RX_FIFO_CTRL_WIDTH_BYTE;
	CSPx->TX_FIFO_LEVEL_CHK = (CSPx->TX_FIFO_LEVEL_CHK &
                                           ~(CSP_TX_FIFO_LEVEL_CHK_SC_Msk | CSP_TX_FIFO_LEVEL_CHK_LC_Msk | CSP_TX_FIFO_LEVEL_CHK_HC_Msk)) |
										    (0x06 | (0x04 << CSP_TX_FIFO_LEVEL_CHK_LC_Pos) | (0x02 << CSP_TX_FIFO_LEVEL_CHK_HC_Pos));
	CSPx->RX_FIFO_LEVEL_CHK = (CSPx->RX_FIFO_LEVEL_CHK &
                                           ~(CSP_RX_FIFO_LEVEL_CHK_SC_Msk | CSP_RX_FIFO_LEVEL_CHK_LC_Msk | CSP_RX_FIFO_LEVEL_CHK_HC_Msk)) |
	                                        (0x02 | (0x04 << CSP_RX_FIFO_LEVEL_CHK_LC_Pos) | (0x06 << CSP_RX_FIFO_LEVEL_CHK_HC_Pos));

}
#endif //�ɵ� zhoudao
/**
  * @brief  set csp tx fifo threshold.
  * @param  ulBase is the base address of the csp module.
  * @param  ulThreshold is the tx fifo threshold
  * @retval None
  */
void CSP_SetTxFifoThreshold(CSP_TypeDef* CSPx, uint32_t ulThreshold)
{
	CSPx->TX_FIFO_CTRL |= (ulThreshold << 2);
}

/**
  * @brief  set csp rx fifo threshold.
  * @param  ulBase is the base address of the csp module.
  * @param  ulThreshold is the rx fifo threshold
  * @retval None
  */
void CSP_SetRxFifoThreshold(CSP_TypeDef* CSPx, uint32_t ulThreshold)
{
	CSPx->RX_FIFO_CTRL |= (ulThreshold << 2);
}

/**
  * @brief  reset csp fifo.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSP_FifoReset(CSP_TypeDef* CSPx)
{
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_RESET;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_RESET;
}

/**
  * @brief  start csp fifo.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSP_FifoStart(CSP_TypeDef* CSPx)
{
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_START;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_START;
}

/**
  * @brief  clear csp rxfifo.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSP_RXFifoClear(CSP_TypeDef* CSPx)
{
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_RESET;
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_START;
}

/**
  * @brief  clear csp txfifo.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSP_TXFifoClear(CSP_TypeDef* CSPx)
{
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_RESET;
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_START;
}

/**
 * @brief  配置TFS引脚的控制模式
 * @param  NewState
 * 	@arg	0	TFS模式
 *  @arg	1	IO模式
 * @retval 无
 */
void CSP_TFS_PIN_MODE(CSP_TypeDef* CSPx, uint8_t NewState)
{
	if (NewState == 1)	{
        CSPx->MODE1 |= 0x4000;
	}
	else {
        CSPx->MODE1 &= ~0x4000;
	}
}

/**
 * @brief  配置RFS引脚的控制模式
 * @param  NewState
 * 	@arg	0	RFS模式
 *  @arg	1	IO模式
 * @retval 无
 */
void CSP_RFS_PIN_MODE(CSP_TypeDef* CSPx, uint8_t NewState)
{
	if (NewState == 1)	{
        CSPx->MODE1 |= 0x2000;
	}
	else {
        CSPx->MODE1 &= ~0x2000;
	}
}

/**
 * @brief  配置TFS引脚有效电平
 * @param  NewState
 * 	@arg	0	低电平有效
 *  @arg	1	高电平有效
 * @retval 无
 */
void CSP_TFS_ACT_LEVEL(CSP_TypeDef* CSPx, uint8_t NewState)
{
    if (NewState == 1)	{
        CSPx->MODE1 |= 0x200;
	}
	else {
        CSPx->MODE1 &= ~0x200;
	}
}

/**
 * @brief  配置RFS引脚有效电平
 * @param  NewState
 * 	@arg	0	低电平有效
 *  @arg	1	高电平有效
 * @retval 无
 */
void CSP_RFS_ACT_LEVEL(CSP_TypeDef* CSPx, uint8_t NewState)
{
    if (NewState == 1)	{
        CSPx->MODE1 |= 0x100;
	}
	else {
        CSPx->MODE1 &= ~0x100;
	}
}

/**
 * @brief  配置TFS引脚在IO模式下的输入输出方向
 * @param  NewState
 * 	@arg	0	输出
 *  @arg	1	输入
 * @retval 无
 */
void CSP_TFS_IO_MODE(CSP_TypeDef* CSPx, uint8_t NewState)
{
	if (NewState == 1)	{
        CSPx->MODE1 |= 0x80000;
	}
	else {
        CSPx->MODE1 &= ~0x80000;
	}
}

/**
 * @brief  配置RFS引脚在IO模式下的输入输出方向
 * @param  NewState
 * 	@arg	0	输出
 *  @arg	1	输入
 * @retval 无
 */
void CSP_RFS_IO_MODE(CSP_TypeDef* CSPx, uint8_t NewState)
{
	if (NewState == 1)	{
        CSPx->MODE1 |= 0x40000;
	}
	else {
        CSPx->MODE1 &= ~0x40000;
	}
}

/**
 * @brief  设置TFS引脚在IO模式下的引脚电平值
 * @param  NewState
 * 	@arg	0	低电平
 *  @arg	1	高电平
 * @retval 无
 */
void CSP_TFS_PIN_VALUE(CSP_TypeDef* CSPx, uint8_t NewState)
{
	if (NewState == 1)	{
        CSPx->PIN_IO_DATA |= 0x2;
	}
	else {
        CSPx->PIN_IO_DATA &= ~0x2;
	}
}

/**
 * @brief  设置RFS引脚在IO模式下的引脚电平值
 * @param  NewState
 * 	@arg	0	低电平
 *  @arg	1	高电平
 * @retval 无
 */
void CSP_RFS_PIN_VALUE(CSP_TypeDef* CSPx, uint8_t NewState)
{
	if (NewState == 1)	{
        CSPx->PIN_IO_DATA |= 0x1;
	}
	else {
        CSPx->PIN_IO_DATA &= ~0x1;
	}
}

/**
  * @brief  config csp rx DMA parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  ucLevelCheck is the rx fifo level check.
  * @param  ulDataLen is the rx DMA_IO lenghth.
  * @retval None
  */
void CSP_DMAConfigRX(CSP_TypeDef* CSPx, uint8_t ucLevelCheck, uint32_t ulDataLen)
{
    CSPx->RX_DMA_IO_CTRL = CSP_RX_DMA_IO_CTRL_DMA_MODE | CSP_RX_DMA_IO_CTRL_DMA_FLUSH_Msk;
    CSPx->RX_DMA_IO_LEN = ulDataLen;

    CSPx->RX_FIFO_CTRL = CSP_TX_FIFO_CTRL_WIDTH_WORD;
    CSPx->RX_FIFO_LEVEL_CHK = ucLevelCheck | (ucLevelCheck << 10) | (ucLevelCheck << 20);

    // CSP FIFO Enable
    CSPx->TX_RX_ENABLE = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    CSPx->RX_FIFO_OP = CSP_RX_FIFO_OP_START;

    CSPx->MODE1 |= CSP_MODE1_CSP_EN;
}

/**
  * @brief  config csp tx DMA parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  ucLevelCheck is the tx fifo level check.
  * @param  ulDataLen is the tx DMA_IO lenghth.
  * @retval None
  */
void CSP_DMAConfigTX(CSP_TypeDef* CSPx, uint8_t ucLevelCheck, uint32_t ulDataLen)
{
    CSPx->TX_DMA_IO_CTRL = CSP_TX_DMA_IO_CTRL_DMA_MODE;
    CSPx->TX_DMA_IO_LEN = ulDataLen;

    CSPx->TX_FIFO_CTRL = CSP_TX_FIFO_CTRL_WIDTH_WORD;
    CSPx->TX_FIFO_LEVEL_CHK = ucLevelCheck | (ucLevelCheck << 10) | (ucLevelCheck << 20);

    // CSP FIFO Enable
    CSPx->TX_RX_ENABLE = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_RESET;

    // Fifo Start
    CSPx->TX_FIFO_OP = CSP_TX_FIFO_OP_START;
    CSPx->MODE1 |= CSP_MODE1_CSP_EN;
}

/**
  * @brief  config csp uart rx timeout parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  relevantFifoEmpty is the tx fifo level check.
  *   This parameter can be one of the following values:
  *         0: timeout will enable no matter fifo is empty or not.
  *         1: only enable when fifo is not empty
  * @param  timeout_num specifies the TIMEOUT bit number for the receive operation. Since the last bit received,
  *         if there is no more data is recieved for TIMEOUT bit number time, the rx timeout interrupt will be triggered.
  * @retval None
  */
void CSP_UARTRxTimeoutConfig(CSP_TypeDef* CSPx, uint32_t relevantFifoEmpty, uint16_t timeout_num)
{
    uint32_t aysnc_param = CSPx->AYSNC_PARAM_REG & ~CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Msk;

    aysnc_param = aysnc_param & ~0xFFFF; //清除低16位

    CSPx->AYSNC_PARAM_REG = aysnc_param | (relevantFifoEmpty << CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Pos) | (timeout_num);
}

#if 1

/**
 * @brief 获取UART超时类型
 * @param CSPx 基地址
 * @return 0：超时倒计时触发条件为RXD处于IDLE
 *         1：超时倒计时触发条件为RXD处于IDLE且RXFIFO非空
 */
uint8_t CSP_UART_TimeoutCondition_Get(CSP_TypeDef* CSPx)
{
    return ((CSPx->AYSNC_PARAM_REG & CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Msk) ? 1 : 0);
}

/**
 * @brief 设置UART超时类型
 * @param CSPx 基地址
 * @param relevantFifoEmpty 
 *         0：超时倒计时触发条件为RXD处于IDLE
 *         1：超时倒计时触发条件为RXD处于IDLE且RXFIFO非空
 */
void CSP_UART_TimeoutCondition_Set(CSP_TypeDef* CSPx, uint32_t relevantFifoEmpty)
{
    CSPx->AYSNC_PARAM_REG = (CSPx->AYSNC_PARAM_REG & ~CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Msk) | (relevantFifoEmpty << CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Pos);
}

#endif

#endif

#if 0	//old
#include "csp.h"

/**
  * @brief  config csp as uart mode.
  * @param  ulBase is the base address of the csp module.
  * @param  ulPclk is the peripheral clock.
  * @param  ulBaudrate is the uart baudrate.
  * @param  ucDatabits is the uart frame data bits.
  * @param  ucParityCheck is the uart parity check:
  *   This parameter can be one of the following values:
  *        CSP_UART_PARITYCHECK_None
  *        CSP_UART_PARITYCHECK_Even
  *        CSP_UART_PARITYCHECK_Odd
  * @param  ucStopbits is the uart frame stop bits.
  * @retval None
  */
void CSPUARTModeSet(unsigned long ulBase, unsigned long ulPclk, unsigned long ulBaudrate, unsigned char ucDatabits, unsigned long ulParityCheck, unsigned char ucStopbits)
{
	CSPOperationModeSet(ulBase, CSP_MODE1_ASYN);

	CSPEndianModeSet(ulBase, CSP_MODE1_ENDIAN_LSBF);

	CSPClockSet(ulBase, CSP_MODE_UART, ulPclk, ulBaudrate);

    if (CSP_UART_PARITYCHECK_None == ulParityCheck) {
        CSPTxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 1, ucDatabits + 1 + ucStopbits);

	    CSPRxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 1 + ucStopbits);
    } else {
        CSPTxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 1, ucDatabits + 2 + ucStopbits);

        CSPRxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 2 + ucStopbits);
    }

    HWREG(ulBase + CSP_INT_STATUS) = CSP_INT_ALL;

    HWREG(ulBase + CSP_TX_DMA_IO_CTRL) = CSP_TX_DMA_IO_CTRL_IO_MODE;

    HWREG(ulBase + CSP_RX_DMA_IO_CTRL) = CSP_RX_DMA_IO_CTRL_IO_MODE;

    HWREG(ulBase + CSP_TX_DMA_IO_LEN) = 0;

    HWREG(ulBase + CSP_RX_DMA_IO_LEN) = 0;

    HWREG(ulBase + CSP_TX_FIFO_CTRL) = CSP_TX_FIFO_CTRL_WIDTH_BYTE;
    HWREG(ulBase + CSP_RX_FIFO_CTRL) = CSP_TX_FIFO_CTRL_WIDTH_BYTE;

    HWREG(ulBase + CSP_AYSNC_PARAM_REG) = (HWREG(ulBase + CSP_AYSNC_PARAM_REG) & ~CSP_AYSNC_PARAM_UART_PARITYCHECK_Msk) | (ulParityCheck);

    // CSP FIFO Enable
    HWREG(ulBase + CSP_TX_RX_ENABLE) = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_RESET;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_START;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_START;

    // CSP Enable
    HWREG(ulBase + CSP_MODE1) |= CSP_MODE1_CSP_EN;
}

void CSPUARTAutoBaudrate(unsigned long ulBase, unsigned long ulCount, unsigned char ucDatabits,  unsigned long ulParityCheck, unsigned char ucStopbits)
{
	unsigned long ulSampleDiv;
    unsigned long ulCSPClkDiv = 0;
    unsigned long diff;
    unsigned long minSampleDiv = 0;
    unsigned long minDiff = 0x0FFFFFFF;
    unsigned long ulCountCal;

	CSPOperationModeSet(ulBase, CSP_MODE1_ASYN);
	CSPEndianModeSet(ulBase, CSP_MODE1_ENDIAN_LSBF);

	for(ulSampleDiv = 0x40; ulSampleDiv > 3; ulSampleDiv--)
	{
		ulCSPClkDiv = (2* ulCount / ulSampleDiv + 1)/2 - 1 ;

		if(ulCSPClkDiv == 0 || ulCSPClkDiv >= 0x1000 || (ulCSPClkDiv + 1)*ulSampleDiv < 16)
		{
			continue;
		}

		ulCountCal = ulSampleDiv * (ulCSPClkDiv + 1);
		if(ulCount >= ulCountCal)
		{
			diff = ulCount - ulCountCal;
		}
		else
		{
			diff = ulCountCal - ulCount;
		}

		if(diff < minDiff)
		{
			minDiff = diff;
			minSampleDiv = ulSampleDiv;
		}

		if(diff == 0)
		{
			break;
		}

	}

	ulSampleDiv = minSampleDiv;
	ulCSPClkDiv = (2 * ulCount / ulSampleDiv + 1) / 2 - 1;
	HWREG(ulBase + CSP_AYSNC_PARAM_REG) = (HWREG(ulBase + CSP_AYSNC_PARAM_REG) & ~CSP_AYSNC_PARAM_DIV2_Msk) | ((ulSampleDiv - 1) << CSP_AYSNC_PARAM_DIV2_Pos);
	HWREG(ulBase + CSP_CLOCK_DIVISOR) = ulCSPClkDiv;

    if (CSP_UART_PARITYCHECK_None == ulParityCheck) {
        CSPTxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 1, ucDatabits + 1 + ucStopbits);

	    CSPRxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 1 + ucStopbits);
    } else {
        CSPTxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 1, ucDatabits + 2 + ucStopbits);

        CSPRxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 2 + ucStopbits);
    }
    HWREG(ulBase + CSP_AYSNC_PARAM_REG) = (HWREG(ulBase + CSP_AYSNC_PARAM_REG) & ~CSP_AYSNC_PARAM_UART_PARITYCHECK_Msk) | (ulParityCheck);

    HWREG(ulBase + CSP_INT_STATUS) = CSP_INT_ALL;

    HWREG(ulBase + CSP_TX_DMA_IO_CTRL) = CSP_TX_DMA_IO_CTRL_IO_MODE;
    HWREG(ulBase + CSP_RX_DMA_IO_CTRL) = CSP_RX_DMA_IO_CTRL_IO_MODE;

    HWREG(ulBase + CSP_TX_DMA_IO_LEN) = 0;
    HWREG(ulBase + CSP_RX_DMA_IO_LEN) = 0;

    HWREG(ulBase + CSP_TX_FIFO_CTRL) = CSP_TX_FIFO_CTRL_WIDTH_BYTE;
    HWREG(ulBase + CSP_RX_FIFO_CTRL) = CSP_TX_FIFO_CTRL_WIDTH_BYTE;

    // CSP FIFO Enable
    HWREG(ulBase + CSP_TX_RX_ENABLE) = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_RESET;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_START;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_START;

    // CSP Enable
    HWREG(ulBase + CSP_MODE1) |= CSP_MODE1_CSP_EN;
}


#if 0
void CSPIrdaMode(unsigned long ulBase, unsigned long ulPclk, unsigned long ulBaudrate, unsigned char ucDatabits, unsigned char ucStopbits, unsigned long ulIdleLevel, unsigned long ulWidth)
{
    CSPOperationModeSet(ulBase, CSP_MODE1_ASYN);

    CSPEndianModeSet(ulBase, CSP_MODE1_ENDIAN_LSBF);

    CSPClockSet(ulBase, CSP_MODE_IRDA, ulPclk, ulBaudrate);

    CSPIrdaIdleLevelSet(ulBase, ulIdleLevel);

    CSPIrdaPulseWidthSet(ulBase, ulWidth);

    CSPIrdaEnable(ulBase, CSP_MODE1_HPSIR_EN);

    CSPTxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 1, ucDatabits + 1 + ucStopbits);

    CSPRxFrameSet(ulBase, 1, ucDatabits, ucDatabits + 1 + ucStopbits);

    HWREG(ulBase + CSP_INT_STATUS) = CSP_INT_ALL;

    HWREG(ulBase + CSP_TX_DMA_IO_CTRL) = CSP_TX_DMA_IO_CTRL_IO_MODE;

    HWREG(ulBase + CSP_RX_DMA_IO_CTRL) = CSP_RX_DMA_IO_CTRL_IO_MODE;

    HWREG(ulBase + CSP_TX_DMA_IO_LEN) = 0;

    HWREG(ulBase + CSP_RX_DMA_IO_LEN) = 0;

    // CSP FIFO Enable
    HWREG(ulBase + CSP_TX_RX_ENABLE) = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_RESET;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_START;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_START;

    // CSP Enable
    HWREG(ulBase + CSP_MODE1) |= CSP_MODE1_CSP_EN;
}
#endif

void CSPSPIMode(unsigned long ulBase, unsigned long ulMode1, unsigned long ulMode2, unsigned long ulTxFrameCtl, unsigned long ulRxFrameCtl)
{
    // clear all pending uart interrupts
    HWREG(ulBase + CSP_INT_STATUS) = CSP_INT_ALL;

    HWREG(ulBase + CSP_MODE1) = ulMode1;

    HWREG(ulBase + CSP_MODE2) = ulMode2;

    HWREG(ulBase + CSP_TX_DMA_IO_CTRL) = CSP_TX_DMA_IO_CTRL_IO_MODE;

    HWREG(ulBase + CSP_RX_DMA_IO_CTRL) = CSP_RX_DMA_IO_CTRL_IO_MODE;

    HWREG(ulBase + CSP_TX_DMA_IO_LEN) = 0;

    HWREG(ulBase + CSP_RX_DMA_IO_LEN) = 0;

    HWREG(ulBase + CSP_TX_FRAME_CTRL) = ulTxFrameCtl;

    HWREG(ulBase + CSP_RX_FRAME_CTRL) = ulRxFrameCtl;

    // CSP FIFO Enable
    HWREG(ulBase + CSP_TX_RX_ENABLE) = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_RESET;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_START;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_START;

    // CSP Enable
    HWREG(ulBase + CSP_MODE1) |= CSP_MODE1_CSP_EN;
}

/**
  * @brief  config csp fifo level.
  * @param  ulBase is the base address of the csp module.
  * @param  ulTxFifo is the tx fifo level.
  * @param  ulRxFifo is the rx fifo level.
  * @retval None
  */
void CSPFIFOLevelSet(unsigned long ulBase, unsigned long ulTxFifo, unsigned long ulRxFifo)
{
    HWREG(ulBase + CSP_TX_FIFO_CTRL) = ulTxFifo;
    HWREG(ulBase + CSP_RX_FIFO_CTRL) = ulRxFifo;
}

/**
  * @brief  get a data.
  * @param  ulBase is the base address of the csp module.
  * @retval a data
  */
unsigned int CSPCharGet(unsigned long ulBase)
{
    while(HWREG(ulBase + CSP_RX_FIFO_STATUS) & CSP_RX_FIFO_STATUS_EMPTY_Msk)
    {
    }

    return(HWREG(ulBase + CSP_RX_FIFO_DATA));
}

/**
  * @brief  put a data.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSPCharPut(unsigned long ulBase, unsigned int ucData)
{
    while(HWREG(ulBase + CSP_TX_FIFO_STATUS) & CSP_TX_FIFO_STATUS_FULL_Msk)
    {
    }

    HWREG(ulBase + CSP_TX_FIFO_DATA) = ucData;
}

/**
  * @brief Gets the RX FIFO status of csp.
  * @note This function is getting the current Receiver FIFO status.
  * @param ulBase: the base address of g_at_lpuart module.
  * @param  ucFlagType specifies the flag to check.
  *   This parameter can be one of the following values:
  *   @arg CSP_FIFO_EMPTY: UART Fifo empty flag
  *   @arg CSP_FIFO_FULL: UART Fifo full flag
  *   @arg CSP_FIFO_LEVEL: The Fifo bytes data number left;when fifo is full, this is 0
  * @retval The current fifo state
  */
unsigned char CSPRxFifoStatusGet(unsigned long ulBase, unsigned char ucFlagType)
{
	if(ucFlagType == CSP_FIFO_EMPTY)
	{
		return ((HWREG(ulBase + CSP_RX_FIFO_STATUS) & CSP_RX_FIFO_STATUS_EMPTY_Msk) ? 1 : 0);
	}
	else if(ucFlagType == CSP_FIFO_FULL)
	{
		return ((HWREG(ulBase + CSP_RX_FIFO_STATUS) & CSP_RX_FIFO_STATUS_FULL_Msk) ? 1 : 0);
	}
	else
	{
		return ((HWREG(ulBase + CSP_RX_FIFO_STATUS) & CSP_RX_FIFO_STATUS_LEVEL_Msk));
	}
}

/**
  * @brief Gets the TX FIFO status of csp.
  * @note This function is getting the current Transmitter FIFO status.
  * @param ulBase: the base address of g_at_lpuart module.
  * @param  ucFlagType specifies the flag to check.
  *   This parameter can be one of the following values:
  *   @arg CSP_FIFO_EMPTY: UART Fifo empty flag
  *   @arg CSP_FIFO_FULL: UART Fifo full flag
  *   @arg CSP_FIFO_LEVEL: The Fifo bytes data number left;when fifo is full, this is 0
  * @retval The current fifo state
  */
unsigned char CSPTxFifoStatusGet(unsigned long ulBase, unsigned char ucFlagType)
{
	if(ucFlagType == CSP_FIFO_EMPTY)
	{
		return ((HWREG(ulBase + CSP_TX_FIFO_STATUS) & CSP_TX_FIFO_STATUS_EMPTY_Msk) ? 1 : 0);
	}
	else if(ucFlagType == CSP_FIFO_FULL)
	{
		return ((HWREG(ulBase + CSP_TX_FIFO_STATUS) & CSP_TX_FIFO_STATUS_FULL_Msk) ? 1 : 0);
	}
	else
	{
		return ((HWREG(ulBase + CSP_TX_FIFO_STATUS) & CSP_TX_FIFO_STATUS_LEVEL_Msk));
	}
}

/**
  * @brief  get fifo data no blocking.
  * @param  ulBase is the base address of the csp module.
  * @retval the fifo data if fifo not empty , else -1: error
  */
long CSPCharGetNonBlocking(unsigned long ulBase)
{
    if(!(HWREG(ulBase + CSP_RX_FIFO_STATUS) & CSP_RX_FIFO_STATUS_EMPTY_Msk))
    {
        return(HWREG(ulBase + CSP_RX_FIFO_DATA));
    }
    else
    {
        return (-1);
    }
}

/**
  * @brief  put fifo data no blocking.
  * @param  ulBase is the base address of the csp module.
  * @param  ucData is the data need putting into txfifo.
  * @retval true if fifo not full , else false if fifo full.
  */
long CSPCharPutNonBlocking(unsigned long ulBase, unsigned char ucData)
{
    if(!(HWREG(ulBase + CSP_TX_FIFO_STATUS) & CSP_TX_FIFO_STATUS_FULL_Msk))
    {
        HWREG(ulBase + CSP_TX_FIFO_DATA) = ucData;

        return (true);
    }
    else
    {
        return (false);
    }
}

/**
  * @brief  config csp fifo level.
  * @param  ulBase is the base address of the csp module.
  * @param  ulTxFifo is the tx fifo level.
  * @param  ulRxFifo is the rx fifo level.
  * @retval None
  */
void CSPOperationModeSet(unsigned long ulBase, unsigned long ulOperationMode)
{
    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~CSP_MODE1_SYNC_MODE_Msk) | ulOperationMode;
}

/**
  * @brief  config csp clock mode.
  * @param  ulBase is the base address of the csp module.
  * @param  ulClockMode is the clock mode as master or slave:
  *         CSP_MODE1_CLOCK_MODE_Master
  *         CSP_MODE1_CLOCK_MODE_Slave
  * @retval None
  */
void CSPClockModeSet(unsigned long ulBase, unsigned long ulClockMode)
{
    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~CSP_MODE1_CLOCK_MODE_Msk) | ulClockMode;
}

void CSPIrdaEnable(unsigned long ulBase, unsigned long ulIrdaEn)
{
    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~CSP_MODE1_HPSIR_EN_Msk) | ulIrdaEn;
}

/**
  * @brief  config csp endian mode.
  * @param  ulBase is the base address of the csp module.
  * @param  ulEndianMode is the endian mode as one of follows:
  *         CSP_MODE1_ENDIAN_MSBF
  *         CSP_MODE1_ENDIAN_LSBF
  * @retval None
  */
void CSPEndianModeSet(unsigned long ulBase, unsigned long ulEndianMode)
{
    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~CSP_MODE1_ENDIAN_CTRL_Msk) | ulEndianMode;
}

/**
  * @brief  enable csp module.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSPEnable(unsigned long ulBase)
{
    HWREG(ulBase + CSP_MODE1) |= CSP_MODE1_CSP_EN;
}

/**
  * @brief  disable csp module.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSPDisable(unsigned long ulBase)
{
    HWREG(ulBase + CSP_MODE1) &= (~CSP_MODE1_CSP_EN);
}

/**
  * @brief  config csp rx and tx driven edge.
  * @param  ulBase is the base address of the csp module.
  * @param  ulRxEdge is the rx driven edge as one of follows:
  *         CSP_MODE1_RXD_ACT_RISE
  *         CSP_MODE1_RXD_ACT_FALL
  * @param  ulTxEdge is the tx driven edge as one of follows:
  *         CSP_MODE1_TXD_ACT_RISE
  *         CSP_MODE1_TXD_ACT_FALL
  * @retval None
  */
void CSPDrivenEdgeSet(unsigned long ulBase, unsigned long ulRxEdge, unsigned long ulTxEdge)
{
    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_RXD_ACT_EDGE_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk)) | (ulRxEdge | ulTxEdge);
}

void CSPSyncValidLevelSet(unsigned long ulBase, unsigned long ulRFS, unsigned long ulTFS)
{
    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_RFS_ACT_LEVEL_Msk | CSP_MODE1_TFS_ACT_LEVEL_Msk)) | (ulRFS | ulTFS);
}

void CSPClockIdleModeSet(unsigned long ulBase, unsigned long ulClkIdleMode)
{
    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~CSP_MODE1_CLK_IDLE_MODE_Msk) | ulClkIdleMode;
}

void CSPClockIdleLevelSet(unsigned long ulBase, unsigned long ulClkIdleLevel)
{
    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~CSP_MODE1_CLK_IDLE_LEVEL_Msk) | ulClkIdleLevel;
}

void CSPPinModeSet(unsigned long ulBase, unsigned long ulCSPPin, unsigned long ulPinMode, unsigned long ulPinDirection)
{
    if(ulCSPPin != CSP_PIN_CLK && ulCSPPin != CSP_PIN_RFS && ulCSPPin != CSP_PIN_TFS && ulCSPPin != CSP_PIN_RXD && ulCSPPin != CSP_PIN_TXD)
    {
        return ;
    }

    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~((1 << ulCSPPin) | (1 << (ulCSPPin+5)))) | (ulPinMode << ulCSPPin) | (ulPinDirection << (ulCSPPin+5));
}

void CSPIrdaPulseWidthSet(unsigned long ulBase, unsigned long ulWidth)
{
    HWREG(ulBase + CSP_MODE2) = (HWREG(ulBase + CSP_MODE2) & (~CSP_MODE2_IRDA_DATA_WIDTH_Msk)) | ulWidth;
}

void CSPIrdaIdleLevelSet(unsigned long ulBase, unsigned long ulIrdaIdleLevel)
{
    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~CSP_MODE1_IrDA_IDLE_LEVEL_Msk) | ulIrdaIdleLevel;
}

/**
  * @brief  config csp peripheral clock and baudrate.
  * @param  ulBase is the base address of the csp module.
  * @param  ulCSPMode is the csp mode as one of follows:
  *         CSP_MODE_UART
  *         CSP_MODE_IRDA
  *         CSP_MODE_SPI_Master
  *         CSP_MODE_SPI_Slave
  *         CSP_MODE_SIM
  * @param  ulPclk is the peripheral clock.
  * @param  ulClock is the baudrate.
  * @retval None
  */
void CSPClockSet(unsigned long ulBase, unsigned long ulCSPMode, unsigned long ulPclk, unsigned ulClock)
{
    unsigned long ulSampleDiv;
    unsigned long ulCSPClkDiv = 0;
    unsigned long ulIrdaWidthDiv;

    unsigned long diff;
    unsigned long minSampleDiv = 16;
    unsigned long minDiff = 0x0FFFFFFF;
    unsigned long ulPclkCalculate;

    if(ulCSPMode == CSP_MODE_UART)
    {
        //
        // Calculate the best ulSampleDiv and ulCSPClkDiv.
        //
        for(ulSampleDiv = 0x40; ulSampleDiv > 3; ulSampleDiv--)
        {
            ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulClock + 1) / 2 - 1;

            if(ulCSPClkDiv >= 0x1000 || (ulCSPClkDiv + 1)*ulSampleDiv < 16)
            {
                continue;
            }

			ulPclkCalculate = ulClock * ulSampleDiv * (ulCSPClkDiv + 1);
			if(ulPclk >= ulPclkCalculate)
			{
				diff = ulPclk - ulPclkCalculate;
			}
			else
			{
				diff = ulPclkCalculate - ulPclk;
			}

            if(diff < minDiff)
            {
                minDiff = diff;
                minSampleDiv = ulSampleDiv;
            }

			if(diff == 0)
			{
				break;
			}

        }

        ulSampleDiv = minSampleDiv;

        ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulClock + 1) / 2 - 1;

        HWREG(ulBase + CSP_AYSNC_PARAM_REG) = (HWREG(ulBase + CSP_AYSNC_PARAM_REG) & ~CSP_AYSNC_PARAM_DIV2_Msk) | ((ulSampleDiv - 1) << CSP_AYSNC_PARAM_DIV2_Pos);
    }
    else if(ulCSPMode == CSP_MODE_IRDA)
    {
        ulSampleDiv = 16;

        ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulClock + 1) / 2 - 1;

        ulIrdaWidthDiv = 16 * ulPclk / 10000000 + 1;

        HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~CSP_MODE1_IRDA_WIDTH_DIV_Msk) | (ulIrdaWidthDiv << CSP_MODE1_IRDA_WIDTH_DIV_Pos);

        HWREG(ulBase + CSP_AYSNC_PARAM_REG) = (HWREG(ulBase + CSP_AYSNC_PARAM_REG) & ~CSP_AYSNC_PARAM_DIV2_Msk) | ((ulSampleDiv - 1) << CSP_AYSNC_PARAM_DIV2_Pos);
    }
    else if(ulCSPMode == CSP_MODE_SPI_Master)
    {
        ulCSPClkDiv = ulPclk / ulClock / 2 - 1;
    }
	else if(ulCSPMode == CSP_MODE_SPI_Slave)
	{
		ulCSPClkDiv = 4;
	}
    else if(ulCSPMode == CSP_MODE_SIM)
    {
        // ulCSPClkDiv = ;
    }

    HWREG(ulBase + CSP_CLOCK_DIVISOR) = ulCSPClkDiv;
}


void CSPHwDetectClockSet(unsigned long ulBase, unsigned long ulPclk, unsigned ulBaudrate)
{
    unsigned long ulSampleDiv;
    unsigned long ulCSPClkDiv;

    unsigned long diff;
    unsigned long minSampleDiv = 16;
    unsigned long minDiff = 0x0FFFFFFF;
    unsigned long ulPclkCalculate;

	//
	// Calculate the best ulSampleDiv and ulCSPClkDiv.
	//
	for(ulSampleDiv = 8; ulSampleDiv > 3; ulSampleDiv--)
	{
		ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulBaudrate + 1) / 2 - 1;

		if(ulCSPClkDiv >= 0x1000 || (ulCSPClkDiv + 1)*ulSampleDiv < 16)
		{
			continue;
		}

		ulPclkCalculate = ulBaudrate * ulSampleDiv * (ulCSPClkDiv + 1);

		if(ulPclk >= ulPclkCalculate)
		{
			diff = ulPclk - ulPclkCalculate;
		}
		else
		{
			diff = ulPclkCalculate - ulPclk;
		}

		if(diff < minDiff)
		{
			minDiff = diff;
			minSampleDiv = ulSampleDiv;
		}

		if(diff == 0)
		{
			break;
		}

	}

	ulSampleDiv = minSampleDiv;

	ulCSPClkDiv = (2 * ulPclk / ulSampleDiv / ulBaudrate + 1) / 2 - 1;

	HWREG(ulBase + CSP_AYSNC_PARAM_REG) = (HWREG(ulBase + CSP_AYSNC_PARAM_REG) & ~CSP_AYSNC_PARAM_DIV2_Msk) | ((ulSampleDiv - 1) << CSP_AYSNC_PARAM_DIV2_Pos);

	HWREG(ulBase + CSP_CLOCK_DIVISOR) = ulCSPClkDiv;
}

/**
  * @brief  config csp tx frame parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  ulDelay is the tx frame delay
  * @param  ulDataLen is the transmitted data length in one frame.
  * @param  ulSyncLen is the valid length of transmitted sync signal (TFS) in one frame.
  *   This parameter can be one of the following values:
  *         CSP_TX_FRAME_CTRL_TX_SYNC_BITS_8
  *         CSP_TX_FRAME_CTRL_TX_SYNC_BITS_9
  * @param  ulFrameLen is the transmit frame length include active state and idle state.
  * @retval None
  */
void CSPTxFrameSet(unsigned long ulBase, unsigned long ulDelay, unsigned long ulDataLen, unsigned long ulSyncLen, unsigned long ulFrameLen)
{
    HWREG(ulBase + CSP_MODE2) = (HWREG(ulBase + CSP_MODE2) & ~CSP_MODE2_TXD_DELAY_LEN_Msk) | (ulDelay << CSP_MODE2_TXD_DELAY_LEN_Pos);
    HWREG(ulBase + CSP_TX_FRAME_CTRL) = (HWREG(ulBase + CSP_TX_FRAME_CTRL) & 0xE0000000) | (ulDataLen - 1) | (ulSyncLen - 1) << CSP_TX_FRAME_CTRL_TX_SYNC_LEN_Pos | (ulFrameLen - 1) << CSP_TX_FRAME_CTRL_TX_FRAME_LEN_Pos | (ulDataLen - 1) << CSP_TX_FRAME_CTRL_TX_SHIFTER_LEN_Pos;
}

/**
  * @brief  config csp rx frame parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  ulDelay is the rx frame delay
  * @param  ulDataLen is the received data length in one frame.
  * @param  ulFrameLen is the received frame length include active and idle state.
  * @retval None
  */
void CSPRxFrameSet(unsigned long ulBase, unsigned long ulDelay, unsigned long ulDataLen, unsigned long ulFrameLen)
{
    HWREG(ulBase + CSP_MODE2) = (HWREG(ulBase + CSP_MODE2) & ~CSP_MODE2_RXD_DELAY_LEN_Msk) | (ulDelay << CSP_MODE2_RXD_DELAY_LEN_Pos);
    HWREG(ulBase + CSP_RX_FRAME_CTRL) = (HWREG(ulBase + CSP_RX_FRAME_CTRL) & 0xFFE00000) | (ulDataLen - 1) | (ulFrameLen - 1) << CSP_RX_FRAME_CTRL_RX_FRAME_LEN_Pos | (ulDataLen - 1) << CSP_RX_FRAME_CTRL_RX_SHIFTER_LEN_Pos;
}

void CSPRxEnable(unsigned long ulBase)
{
    HWREG(ulBase + CSP_TX_RX_ENABLE) |= CSP_TX_RX_ENABLE_RX_ENA;
}

void CSPRxDisable(unsigned long ulBase)
{
    HWREG(ulBase + CSP_TX_RX_ENABLE) &= ~CSP_TX_RX_ENABLE_RX_ENA;
}

unsigned char CSPRxStatusGet(unsigned long ulBase)
{
    return (HWREGB(ulBase + CSP_TX_RX_ENABLE) & CSP_TX_RX_ENABLE_RX_ENA);
}

void CSPTxEnable(unsigned long ulBase)
{
    HWREG(ulBase + CSP_TX_RX_ENABLE) |= CSP_TX_RX_ENABLE_TX_ENA;
}

void CSPTxDisable(unsigned long ulBase)
{
    HWREG(ulBase + CSP_TX_RX_ENABLE) &= ~CSP_TX_RX_ENABLE_TX_ENA;
}


void CSPIntRegister(unsigned long ulIntChannel, unsigned long *g_pRAMVectors, void(*pfnHandler)(void))
{

	IntRegister(ulIntChannel, g_pRAMVectors, pfnHandler);

	IntEnable(ulIntChannel);
}

void CSPIntUnregister(unsigned long ulIntChannel, unsigned long *g_pRAMVectors)
{
	IntDisable(ulIntChannel);

	IntUnregister(ulIntChannel, g_pRAMVectors);
}

void CSPIntEnable(unsigned long ulBase, unsigned long ulIntFlags)
{
    HWREG(ulBase + CSP_INT_ENABLE) |= ulIntFlags;
}

void CSPIntDisable(unsigned long ulBase, unsigned long ulIntFlags)
{
    HWREG(ulBase + CSP_INT_ENABLE) &= (~ulIntFlags);
}

unsigned long CSPIntStatus(unsigned long ulBase)
{
    return HWREG(ulBase + CSP_INT_STATUS);
}

void CSPIntClear(unsigned long ulBase, unsigned long ulIntFlags)
{
    HWREG(ulBase + CSP_INT_STATUS) = ulIntFlags;
}

void CSPSPIConfigSetExpClk(unsigned long ulBase, unsigned char ulMode, unsigned long ulPclk, unsigned ulClock, unsigned char CPOL, unsigned char CPHA)
{
	unsigned long tx_data_len, tx_sync_len, tx_frm_len, tx_shifter_len;
	unsigned long rx_data_len, rx_frm_len, rx_shifter_len;

	if(ulMode == CSP_MODE1_CLOCK_MODE_Master) {
		if(CPOL == 0 && CPHA == 0) {
		    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL);
			tx_frm_len = 11;
			rx_frm_len = 11;
		} else if(CPOL == 0 && CPHA == 1) {  //ok
		    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE);
			tx_frm_len = 11;
			rx_frm_len = 11;
		} else if(CPOL == 1 && CPHA == 0) {
		    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE | CSP_MODE1_CLK_IDLE_LEVEL_Msk);
			tx_frm_len = 11;
			rx_frm_len = 11;
		} else if(CPOL == 1 && CPHA == 1) {
		    HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_CLOCK_MODE_Msk | CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Master | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL | CSP_MODE1_CLK_IDLE_LEVEL_Msk);
			tx_frm_len = 11;
			rx_frm_len = 11;
		}
		tx_data_len = 7; // 8 bit - 1
		tx_sync_len = 9; // add 1 setup bit and 1 hold bit
		tx_shifter_len = 7; //The same as the data length

		rx_data_len = 7;// 8 bit data
		rx_shifter_len = 7; //The same as the data length

		HWREG(ulBase + CSP_MODE2) = (HWREG(ulBase + CSP_MODE2) & ~(CSP_MODE2_RXD_DELAY_LEN_Msk | CSP_MODE2_TXD_DELAY_LEN_Msk)) |
		                            (1 << CSP_MODE2_RXD_DELAY_LEN_Pos) | (1 << CSP_MODE2_TXD_DELAY_LEN_Pos);

		CSPClockSet(ulBase, CSP_MODE_SPI_Master, ulPclk, ulClock);
	}
	else if(ulMode == CSP_MODE1_CLOCK_MODE_Slave)
	{
		if(CPOL == 0 && CPHA == 0) {
			HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL);
			tx_frm_len = 9;
			rx_frm_len = 9;
			HWREG(ulBase + CSP_SYNC_SLAVE_CFG) |= (1<<CSP_SYNC_SLAVE_CFG_FS_START_PLSGEN_Pos);
		} else if(CPOL == 0 && CPHA == 1) {
			HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE);
			tx_frm_len = 8;
			rx_frm_len = 9;
		} else if(CPOL == 1 && CPHA == 0) {
			HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_TXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_FALL | CSP_MODE1_TXD_ACT_RISE);
			tx_frm_len = 9;//9;
			rx_frm_len = 9;//8;
			HWREG(ulBase + CSP_SYNC_SLAVE_CFG) |= (1<<CSP_SYNC_SLAVE_CFG_FS_START_PLSGEN_Pos);
		} else if(CPOL == 1 && CPHA == 1) {
			HWREG(ulBase + CSP_MODE1) = (HWREG(ulBase + CSP_MODE1) & ~(CSP_MODE1_ENDIAN_CTRL_Msk | CSP_MODE1_RXD_ACT_EDGE_Msk)) |
			                            (CSP_MODE1_SYNC | CSP_MODE1_CLOCK_MODE_Slave | CSP_MODE1_ENDIAN_MSBF | CSP_MODE1_RXD_ACT_RISE | CSP_MODE1_TXD_ACT_FALL);
			tx_frm_len = 8;
			rx_frm_len = 9;
		}

		tx_data_len = 7; // 8 bit - 1
		tx_sync_len = 0; // any value
		tx_shifter_len = 7; //The same as the data length

		rx_data_len = 7;// 8 bit data
		rx_shifter_len = 7; //The same as the data length

		HWREG(ulBase + CSP_MODE2) = (HWREG(ulBase + CSP_MODE2)& ~(CSP_MODE2_RXD_DELAY_LEN_Msk | CSP_MODE2_TXD_DELAY_LEN_Msk)) |
                           		    (1 << CSP_MODE2_RXD_DELAY_LEN_Pos) | (0 << CSP_MODE2_TXD_DELAY_LEN_Pos) |
		                            CSP_MODE2_RFS_MS_MODE_Msk | CSP_MODE2_TFS_MS_MODE_Msk ;

		HWREG(ulBase + CSP_SYNC_SLAVE_CFG) =(HWREG(ulBase + CSP_SYNC_SLAVE_CFG) & ~CSP_SYNC_SLAVE_CFG_IDLE_CLK_NUM_Msk) |
		                                    CSP_SYNC_SLAVE_CFG_RFSIN2TFSIN_Msk | (1<<CSP_SYNC_SLAVE_CFG_IDLE_CLK_NUM_Pos);

		HWREG(ulBase + CSP_TX_FRAME_CTRL) |= CSP_TX_FRAME_CTRL_SLAVE_CLK_SAMPLE_Msk;

	    CSPClockSet(ulBase, CSP_MODE_SPI_Slave, ulPclk, ulClock);
	}

//	tx_data_len = 7;   // 8 bit data
//	tx_sync_len = 9;   // add 1 setup bit and 1 hold bit
//	tx_shifter_len = 7; //The same as the data length
//
//	rx_data_len = 7;  // 8 bit data
//	rx_shifter_len = 7; //The same as the data length

    HWREG(ulBase + CSP_TX_FRAME_CTRL) = (HWREG(ulBase + CSP_TX_FRAME_CTRL) &
	                                   ~(CSP_TX_FRAME_CTRL_TX_DATA_LEN_Msk | CSP_TX_FRAME_CTRL_TX_SYNC_LEN_Msk | CSP_TX_FRAME_CTRL_TX_FRAME_LEN_Msk | CSP_TX_FRAME_CTRL_TX_SHIFTER_LEN_Msk)) |
	                                    (tx_data_len | (tx_sync_len << 8) | (tx_frm_len << 16) | (tx_shifter_len << 24));

    HWREG(ulBase + CSP_RX_FRAME_CTRL) = (HWREG(ulBase + CSP_RX_FRAME_CTRL) &
                                       ~(CSP_RX_FRAME_CTRL_RX_DATA_LEN_Msk | CSP_RX_FRAME_CTRL_RX_FRAME_LEN_Msk | CSP_RX_FRAME_CTRL_RX_SHIFTER_LEN_Msk)) |
	                                    (rx_data_len | (rx_frm_len << 8) | (rx_shifter_len << 16));

    HWREG(ulBase + CSP_TX_DMA_IO_CTRL) |= CSP_TX_DMA_IO_CTRL_IO_MODE;
    HWREG(ulBase + CSP_RX_DMA_IO_CTRL) |= CSP_RX_DMA_IO_CTRL_IO_MODE;
    HWREG(ulBase + CSP_TX_DMA_IO_LEN) = 0;
    HWREG(ulBase + CSP_RX_DMA_IO_LEN) = 0;

	HWREG(ulBase + CSP_TX_FIFO_CTRL) = (HWREG(ulBase + CSP_TX_FIFO_CTRL) & ~CSP_TX_FIFO_CTRL_WIDTH_Msk) | CSP_TX_FIFO_CTRL_WIDTH_BYTE;
	HWREG(ulBase + CSP_RX_FIFO_CTRL) = (HWREG(ulBase + CSP_RX_FIFO_CTRL) & ~CSP_RX_FIFO_CTRL_WIDTH_Msk) | CSP_RX_FIFO_CTRL_WIDTH_BYTE;
	HWREG(ulBase + CSP_TX_FIFO_LEVEL_CHK) = (HWREG(ulBase + CSP_TX_FIFO_LEVEL_CHK) &
                                           ~(CSP_TX_FIFO_LEVEL_CHK_SC_Msk | CSP_TX_FIFO_LEVEL_CHK_LC_Msk | CSP_TX_FIFO_LEVEL_CHK_HC_Msk)) |
										    (0x06 | (0x04 << CSP_TX_FIFO_LEVEL_CHK_LC_Pos) | (0x02 << CSP_TX_FIFO_LEVEL_CHK_HC_Pos));
	HWREG(ulBase + CSP_RX_FIFO_LEVEL_CHK) = (HWREG(ulBase + CSP_RX_FIFO_LEVEL_CHK) &
                                           ~(CSP_RX_FIFO_LEVEL_CHK_SC_Msk | CSP_RX_FIFO_LEVEL_CHK_LC_Msk | CSP_RX_FIFO_LEVEL_CHK_HC_Msk)) |
	                                        (0x02 | (0x04 << CSP_RX_FIFO_LEVEL_CHK_LC_Pos) | (0x06 << CSP_RX_FIFO_LEVEL_CHK_HC_Pos));

}

/**
  * @brief  set csp tx fifo threshold.
  * @param  ulBase is the base address of the csp module.
  * @param  ulThreshold is the tx fifo threshold
  * @retval None
  */
void CSPSetTxFifoThreshold(unsigned long ulBase, unsigned long ulThreshold)
{
	HWREG(ulBase + CSP_TX_FIFO_CTRL) |= (ulThreshold << 2);
}

/**
  * @brief  set csp rx fifo threshold.
  * @param  ulBase is the base address of the csp module.
  * @param  ulThreshold is the rx fifo threshold
  * @retval None
  */
void CSPSetRxFifoThreshold(unsigned long ulBase, unsigned long ulThreshold)
{
	HWREG(ulBase + CSP_RX_FIFO_CTRL) |= (ulThreshold << 2);
}

/**
  * @brief  reset csp fifo.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSPFifoReset(unsigned long ulBase)
{
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_RESET;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_RESET;
}

/**
  * @brief  start csp fifo.
  * @param  ulBase is the base address of the csp module.
  * @retval None
  */
void CSPFifoStart(unsigned long ulBase)
{
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_START;
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_START;
}

/**
  * @brief  config csp rx DMA parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  ucLevelCheck is the rx fifo level check.
  * @param  ulDataLen is the rx DMA_IO lenghth.
  * @retval None
  */
void CSPDMAConfigRX(unsigned long ulBase, unsigned char ucLevelCheck, unsigned long ulDataLen)
{
    HWREG(ulBase + CSP_RX_DMA_IO_CTRL) = CSP_RX_DMA_IO_CTRL_DMA_MODE | CSP_RX_DMA_IO_CTRL_DMA_FLUSH_Msk;
    HWREG(ulBase + CSP_RX_DMA_IO_LEN) = ulDataLen;

    HWREG(ulBase + CSP_RX_FIFO_CTRL) = CSP_TX_FIFO_CTRL_WIDTH_WORD;
    HWREG(ulBase + CSP_RX_FIFO_LEVEL_CHK) = ucLevelCheck | (ucLevelCheck << 10) | (ucLevelCheck << 20);

    // CSP FIFO Enable
    HWREG(ulBase + CSP_TX_RX_ENABLE) = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_RESET;

    // Fifo Start
    HWREG(ulBase + CSP_RX_FIFO_OP) = CSP_RX_FIFO_OP_START;

    HWREG(ulBase + CSP_MODE1) |= CSP_MODE1_CSP_EN;
}

/**
  * @brief  config csp tx DMA parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  ucLevelCheck is the tx fifo level check.
  * @param  ulDataLen is the tx DMA_IO lenghth.
  * @retval None
  */
void CSPDMAConfigTX(unsigned long ulBase, unsigned char ucLevelCheck, unsigned long ulDataLen)
{
    HWREG(ulBase + CSP_TX_DMA_IO_CTRL) = CSP_TX_DMA_IO_CTRL_DMA_MODE;
    HWREG(ulBase + CSP_TX_DMA_IO_LEN) = ulDataLen;

    HWREG(ulBase + CSP_TX_FIFO_CTRL) = CSP_TX_FIFO_CTRL_WIDTH_WORD;
    HWREG(ulBase + CSP_TX_FIFO_LEVEL_CHK) = ucLevelCheck | (ucLevelCheck << 10) | (ucLevelCheck << 20);

    // CSP FIFO Enable
    HWREG(ulBase + CSP_TX_RX_ENABLE) = (CSP_TX_RX_ENABLE_TX_ENA | CSP_TX_RX_ENABLE_RX_ENA);

    // Fifo Reset
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_RESET;

    // Fifo Start
    HWREG(ulBase + CSP_TX_FIFO_OP) = CSP_TX_FIFO_OP_START;

    HWREG(ulBase + CSP_MODE1) |= CSP_MODE1_CSP_EN;
}

/**
  * @brief  config csp uart rx timeout parameters.
  * @param  ulBase is the base address of the csp module.
  * @param  relevantFifoEmpty is the tx fifo level check.
  *   This parameter can be one of the following values:
  *         0: timeout will enable no matter fifo is empty or not.
  *         1: only enable when fifo is not empty
  * @param  timeout_num specifies the TIMEOUT bit number for the receive operation. Since the last bit received,
  *         if there is no more data is recieved for TIMEOUT bit number time, the rx timeout interrupt will be triggered.
  * @retval None
  */
void CSPUARTRxTimeoutConfig(unsigned long ulBase, unsigned char relevantFifoEmpty, unsigned short timeout_num)
{
    unsigned long aysnc_param = HWREG(ulBase + CSP_AYSNC_PARAM_REG) & ~CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Msk;

    HWREG(ulBase + CSP_AYSNC_PARAM_REG) = aysnc_param | (relevantFifoEmpty << CSP_AYSNC_PARAM_RX_TIMEOUT_EN_Pos) | (timeout_num);
}

#endif
