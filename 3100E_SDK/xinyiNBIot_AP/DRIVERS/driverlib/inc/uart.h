#ifndef __UART_H__
#define __UART_H__

#include "hw_uart.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Values that can be passed to UARTFIFOFlush() or UARTFIFOStatus().
//
//*****************************************************************************
#define UART_FIFO_RX      0x01
#define UART_FIFO_TX      0x02
#define UART_FIFO_ALL     0x03

#define	UART_FIFO_EMPTY       0x01
#define	UART_FIFO_FULL        0x02
#define	UART_FIFO_LEVEL       0x03
#define	UART_FIFO_DATA_LEN    0x04
//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void UARTEnable(unsigned long ulBase);
extern void UARTDisable(unsigned long ulBase);
extern unsigned char UART_Enable_Get(unsigned long ulBase);
extern void UARTFIFOEnable(unsigned long ulBase, unsigned char ucFIFOFlags);
extern void UARTFIFODisable(unsigned long ulBase, unsigned char ucFIFOFlags);
extern unsigned char UART_TxEnable_Get(unsigned long ulBase);
extern void UARTDmaTransferEnable(unsigned long ulBase);
extern void UARTDmaTransferDisable(unsigned long ulBase);
extern void UART_RXFIFO_LevelSet(unsigned long ulBase, unsigned long ulRxLevel);
extern void UART_TXFIFO_LevelSet(unsigned long ulBase, unsigned long ulTxLevel);
extern void  UART_Enable_FrameParityErrToWFIFO(unsigned long ulBase);
extern void  UART_Disable_FrameParityErrToWFIFO(unsigned long ulBase);
extern void UARTTxWaitSet(unsigned long ulBase, unsigned char ucWaitTime);
extern void UARTWaitTxDone(unsigned long ulBase);
extern void UARTConfigSetExpClk(unsigned long ulBase, unsigned long ulPclk,
                                unsigned long ulBaudrate, unsigned long ulConfig);
extern void UART_ConfigRegister_Set(unsigned long ulBase, unsigned long ulConfig);
extern void UARTConfigGetExpClk(unsigned long ulBase, unsigned long ulPclk,
                                unsigned long *pulBaud, unsigned long *pulConfig);
extern void UARTAutoBaudrate(unsigned long ulBase, unsigned long ulConfig);
extern unsigned char UARTABDEndStatus(unsigned long ulBase);
extern void UARTIntEnable(unsigned long ulBase, unsigned long ulIntFlags);
extern void UARTIntDisable(unsigned long ulBase, unsigned long ulIntFlags);
extern unsigned short UARTIntRead(unsigned long ulBase);
extern void UART_IntClear(unsigned long ulBase,uint32_t RegValue);
extern unsigned short UARTIntMasked(unsigned long ulBase);
extern unsigned char UARTRxIdle(unsigned long ulBase);
extern void UARTFIFOFlush(unsigned long ulBase, unsigned char ucFIFOFlags);
extern unsigned char UARTRxFifoStatusGet(unsigned long ulBase, unsigned char ucFlagType);
extern unsigned char UARTTxFifoStatusGet(unsigned long ulBase, unsigned char ucFlagType);
extern unsigned char UART_TxDoneStatus_Get(unsigned long ulBase);
extern unsigned char UARTFIFOByteLevel(unsigned long ulBase, unsigned char ucFIFOFlags);
#if 1

/**
 * @brief ����LPUART��TXFIO�Ƿ�����
 * @note  ��LPUART�Ĳο�ʱ��Ϊ32Kʱ����ʹ�øýӿڣ���ʱTX�󲿷�״̬����������Ҫ��3��32k���ڡ�
 *        ֻ��txfifo_level�Ĵ�����txfifo_byte_level�Ĵ����ĸ����ǿ�ġ�
 *        ���LPUART�ķ���״ֻ̬��ʹ���������Ĵ������жϡ�
 * @return 1:txfifo���ˣ�0:txfifoû��
 */
extern unsigned char LPUART_IS_TXFIFO_FULL(void);

/**
 * @brief ����LPUART��TXFIO�Ƿ�ա�
 * @note  ��LPUART�Ĳο�ʱ��Ϊ32Kʱ����ʹ�øýӿڣ���ʱTX�󲿷�״̬����������Ҫ��3��32k���ڡ�
 *        ֻ��txfifo_level�Ĵ�����txfifo_byte_level�Ĵ����ĸ����ǿ�ġ�
 *        ���LPUART�ķ���״ֻ̬��ʹ���������Ĵ������жϡ�
 * @return 1:txfifo���ˣ�0:txfifoû��
 */
extern unsigned char LPUART_IS_TXFIFO_EMPTY(void);

#endif
extern void UARTWakeUpModeConfig(unsigned long ulBase, unsigned char ucConfig, unsigned char ucPattern1, unsigned char ucPattern2);
extern void UARTWakeUpModeEnable(unsigned long ulBase);
extern void UARTWakeUpModeDisable(unsigned long ulBase);
extern void UARTSequenceDetectModeSet(unsigned long ulBase, unsigned char ulValidBits, unsigned char ulPattern);
extern void UARTSequenceDetectEnable(unsigned long ulBase);
extern void UARTSequenceDetectDisable(unsigned long ulBase);
extern void UARTTimeOutConfig(unsigned long ulBase, unsigned char ucStartCondition, unsigned char ucValue);
extern void UARTTimeOutCondition_Set(unsigned long ulBase, unsigned char ucStartCondition);
extern unsigned char UART_TimeoutCondition_Get(unsigned long ulBase);
extern void UARTTimeOutEnable(unsigned long ulBase);
extern void UARTTimeOutDisable(unsigned long ulBase);
extern unsigned char UARTRXEnaStatus(unsigned long ulBase);
extern void UARTFlowCtrlEnable(unsigned long ulBase);
extern void UARTFlowCtrlDisable(unsigned long ulBase);
extern void UARTFlowCtrlConfig(unsigned long ulBase, unsigned char ucConfig);
extern unsigned char UARTFlowCtrlRtsGet(unsigned long ulBase);
extern void UARTFlowCtrlRtsSet(unsigned long ulBase);
extern void UARTFlowCtrlRtsClear(unsigned long ulBase);
extern unsigned char UARTFlowCtrlCtsGet(unsigned long ulBase);
extern void UARTFlowCtrlCtsSet(unsigned long ulBase);
extern void UARTFlowCtrlCtsClear(unsigned long ulBase);
extern void UARTStartOffsetConfig(unsigned long ulBase, unsigned char ucValue);
extern void UARTStartOffsetEnable(unsigned long ulBase);
extern unsigned char UARTStartOffsetFlag(unsigned long ulBase);
extern unsigned char UARTCharGet(unsigned long ulBase);
extern unsigned char UARTReadData(unsigned long ulBase);
extern unsigned long UARTCharGetNonBlocking(unsigned long ulBase);
extern void UARTCharPut(unsigned long ulBase, unsigned char ucData);
extern void UARTWriteData(unsigned long ulBase, unsigned char ucData);
extern unsigned char UARTCharPutNonBlocking(unsigned long ulBase, unsigned char ucData);
extern void UARTPrintf(unsigned long ulBase, char* fmt, ...);
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __UART_H__
