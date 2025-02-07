#if 1	//new
#include "spi.h"
#include "interrupt.h"

/**
  * @brief Configures the synchronous serial interface.
  * @param ulDivide: the division of clock frequency 
  *   This parameter can be one of the following values
  *     @arg SPI_CONFIG_CLK_DIV_2
  *     @arg SPI_CONFIG_CLK_DIV_4
  *     @arg SPI_CONFIG_CLK_DIV_8
  *     @arg SPI_CONFIG_CLK_DIV_16
  *     @arg SPI_CONFIG_CLK_DIV_32
  *     @arg SPI_CONFIG_CLK_DIV_64
  *     @arg SPI_CONFIG_CLK_DIV_128
  *     @arg SPI_CONFIG_CLK_DIV_256  
  * @param ulProtocol: specifies the data transfer protocol
  *   This parameter can be one of the following values:
  *     @arg SPI_FRF_MOTO_MODE_0: polarity 0, phase 0
  *     @arg SPI_FRF_MOTO_MODE_1: polarity 0, phase 1
  *     @arg SPI_FRF_MOTO_MODE_2: polarity 1, phase 0
  *     @arg SPI_FRF_MOTO_MODE_3: polarity 1, phase 1
  * @param ulMode: specifies the mode of operation. 
  *   This parameter can be one of the following values:
  *     @arg SPI_CONFIG_MODE_MASTER
  *     @arg SPI_CONFIG_MODE_SLAVE
  * @param ulDataWidth specifies number of bits transferred per frame.
  *   This parameter can be one of the following values:
  *     @arg SPI_CONFIG_WORD_SIZE_BITS_8
  *     @arg SPI_CONFIG_WORD_SIZE_BITS_16
  *     @arg SPI_CONFIG_WORD_SIZE_BITS_32
  * @retval None
  */
void SPI_ConfigSetExpClk(uint32_t ulDivide, uint32_t ulProtocol, uint32_t ulMode, uint32_t ulDataWidth)                   
{
	SPI->CONFIG = (SPI->CONFIG & ~(SPI_CONFIG_MODE_Msk | SPI_CONFIG_CPOL_Msk | SPI_CONFIG_CPHA_Msk | SPI_CONFIG_CLK_DIV_Msk | SPI_CONFIG_WORD_SIZE_Msk)) |
                        (ulDivide | ulProtocol | ulMode | ulDataWidth);
}

/**
  * @brief Enable the SPI.
  * @note  The SPI must be configured before it is enabled
  * @retval None
  */
void SPI_Enable(void)
{
	SPI->ENABLE = 0x1;
}

/**
  * @brief Disables the SPI.
  * @retval None
  */
void SPI_Disable(void)
{
    SPI->ENABLE = 0;
}

/**
  * @brief Set the mode of slave selection control
  * @param ulMode: the mode of slave selection control 
  *   This parameter can be one of the following values:
  *     @arg SPI_CONFIG_SS_MODE_NORMAL: the peripheral select signals SS_0/SS_1 only one is
  *       active low at any one time.
  *     @arg SPI_CONFIG_SS_MODE_DECODE: Enable decoding function of slave selection control,the 
  *       peripheral select signals are output directly through SS0&SS1 and connect a 2-bit 
  *       to 4-bit decoder to provide the capability of generating up to 4 slave selects.    
  * @retval None
  */
void SPI_ChipSelectMode(uint32_t ulMode)
{
    SPI->CONFIG = (SPI->CONFIG & ~(SPI_CONFIG_SS_MODE_Msk)) | ulMode;
}

/**
  * @brief Setting selection channel of peripheral chip.
  * @note if pdec=0, the peripheral select signals SS_0/SS_0 only one is active low 
  *       at any one time; if pdec=1, the peripheral select signals are output directly 
  *       through SS0&SS1 and connect a 2-bit to 4-bit decoder to provide the capability of 
  *       generating up to 4 peripheral chip selects.
  * @param ulChipSelect: the channels 
  *   This parameter can be one of the following values:
  *		if Mode is SPI_CONFIG_SS_MODE_NORMAL, the following parameters are valid:
  *     @arg SPI_CONFIG_SS_LINES_SS0: use SS0 as the control of slave selection
  *     @arg SPI_CONFIG_SS_LINES_SS1: use SS1 as the control of slave selection
  *     @arg SPI_CONFIG_SS_LINES_NONE: no lines is active
  *		if Mode is SPI_CONFIG_SS_MODE_DECODE, the following parameters are valid:
  *     @arg SPI_CONFIG_SS_LINES_OUT_00: SS0 output 0 and SS1 output 0 
  *     @arg SPI_CONFIG_SS_LINES_OUT_01: SS0 output 1 and SS1 output 0 
  *     @arg SPI_CONFIG_SS_LINES_OUT_10: SS0 output 0 and SS1 output 1 
  *     @arg SPI_CONFIG_SS_LINES_OUT_11: SS0 output 1 and SS1 output 1
  * @retval None
  */
void SPI_ChipSelect(uint32_t ulChipSelect)
{
    SPI->CONFIG = (SPI->CONFIG & ~(SPI_CONFIG_SS_LINES_Msk)) | ulChipSelect;
}

