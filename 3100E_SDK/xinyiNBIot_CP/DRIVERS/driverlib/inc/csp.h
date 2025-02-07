#ifndef __CSP_H__
#define __CSP_H__

#include "hw_csp.h"
#include "interrupt.h"

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

#define CSP_MODE_UART           1
#define CSP_MODE_IRDA           2
#define CSP_MODE_SPI_Master     3
#define CSP_MODE_SPI_Slave      4
#define CSP_MODE_SIM            5
    
#define CSP_PIN_CLK             CSP_MODE1_CLK_PIN_MODE_Pos
#define CSP_PIN_RFS             CSP_MODE1_RFS_PIN_MODE_Pos
#define CSP_PIN_TFS             CSP_MODE1_TFS_PIN_MODE_Pos
#define CSP_PIN_RXD             CSP_MODE1_RXD_PIN_MODE_Pos
#define CSP_PIN_TXD             CSP_MODE1_TXD_PIN_MODE_Pos

#define CSP_PIN_MODE_HW         0
#define CSP_PIN_MODE_IO         1

#define CSP_PIN_OUTPUT          0
#define CSP_PIN_INPUT           1
	
#define CSP_FIFO_EMPTY     0x01
#define CSP_FIFO_FULL      0x02
#define CSP_FIFO_LEVEL     0x03	

extern void CSPUARTModeSet(unsigned long ulBase, unsigned long ulPclk, unsigned long ulBaudrate, unsigned char ucDatabits, unsigned long ulParityCheck, unsigned char ucStopbits);
//extern void CSPIrdaMode(unsigned long ulBase, unsigned long ulPclk, unsigned long ulBaudrate, unsigned char ucDatabits, unsigned char ucStopbits, unsigned long ulIdleLevel, unsigned long ulWidth);
extern void CSPSPIMode(unsigned long ulBase, unsigned long ulMode1, unsigned long ulMode2, unsigned long ulTxFrameCtl, unsigned long ulRxFrameCtl);
extern void CSPFIFOLevelSet(unsigned long ulBase, unsigned long ulTxLevel, unsigned long ulRxLevel);
extern unsigned int CSPCharGet(unsigned long ulBase);
extern void CSPCharPut(unsigned long ulBase, unsigned int ucData);
extern unsigned char CSPRxFifoStatusGet(unsigned long ulBase, unsigned char ucFlagType);
extern unsigned char CSPTxFifoStatusGet(unsigned long ulBase, unsigned char ucFlagType);
extern long CSPCharGetNonBlocking(unsigned long ulBase);
extern long CSPCharPutNonBlocking(unsigned long ulBase, unsigned char ucData);
extern void CSPOperationModeSet(unsigned long ulBase, unsigned long ulOperationMode);
extern void CSPClockModeSet(unsigned long ulBase, unsigned long ulClockMode);
extern void CSPHwDetectClockSet(unsigned long ulBase, unsigned long ulPclk, unsigned ulClock);
extern void CSPIrdaEnable(unsigned long ulBase, unsigned long ulIrdaEn);
extern void CSPEndianModeSet(unsigned long ulBase, unsigned long ulEndianMode);
extern void CSPEnable(unsigned long ulBase);
extern void CSPDisable(unsigned long ulBase);
extern void CSPDrivenEdgeSet(unsigned long ulBase, unsigned long ulRxEdge, unsigned long ulTxEdge);
extern void CSPSyncValidLevelSet(unsigned long ulBase, unsigned long ulRFS, unsigned long ulTFS);
extern void CSPClockIdleModeSet(unsigned long ulBase, unsigned long ulClkIdleMode);
extern void CSPClockIdleLevelSet(unsigned long ulBase, unsigned long ulClkIdleLevel);
extern void CSPPinModeSet(unsigned long ulBase, unsigned long ulCSPPin, unsigned long ulPinMode, unsigned long ulPinDirection);
extern void CSPIrdaPulseWidthSet(unsigned long ulBase, unsigned long ulWidth);
extern void CSPIrdaIdleLevelSet(unsigned long ulBase, unsigned long ulIrdaIdleLevel);
extern void CSPClockSet(unsigned long ulBase, unsigned long ulCSPMode, unsigned long ulPclk, unsigned ulClock);
extern void CSPTxFrameSet(unsigned long ulBase, unsigned long ulDelay, unsigned long ulDataLen, unsigned long ulSyncLen, unsigned long ulFrameLen);
extern void CSPRxFrameSet(unsigned long ulBase, unsigned long ulDelay, unsigned long ulDataLen, unsigned long ulFrameLen);
extern void CSPRxEnable(unsigned long ulBase);
extern void CSPRxDisable(unsigned long ulBase);
extern unsigned char CSPRxStatusGet(unsigned long ulBase);
extern void CSPTxEnable(unsigned long ulBase);
extern void CSPTxDisable(unsigned long ulBase);
extern void CSPIntRegister(unsigned long ulBase, unsigned long *g_pRAMVectors, void(*pfnHandler)(void));
extern void CSPIntUnregister(unsigned long ulBase, unsigned long *g_pRAMVectors);
extern void CSPIntEnable(unsigned long ulBase, unsigned long ulIntFlags);
extern void CSPIntDisable(unsigned long ulBase, unsigned long ulIntFlags);
extern unsigned long CSPIntStatus(unsigned long ulBase);
extern void CSPIntClear(unsigned long ulBase, unsigned long ulIntFlags);
extern void CSPDMAConfigRX(unsigned long ulBase, unsigned char ucLevelCheck, unsigned long ulDataLen);
extern void CSPDMAConfigTX(unsigned long ulBase, unsigned char ucLevelCheck, unsigned long ulDataLen);
extern void CSPUARTRxTimeoutConfig(unsigned long ulBase, unsigned char relevantFifoEmpty, unsigned short timeout_num);


extern void CSPSPIConfigSetExpClk(unsigned long ulBase, unsigned char ulMode, unsigned long ulPclk, unsigned ulClock, unsigned char CPOL, unsigned char CPHA);
extern void CSPSetRxFifoThreshold(unsigned long ulBase, unsigned long ulThreshold);
extern void CSPSetTxFifoThreshold(unsigned long ulBase, unsigned long ulThreshold);
extern void CSPFifoReset(unsigned long ulBase);
extern void CSPFifoStart(unsigned long ulBase);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __CSP_H__