/**
  * @brief Disable the the busrt mode of slave.
  * @note  This function is only used for slave mode,
  *        the spi_clk would running continuously when spi_cs stays active between two bytes.
  * @retval None
  */
void SPI_SlaveBurstEnable(void)
{
	SPI->CONFIG |= SPI_CONFIG_SLAVE_BURST_Msk;
}

/**
  * @brief Enable the the busrt mode of slave.
  * @note  This function is only used for slave mode,
  *    spi_sclk should be idle for one sclk cycle at least between two bytes.
  * @retval None
  */
void SPI_SlaveBurstDisable(void)
{
	SPI->CONFIG &= ~SPI_CONFIG_SLAVE_BURST_Msk;
}

/**
  * @brief Set Tx FIFO Threshold.
  * @param ucThreshold: the threshold value(less than 128)
  * @retval None
  */
void SPI_SetTxFifoThreshold(uint8_t ucThreshold)
{  
    SPI->TX_THRESH = ucThreshold;
}

/**
  * @brief Set Rx FIFO Threshold.
  * @param ucThreshold: the threshold value(less than 128)
  * @retval None
  */
void SPI_SetRxFifoThreshold(uint8_t ulThreshold)
{
    SPI->RX_THRESH = ulThreshold;
}

/**
  * @brief Reset Tx FIFO.
  * @retval None
  */
void SPI_TxFifoReset(void)
{
	SPI->TX_FIFO_OP |= SPI_FIFO_OP_RESET_Msk;
	SPI->TX_FIFO_OP &= ~SPI_FIFO_OP_RESET_Msk;
}

/**
  * @brief Enable Tx FIFO.
  * @retval None
  */
void SPI_TxFifoEnable(void)
{
	SPI->TX_FIFO_OP |= SPI_FIFO_OP_START_Msk;
}

/**
  * @brief Disable Tx FIFO.
  * @retval None
  */
void SPI_TxFifoDisable(void)
{
	SPI->TX_FIFO_OP &= ~SPI_FIFO_OP_START_Msk;
}

/**
  * @brief Reset Rx FIFO.
  * @retval None
  */
void SPI_RxFifoReset(void)
{
    SPI->RX_FIFO_OP |= SPI_FIFO_OP_RESET_Msk;
	SPI->RX_FIFO_OP &= ~SPI_FIFO_OP_RESET_Msk;
}

/**
  * @brief Enable Rx FIFO.
  * @retval None
  */
void SPI_RxFifoEnable(void)
{
    SPI->RX_FIFO_OP |= SPI_FIFO_OP_START_Msk;
}

/**
  * @brief Disable Rx FIFO.
  * @retval None
  */
void SPI_RxFifoDisable(void)
{
    SPI->RX_FIFO_OP &= ~SPI_FIFO_OP_START_Msk;
}

/**
  * @brief Set Delay for SPI.
  * @param ucTransInitDelay: The delay value of first bit transfer after 
  *        setting ss_pin low
  *        Actual delay time =(value + 1)*SPI reference clock
  * @param ucTransAfterDelay: The delay value of between bytes 
  *        Cpha = 0: actual delay time = (value + 3)*SPI reference clock
  *        Cpha = 1: actual delay time = (value + 1)*SPI reference clock
  * @param ucTransBetweenDelay: The delay value of between one chip select 
  *        being de-activated and the activation of another.
  *        Actual delay time =(value + 2)*SPI reference clock
  * @param ucTransNActiveDelay: The delay value of master mode chip select 
  *        outputs are de-asserted between words.
  *        Actual delay time =(value + 1)*SPI reference clock
  * @retval None
  */
void SPI_SetDelay(uint8_t ucTransNActiveDelay, uint8_t ucTransBetweenDelay, uint8_t ucTransAfterDelay, uint8_t ucTransInitDelay)
{ 
    SPI->DELAY = (ucTransNActiveDelay << 24 | ucTransBetweenDelay << 16 | ucTransAfterDelay << 8 | ucTransInitDelay) ;
}

/**
  * @brief Enable Manual CS for SPI Master.
  * @retval None
  */
void SPI_ManualCsSet(void)
{
    SPI->CONFIG |= SPI_CONFIG_MANUAL_CS_Msk;
}

/**
  * @brief Disable Manual CS for SPI Master.
  * @retval None
  */
void SPI_ManualCsClear(void)
{   
    SPI->CONFIG &= ~(SPI_CONFIG_MANUAL_CS_Msk);
}

/**
  * @brief Enable Manual Transmit for SPI.
  * @retval None
  */
void SPI_EnManualTransmit(void)
{   
    SPI->CONFIG |= SPI_CONFIG_MANUAL_START_EN_Msk;
}

/**
  * @brief Disable Manual Transmit for SPI.
  * @retval None
  */
void SPI_DisManualTransmit(void)
{   
    SPI->CONFIG &= ~(SPI_CONFIG_MANUAL_START_EN_Msk);
}

/**
  * @brief Start Manual Transmit for SPI.
  * @retval None
  */
void SPI_StartManualTransmit(void)
{  
    SPI->CONFIG |= SPI_CONFIG_MANUAL_START_CMD_Msk;
}

/**
  * @brief Enable Mode Fail Generation for SPI.
  * @retval None
  */
void SPI_ModeFailEnable(void)
{  
    SPI->CONFIG |= SPI_CONFIG_MODE_FAIL_GEN_Msk;
}

/**
  * @brief Disable Mode Fail Generation for SPI.
  * @retval None 
  */
void SPI_ModeFailDisable(void)
{  
    SPI->CONFIG &= ~(SPI_CONFIG_MODE_FAIL_GEN_Msk);
}

/**
  * @brief Set the idle count.
  * @note This is only called in slave mode,SPI detects a start when the 
  *   clk_in remains stable for pclk * idle_count from inactive to active 
  * @retval None
  */
void SPI_IdleCountSet(uint8_t ucCount)
{
	SPI->SIC = ucCount;
}

/**
  * @brief Get the mode of spi.
  * @retval The mode
  */
uint8_t SPI_ModeGet(void)
{
	return (SPI->CONFIG & SPI_CONFIG_MODE_Msk);
}

/**
  * @brief Get the phase of clock.
  * @retval The phase
  */
uint8_t SPI_ClockPhaseGet(void)
{
	return (SPI->CONFIG & SPI_CONFIG_CPHA_Msk);
}

/**
  * @brief Get the polarity of clock.
  * @retval The polarity
  */
uint8_t SPI_ClockPolarityGet(void)
{
	return (SPI->CONFIG & SPI_CONFIG_CPOL_Msk);
}

/**
  * @brief Get the divisor of clock.
  * @retval The divisor value
  */
uint8_t SPI_ClockDivGet(void)
{
	return (SPI->CONFIG & SPI_CONFIG_CLK_DIV_Msk);
}

/**
  * @brief Get the transmission data width of SPI.
  * @retval The data width value
  */
uint8_t SPI_DataWidthGet(void)
{
	return (SPI->CONFIG & SPI_CONFIG_WORD_SIZE_Msk);
}

/**
  * @brief Gets the RX FIFO status of SPI .
  * @param  ucFlagType specifies the flag to check.
  *   This parameter can be one of the following values:
  *   @arg SPI_FIFO_DATA_LEN: The Fifo bytes data number left;when fifo is full, this is 0
  *   @arg SPI_FIFO_FULL: SPI Fifo full flag
  *   @arg SPI_FIFO_EMPTY: SPI Fifo empty flag
  * @retval  The current fifo state 
  */
uint8_t SPI_RxFifoStatusGet(uint8_t ucFlagType)
{
	if(ucFlagType == SPI_FIFO_DATA_LEN)
	{
		return (SPI->RX_FIFO_STATUS & SPI_FIFO_STATUS_DATALEN_Msk);
	}
	else if(ucFlagType == SPI_FIFO_FULL)
	{
		return ((SPI->RX_FIFO_STATUS & SPI_FIFO_STATUS_FULL_Msk)? 1 : 0) ;
	}  
	else
	{
		return ((SPI->RX_FIFO_STATUS & SPI_FIFO_STATUS_EMPTY_Msk)? 1 : 0) ;
	}
}

/**
  * @brief Gets the TX FIFO status of SPI .
  * @param  ucFlagType specifies the flag to check.
  *   This parameter can be one of the following values:
  *   @arg SPI_FIFO_DATA_LEN: The Fifo bytes data number left;when fifo is full, this is 0
  *   @arg SPI_FIFO_FULL: SPI Fifo full flag
  *   @arg SPI_FIFO_EMPTY: SPI Fifo empty flag
  * @retval  The current fifo state 
  */
uint8_t SPI_TxFifoStatusGet(uint8_t ucFlagType)
{
	if(ucFlagType == SPI_FIFO_DATA_LEN)
	{
		return (SPI->TX_FIFO_STATUS & SPI_FIFO_STATUS_DATALEN_Msk);
	}
	else if(ucFlagType == SPI_FIFO_FULL)
	{
		return ((SPI->TX_FIFO_STATUS & SPI_FIFO_STATUS_FULL_Msk)? 1 : 0) ;
	}  
	else
	{
		return ((SPI->TX_FIFO_STATUS & SPI_FIFO_STATUS_EMPTY_Msk)? 1 : 0) ;
	}
}

/**
  * @brief Get the enabled status of SPI.
  * @retval The enabled status
  */
uint8_t SPI_IsEnable(void)
{
    return SPI->ENABLE;
}

/**
  * @brief Registers an interrupt handler for the SPI.
  * @param g_pRAMVectors: the interrupt vector table
  * @param pfnHandler: a pointer to the function to be called when the
  *    synchronous serial interface interrupt occurs
  * @retval None
  */
void SPI_IntRegister( uint32_t *g_pRAMVectors, void (*pfnHandler)(void))
{
    IntRegister(INT_SPI, g_pRAMVectors, pfnHandler);

    /* Enable the interrupt */
    IntEnable(INT_SPI);
}

/**
  * @brief Unregisters an interrupt handler for the SPI.
  * @param g_pRAMVectors: the interrupt vector table
  * @retval None
  */
void SPI_IntUnregister(uint32_t *g_pRAMVectors)
{
    /* Disable the interrupt */
    IntDisable(INT_SPI);

    IntUnregister(INT_SPI, g_pRAMVectors);
}

/**
  * @brief Enable individual SPI interrupt sources.
  * @param ulIntFlags: the bit mask of the interrupt sources to be enable  
  *   This parameter can be any combination of the following values:
  *     @arg SPI_INT_MODE_FAIL: The level of CS pin is inconsistent with SPI mode,
  *       if CS is low in master mode (multi-master contention) or CS goes high
  *       during a transmission in slave mode
  *     @arg SPI_INT_TX_FIFO_NFULL: The data in transmitter FIFO < threshold vaule
  *     @arg SPI_INT_TX_FIFO_FULL: Transmitter FIFO Full 
  *     @arg SPI_INT_TX_FIFO_UNDERFLOW: Transmitter FIFO Overflow 
  *     @arg SPI_INT_RX_OVERFLOW: Receiver FIFO Overflow
  *     @arg SPI_INT_RX_FIFO_NEMPTY: The data in receive FIFO >= threshold vaule
  *     @arg SPI_INT_RX_FIFO_FULL: Receiver FIFO Full 
  *     @arg SPI_INT_ALL
  * @retval None
  */
void SPI_IntEnable(uint32_t ulIntFlags)
{
    SPI->IEN |= ulIntFlags;
}

/**
  * @brief Disables individual SPI interrupt sources.
  * @param ulIntFlags: the bit mask of the interrupt sources to be disable  
  *   This parameter can be any combination of the following values:
  *     @arg SPI_INT_RX_OVERFLOW: Receiver FIFO Overflow
  *     @arg SPI_INT_MODE_FAIL: The level of CS pin is inconsistent with SPI mode,
  *       if CS is low in master mode (multi-master contention) or CS goes high
  *       during a transmission in slave mode
  *     @arg SPI_INT_TX_FIFO_NFULL: The data in transmitter FIFO < threshold vaule
  *     @arg SPI_INT_TX_FIFO_FULL: Transmitter FIFO Full 
  *     @arg SPI_INT_RX_FIFO_NEMPTY: The data in receive FIFO > threshold vaule
  *     @arg SPI_INT_RX_FIFO_FULL: Receiver FIFO Full 
  *     @arg SPI_INT_TX_FIFO_UNDERFLOW: Transmitter FIFO Overflow 
  *     @arg SPI_INT_ALL
  * @retval None
  */
void SPI_IntDisable(uint32_t ulIntFlags)
{
    SPI->IDIS |= ulIntFlags;
}

/**
  * @brief Clear the current interrupt.
  * @param ulIntFlags: The bit mask of the interrupt sources to be clear  
  *   This parameter can be any combination of the following values:
  *     @arg SPI_INT_RX_OVERFLOW: Receiver FIFO Overflow
  *     @arg SPI_INT_MODE_FAIL: The level of CS pin is inconsistent with SPI mode,
  *       if CS is low in master mode (multi-master contention) or CS goes high
  *       during a transmission in slave mode
  *     @arg SPI_INT_TX_FIFO_NFULL: The data in transmitter FIFO < threshold vaule
  *     @arg SPI_INT_TX_FIFO_FULL: Transmitter FIFO Full 
  *     @arg SPI_INT_RX_FIFO_NEMPTY: The data in receive FIFO > threshold vaule
  *     @arg SPI_INT_RX_FIFO_FULL: Receiver FIFO Full 
  *     @arg SPI_INT_TX_FIFO_UNDERFLOW: Transmitter FIFO Overflow 
  *     @arg SPI_INT_ALL
  * @retval None
  */
void SPI_IntClear(uint32_t ulIntFlags)
{
    SPI->INT_STATUS |= ulIntFlags;
}

/**
  * @brief Gets the current interrupt status.
  * @retval The current interrupt status. Enumerated as a bit field of values
  *   described in SPI_IntEnable()
  */
uint32_t SPI_IntStatus(void)
{
     return(SPI->INT_STATUS);
}

/**
  * @brief Gets the current interrupt masked.
  * @param ulIntFlags: The bit mask of the interrupt sources to be disable  
  * @retval The current interrupt status. Enumerated as a bit field of values
  *   described in SPI_IntEnable()
  */
uint32_t SPI_IntMasked(void)
{
	return(SPI->IMASK);
}
#endif


#if 0	//old
#include "spi.h"
#include "interrupt.h"

/**
  * @brief Configures the synchronous serial interface.
  * @param ulDivide: the division of clock frequency 
  *   This parameter can be one of the following values
  *     @arg SPI_CONFIG_CLK_DIV_2
  *     @arg SPI_CONFIG_CLK_DIV_4
  *     @arg SPI_CONFIG_CLK_DIV_8
  *     @arg SPI_CONFIG_CLK_DIV_16
  *     @arg SPI_CONFIG_CLK_DIV_32
  *     @arg SPI_CONFIG_CLK_DIV_64
  *     @arg SPI_CONFIG_CLK_DIV_128
  *     @arg SPI_CONFIG_CLK_DIV_256  
  * @param ulProtocol: specifies the data transfer protocol
  *   This parameter can be one of the following values:
  *     @arg SPI_FRF_MOTO_MODE_0: polarity 0, phase 0
  *     @arg SPI_FRF_MOTO_MODE_1: polarity 0, phase 1
  *     @arg SPI_FRF_MOTO_MODE_2: polarity 1, phase 0
  *     @arg SPI_FRF_MOTO_MODE_3: polarity 1, phase 1
  * @param ulMode: specifies the mode of operation. 
  *   This parameter can be one of the following values:
  *     @arg SPI_CONFIG_MODE_MASTER
  *     @arg SPI_CONFIG_MODE_SLAVE
  * @param ulDataWidth specifies number of bits transferred per frame.
  *   This parameter can be one of the following values:
  *     @arg SPI_CONFIG_WORD_SIZE_BITS_8
  *     @arg SPI_CONFIG_WORD_SIZE_BITS_16
  *     @arg SPI_CONFIG_WORD_SIZE_BITS_32
  * @retval None
  */
void SPIConfigSetExpClk(unsigned long ulDivide, unsigned long ulProtocol, unsigned long ulMode, unsigned long ulDataWidth)                   
{
    HWREG(SPI_CONFIG) = (HWREG(SPI_CONFIG) & ~(SPI_CONFIG_MODE_Msk | SPI_CONFIG_CPOL_Msk | SPI_CONFIG_CPHA_Msk | SPI_CONFIG_CLK_DIV_Msk | SPI_CONFIG_WORD_SIZE_Msk)) |
                        (ulDivide | ulProtocol | ulMode | ulDataWidth);
}

/**
  * @brief Enable the SPI.
  * @note  The SPI must be configured before it is enabled
  * @retval None
  */
void SPIEnable(void)
{
    HWREG(SPI_ENABLE) = 0x1;
}

/**
  * @brief Disables the SPI.
  * @retval None
  */
void SPIDisable(void)
{
    HWREG(SPI_ENABLE) = 0;
}

/**
  * @brief Set the mode of slave selection control
  * @param ulMode: the mode of slave selection control 
  *   This parameter can be one of the following values:
  *     @arg SPI_CONFIG_SS_MODE_NORMAL: the peripheral select signals SS_0/SS_1 only one is
  *       active low at any one time.
  *     @arg SPI_CONFIG_SS_MODE_DECODE: Enable decoding function of slave selection control,the 
  *       peripheral select signals are output directly through SS0&SS1 and connect a 2-bit 
  *       to 4-bit decoder to provide the capability of generating up to 4 slave selects.    
  * @retval None
  */
void SPIChipSelectMode(unsigned long ulMode)
{
    HWREG(SPI_CONFIG) = (HWREG(SPI_CONFIG) & ~(SPI_CONFIG_SS_MODE_Msk)) | ulMode;
}

/**
  * @brief Setting selection channel of peripheral chip.
  * @note if pdec=0, the peripheral select signals SS_0/SS_0 only one is active low 
  *       at any one time; if pdec=1, the peripheral select signals are output directly 
  *       through SS0&SS1 and connect a 2-bit to 4-bit decoder to provide the capability of 
  *       generating up to 4 peripheral chip selects.
  * @param ulChipSelect: the channels 
  *   This parameter can be one of the following values:
  *		if Mode is SPI_CONFIG_SS_MODE_NORMAL, the following parameters are valid:
  *     @arg SPI_CONFIG_SS_LINES_SS0: use SS0 as the control of slave selection
  *     @arg SPI_CONFIG_SS_LINES_SS1: use SS1 as the control of slave selection
  *     @arg SPI_CONFIG_SS_LINES_NONE: no lines is active
  *		if Mode is SPI_CONFIG_SS_MODE_DECODE, the following parameters are valid:
  *     @arg SPI_CONFIG_SS_LINES_OUT_00: SS0 output 0 and SS1 output 0 
  *     @arg SPI_CONFIG_SS_LINES_OUT_01: SS0 output 1 and SS1 output 0 
  *     @arg SPI_CONFIG_SS_LINES_OUT_10: SS0 output 0 and SS1 output 1 
  *     @arg SPI_CONFIG_SS_LINES_OUT_11: SS0 output 1 and SS1 output 1
  * @retval None
  */
void SPIChipSelect(unsigned long ulChipSelect)
{
    HWREG(SPI_CONFIG) = (HWREG(SPI_CONFIG) & ~(SPI_CONFIG_SS_LINES_Msk)) | ulChipSelect;
}

/**
  * @brief Disable the the busrt mode of slave.
  * @note  This function is only used for slave mode,
  *        the spi_clk would running continuously when spi_cs stays active between two bytes.
  * @retval None
  */
void SPISlaveBurstEnable(void)
{
	HWREG(SPI_CONFIG) |= SPI_CONFIG_SLAVE_BURST_Msk;
}

/**
  * @brief Enable the the busrt mode of slave.
  * @note  This function is only used for slave mode,
  *    spi_sclk should be idle for one sclk cycle at least between two bytes.
  * @retval None
  */
void SPISlaveBurstDisable(void)
{
	HWREG(SPI_CONFIG) &= ~SPI_CONFIG_SLAVE_BURST_Msk;
}

/**
  * @brief Set Tx FIFO Threshold.
  * @param ucThreshold: the threshold value(less than 128)
  * @retval None
  */
void SPISetTxFifoThreshold(unsigned char ucThreshold)
{  
    HWREG(SPI_TX_THRESH) = ucThreshold;
}

/**
  * @brief Set Rx FIFO Threshold.
  * @param ucThreshold: the threshold value(less than 128)
  * @retval None
  */
void SPISetRxFifoThreshold(unsigned char ulThreshold)
{
    HWREG(SPI_RX_THRESH) = ulThreshold;
}

/**
  * @brief Reset Tx FIFO.
  * @retval None
  */
void SPITxFifoReset(void)
{
//	HWREG(SPI_TX_FIFO_OP) = (HWREG(SPI_TX_FIFO_OP) & 0x04) | SPI_FIFO_OP_RESET_Msk;
	HWREG(SPI_TX_FIFO_OP) |= SPI_FIFO_OP_RESET_Msk;
	HWREG(SPI_TX_FIFO_OP) &= ~SPI_FIFO_OP_RESET_Msk;
}

/**
  * @brief Enable Tx FIFO.
  * @retval None
  */
void SPITxFifoEnable(void)
{
//	HWREG(SPI_TX_FIFO_OP) = (HWREG(SPI_TX_FIFO_OP) & 0x04) | SPI_FIFO_OP_START_Msk;
	HWREG(SPI_TX_FIFO_OP) |= SPI_FIFO_OP_START_Msk;
}

/**
  * @brief Disable Tx FIFO.
  * @retval None
  */
void SPITxFifoDisable(void)
{
//	HWREG(SPI_TX_FIFO_OP) = (HWREG(SPI_TX_FIFO_OP) & 0x04) & ~SPI_FIFO_OP_START_Msk;
	HWREG(SPI_TX_FIFO_OP) &= ~SPI_FIFO_OP_START_Msk;
}

/**
  * @brief Reset Rx FIFO.
  * @retval None
  */
void SPIRxFifoReset(void)
{
    HWREG(SPI_RX_FIFO_OP) |= SPI_FIFO_OP_RESET_Msk;
	HWREG(SPI_RX_FIFO_OP) &= ~SPI_FIFO_OP_RESET_Msk;
}

/**
  * @brief Enable Rx FIFO.
  * @retval None
  */
void SPIRxFifoEnable(void)
{
    HWREG(SPI_RX_FIFO_OP) |= SPI_FIFO_OP_START_Msk;
}

/**
  * @brief Disable Rx FIFO.
  * @retval None
  */
void SPIRxFifoDisable(void)
{
    HWREG(SPI_RX_FIFO_OP) &= ~SPI_FIFO_OP_START_Msk;
}

/**
  * @brief Set Delay for SPI.
  * @param ucTransInitDelay: The delay value of first bit transfer after 
  *        setting ss_pin low
  *        Actual delay time =(value + 1)*SPI reference clock
  * @param ucTransAfterDelay: The delay value of between bytes 
  *        Cpha = 0: actual delay time = (value + 3)*SPI reference clock
  *        Cpha = 1: actual delay time = (value + 1)*SPI reference clock
  * @param ucTransBetweenDelay: The delay value of between one chip select 
  *        being de-activated and the activation of another.
  *        Actual delay time =(value + 2)*SPI reference clock
  * @param ucTransNActiveDelay: The delay value of master mode chip select 
  *        outputs are de-asserted between words.
  *        Actual delay time =(value + 1)*SPI reference clock
  * @retval None
  */
void SPISetDelay(unsigned char ucTransNActiveDelay, unsigned char ucTransBetweenDelay, unsigned char ucTransAfterDelay, unsigned char ucTransInitDelay)
{ 
    HWREG(SPI_DELAY) = (ucTransNActiveDelay << 24 | ucTransBetweenDelay << 16 | ucTransAfterDelay << 8 | ucTransInitDelay) ;
}

/**
  * @brief Enable Manual CS for SPI Master.
  * @retval None
  */
void SPIManualCsSet(void)
{
    HWREG(SPI_CONFIG) |= SPI_CONFIG_MANUAL_CS_Msk;
}

/**
  * @brief Disable Manual CS for SPI Master.
  * @retval None
  */
void SPIManualCsClear(void)
{   
    HWREG(SPI_CONFIG) &= ~(SPI_CONFIG_MANUAL_CS_Msk);
}

/**
  * @brief Enable Manual Transmit for SPI.
  * @retval None
  */
void SPIEnManualTransmit(void)
{   
    HWREG(SPI_CONFIG) |= SPI_CONFIG_MANUAL_START_EN_Msk;
}

/**
  * @brief Disable Manual Transmit for SPI.
  * @retval None
  */
void SPIDisManualTransmit(void)
{   
    HWREG(SPI_CONFIG) &= ~(SPI_CONFIG_MANUAL_START_EN_Msk);
}

/**
  * @brief Start Manual Transmit for SPI.
  * @retval None
  */
void SPIStartManualTransmit(void)
{  
    HWREG(SPI_CONFIG) |= SPI_CONFIG_MANUAL_START_CMD_Msk;
}

/**
  * @brief Enable Mode Fail Generation for SPI.
  * @retval None
  */
void SPIModeFailEnable(void)
{  
    HWREG(SPI_CONFIG) |= SPI_CONFIG_MODE_FAIL_GEN_Msk;
}

/**
  * @brief Disable Mode Fail Generation for SPI.
  * @retval None 
  */
void SPIModeFailDisable(void)
{  
    HWREG(SPI_CONFIG) &= ~(SPI_CONFIG_MODE_FAIL_GEN_Msk);
}

/**
  * @brief Set the idle count.
  * @note This is only called in slave mode,SPI detects a start when the 
  *   clk_in remains stable for pclk * idle_count from inactive to active 
  * @retval None
  */
void SPIIdleCountSet(unsigned char ucCount)
{
	HWREGB(SPI_SIC) = ucCount;
}

/**
  * @brief Get the mode of spi.
  * @retval The mode
  */
unsigned char SPIModeGet(void)
{
	return (HWREG(SPI_CONFIG) & SPI_CONFIG_MODE_Msk);
}

/**
  * @brief Get the phase of clock.
  * @retval The phase
  */
unsigned char SPIClockPhaseGet(void)
{
	return (HWREG(SPI_CONFIG) & SPI_CONFIG_CPHA_Msk);
}

/**
  * @brief Get the polarity of clock.
  * @retval The polarity
  */
unsigned char SPIClockPolarityGet(void)
{
	return (HWREG(SPI_CONFIG) & SPI_CONFIG_CPOL_Msk);
}

/**
  * @brief Get the divisor of clock.
  * @retval The divisor value
  */
unsigned char SPIClockDivGet(void)
{
	return (HWREG(SPI_CONFIG) & SPI_CONFIG_CLK_DIV_Msk);
}

/**
  * @brief Get the transmission data width of SPI.
  * @retval The data width value
  */
unsigned char SPIDataWidthGet(void)
{
	return (HWREG(SPI_CONFIG) & SPI_CONFIG_WORD_SIZE_Msk);
}

/**
  * @brief Gets the RX FIFO status of SPI .
  * @param  ucFlagType specifies the flag to check.
  *   This parameter can be one of the following values:
  *   @arg SPI_FIFO_DATA_LEN: The Fifo bytes data number left;when fifo is full, this is 0
  *   @arg SPI_FIFO_FULL: SPI Fifo full flag
  *   @arg SPI_FIFO_EMPTY: SPI Fifo empty flag
  * @retval  The current fifo state 
  */
unsigned char SPIRxFifoStatusGet(unsigned char ucFlagType)
{
	if(ucFlagType == SPI_FIFO_DATA_LEN)
	{
		return (HWREGH(SPI_RX_FIFO_STATUS) & SPI_FIFO_STATUS_DATALEN_Msk);
	}
	else if(ucFlagType == SPI_FIFO_FULL)
	{
		return ((HWREGH(SPI_RX_FIFO_STATUS) & SPI_FIFO_STATUS_FULL_Msk)? 1 : 0) ;
	}  
	else
	{
		return ((HWREGH(SPI_RX_FIFO_STATUS) & SPI_FIFO_STATUS_EMPTY_Msk)? 1 : 0) ;
	}
}

/**
  * @brief Gets the TX FIFO status of SPI .
  * @param  ucFlagType specifies the flag to check.
  *   This parameter can be one of the following values:
  *   @arg SPI_FIFO_DATA_LEN: The Fifo bytes data number left;when fifo is full, this is 0
  *   @arg SPI_FIFO_FULL: SPI Fifo full flag
  *   @arg SPI_FIFO_EMPTY: SPI Fifo empty flag
  * @retval  The current fifo state 
  */
unsigned char SPITxFifoStatusGet(unsigned char ucFlagType)
{
	if(ucFlagType == SPI_FIFO_DATA_LEN)
	{
		return (HWREGH(SPI_TX_FIFO_STATUS) & SPI_FIFO_STATUS_DATALEN_Msk);
	}
	else if(ucFlagType == SPI_FIFO_FULL)
	{
		return ((HWREGH(SPI_TX_FIFO_STATUS) & SPI_FIFO_STATUS_FULL_Msk)? 1 : 0) ;
	}  
	else
	{
		return ((HWREGH(SPI_TX_FIFO_STATUS) & SPI_FIFO_STATUS_EMPTY_Msk)? 1 : 0) ;
	}
}

/**
  * @brief Get the enabled status of SPI.
  * @retval The enabled status
  */
unsigned char SPIIsEnable(void)
{
    return HWREG(SPI_ENABLE);
}

/**
  * @brief Registers an interrupt handler for the SPI.
  * @param g_pRAMVectors: the interrupt vector table
  * @param pfnHandler: a pointer to the function to be called when the
  *    synchronous serial interface interrupt occurs
  * @retval None
  */
void SPIIntRegister( unsigned long *g_pRAMVectors, void (*pfnHandler)(void))
{
    IntRegister(INT_SPI, g_pRAMVectors, pfnHandler);

    /* Enable the interrupt */
    IntEnable(INT_SPI);
}

/**
  * @brief Unregisters an interrupt handler for the SPI.
  * @param g_pRAMVectors: the interrupt vector table
  * @retval None
  */
void SPIIntUnregister(unsigned long *g_pRAMVectors)
{
    /* Disable the interrupt */
    IntDisable(INT_SPI);

    IntUnregister(INT_SPI, g_pRAMVectors);
}

/**
  * @brief Enable individual SPI interrupt sources.
  * @param ulIntFlags: the bit mask of the interrupt sources to be enable  
  *   This parameter can be any combination of the following values:
  *     @arg SPI_INT_MODE_FAIL: The level of CS pin is inconsistent with SPI mode,
  *       if CS is low in master mode (multi-master contention) or CS goes high
  *       during a transmission in slave mode
  *     @arg SPI_INT_TX_FIFO_NFULL: The data in transmitter FIFO < threshold vaule
  *     @arg SPI_INT_TX_FIFO_FULL: Transmitter FIFO Full 
  *     @arg SPI_INT_TX_FIFO_UNDERFLOW: Transmitter FIFO Overflow 
  *     @arg SPI_INT_RX_OVERFLOW: Receiver FIFO Overflow
  *     @arg SPI_INT_RX_FIFO_NEMPTY: The data in receive FIFO >= threshold vaule
  *     @arg SPI_INT_RX_FIFO_FULL: Receiver FIFO Full 
  *     @arg SPI_INT_ALL
  * @retval None
  */
void SPIIntEnable(unsigned long ulIntFlags)
{
    HWREG(SPI_IEN) |= ulIntFlags;
}

/**
  * @brief Disables individual SPI interrupt sources.
  * @param ulIntFlags: the bit mask of the interrupt sources to be disable  
  *   This parameter can be any combination of the following values:
  *     @arg SPI_INT_RX_OVERFLOW: Receiver FIFO Overflow
  *     @arg SPI_INT_MODE_FAIL: The level of CS pin is inconsistent with SPI mode,
  *       if CS is low in master mode (multi-master contention) or CS goes high
  *       during a transmission in slave mode
  *     @arg SPI_INT_TX_FIFO_NFULL: The data in transmitter FIFO < threshold vaule
  *     @arg SPI_INT_TX_FIFO_FULL: Transmitter FIFO Full 
  *     @arg SPI_INT_RX_FIFO_NEMPTY: The data in receive FIFO > threshold vaule
  *     @arg SPI_INT_RX_FIFO_FULL: Receiver FIFO Full 
  *     @arg SPI_INT_TX_FIFO_UNDERFLOW: Transmitter FIFO Overflow 
  *     @arg SPI_INT_ALL
  * @retval None
  */
void SPIIntDisable(unsigned long ulIntFlags)
{
    HWREG(SPI_IDIS) |= ulIntFlags;
}

/**
  * @brief Clear the current interrupt.
  * @param ulIntFlags: The bit mask of the interrupt sources to be clear  
  *   This parameter can be any combination of the following values:
  *     @arg SPI_INT_RX_OVERFLOW: Receiver FIFO Overflow
  *     @arg SPI_INT_MODE_FAIL: The level of CS pin is inconsistent with SPI mode,
  *       if CS is low in master mode (multi-master contention) or CS goes high
  *       during a transmission in slave mode
  *     @arg SPI_INT_TX_FIFO_NFULL: The data in transmitter FIFO < threshold vaule
  *     @arg SPI_INT_TX_FIFO_FULL: Transmitter FIFO Full 
  *     @arg SPI_INT_RX_FIFO_NEMPTY: The data in receive FIFO > threshold vaule
  *     @arg SPI_INT_RX_FIFO_FULL: Receiver FIFO Full 
  *     @arg SPI_INT_TX_FIFO_UNDERFLOW: Transmitter FIFO Overflow 
  *     @arg SPI_INT_ALL
  * @retval None
  */
void SPIIntClear(unsigned long ulIntFlags)
{
    HWREG(SPI_INT_STATUS) |= ulIntFlags;
}

/**
  * @brief Gets the current interrupt status.
  * @retval The current interrupt status. Enumerated as a bit field of values
  *   described in SPIIntEnable()
  */
unsigned long SPIIntStatus(void)
{
     return(HWREG(SPI_INT_STATUS));
}

/**
  * @brief Gets the current interrupt masked.
  * @param ulIntFlags: The bit mask of the interrupt sources to be disable  
  * @retval The current interrupt status. Enumerated as a bit field of values
  *   described in SPIIntEnable()
  */
unsigned long SPIIntMasked(void)
{
	return(HWREG(SPI_IMASK));
}
#endif
