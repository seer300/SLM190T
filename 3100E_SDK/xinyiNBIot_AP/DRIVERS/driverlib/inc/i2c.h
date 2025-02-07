#if 1	//new
#ifndef __I2C_H__
#define __I2C_H__

#include "hw_i2c.h"
#include "hw_ints.h"
#include "xinyi2100.h"

#include "hw_types.h"
#include "debug.h"
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

typedef enum {
    I2C_slave = 0,
    I2C_master_normal_noStartByte = 8,
    I2C_master_normal_withStartByte = 9,
    I2C_master_hs_noStartByte = 10,
    I2C_master_hs_withStartByte = 11,
    I2C_master_generalCallAddress = 12
}I2C_workmode;

typedef enum {
    I2C_addr_7b = 0,
    I2C_addr_10b = 1
}I2C_addrbits;

typedef enum {
    I2C_standard_mode = 0,    //100k
    I2C_fast_mode,            //400k
    I2C_fast_mode_pluse,      //1M
    I2C_highspeed_mode        //3.4M
}I2C_clockmode;

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void I2C_Init(I2C_TypeDef* I2Cx,
                I2C_workmode workmode,
                I2C_clockmode clockmode,
                I2C_addrbits addrbits,
                uint32_t pclk);

extern void I2C_Reset(I2C_TypeDef* I2Cx);
extern void I2C_DeInit(I2C_TypeDef* I2Cx);
extern void I2C_SetFiFoThreshold(I2C_TypeDef* I2Cx, uint8_t TxFiFoThreshold, uint8_t RxFiFoThreshold);
extern void I2C_SetTxFiFoThreshold(I2C_TypeDef *I2Cx, uint8_t TxFiFoThreshold);
extern void I2C_SetRxFiFoThreshold(I2C_TypeDef *I2Cx, uint8_t RxFiFoThreshold);
extern void I2C_SetAddr(I2C_TypeDef* I2Cx, uint16_t addr);
extern void I2C_SetMasterClockStretch(I2C_TypeDef* I2Cx, uint8_t enable);
extern void I2C_SetDelayBetweenByte(I2C_TypeDef* I2Cx, uint8_t enable, uint8_t delayBetweenByte);
extern void I2C_IntEnable(I2C_TypeDef* I2Cx, uint32_t ulIntFlags);
extern void I2C_IntDisable(I2C_TypeDef* I2Cx, uint32_t ulIntFlags);
extern void I2C_IntClear(I2C_TypeDef* I2Cx, uint32_t ulIntFlags);
extern uint32_t I2C_IntStatus(I2C_TypeDef* I2Cx);
extern void I2C_IntRegister(I2C_TypeDef* I2Cx, uint32_t *g_pRAMVectors, void (*pfnHandler)(void));
extern void I2C_IntUnregister(I2C_TypeDef* I2Cx, uint32_t *g_pRAMVectors);
extern uint16_t I2C_Status(I2C_TypeDef* I2Cx);
extern uint8_t I2C_TxFiFoLevel(I2C_TypeDef* I2Cx);
extern uint8_t I2C_RxFiFoLevel(I2C_TypeDef* I2Cx);
extern void I2C_PutData(I2C_TypeDef* I2Cx, uint16_t data);
extern uint8_t I2C_GetData(I2C_TypeDef* I2Cx);
extern uint32_t I2C_GetDataNonBlocking(I2C_TypeDef *I2Cx);
extern uint32_t I2C_MasterWriteData(I2C_TypeDef* I2Cx, uint8_t *pdata, uint32_t len);
extern uint32_t I2C_SlaveWriteData(I2C_TypeDef* I2Cx, uint8_t *pdata, uint32_t len);
extern uint32_t I2C_MasterReadData(I2C_TypeDef* I2Cx, uint8_t *pdata, uint32_t len);
extern uint32_t I2C_SlaveReadData(I2C_TypeDef* I2Cx, uint8_t *pdata, uint32_t len);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __I2C_H__

#endif	//new



#if 0	//old
#ifndef __I2C_H__
#define __I2C_H__

#include "hw_i2c.h"
#include "hw_ints.h"
#include "xinyi2100.h"

#include "hw_types.h"
#include "debug.h"
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

typedef enum {
    I2C_slave = 0,
    I2C_master_normal_noStartByte = 8,
    I2C_master_normal_withStartByte = 9,
    I2C_master_hs_noStartByte = 10,
    I2C_master_hs_withStartByte = 11,
    I2C_master_generalCallAddress = 12
}I2C_workmode;

typedef enum {
    I2C_addr_7b = 0,
    I2C_addr_10b = 1
}I2C_addrbits;

typedef enum {
    I2C_standard_mode = 0,    //100k
    I2C_fast_mode,            //400k
    I2C_fast_mode_pluse,      //1M
    I2C_highspeed_mode        //3.4M
}I2C_clockmode;

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void I2CInit(unsigned long ulBase,
                I2C_workmode workmode,
                I2C_clockmode clockmode,
                I2C_addrbits addrbits,
                unsigned long pclk);

extern void I2CReset(unsigned long ulBase);
extern void I2CDeInit(unsigned long ulBase);
extern void I2CSetFiFoThreshold(unsigned long ulBase, unsigned char TxFiFoThreshold, unsigned char RxFiFoThreshold);
extern void I2CSetAddr(unsigned long ulBase, unsigned short addr);
extern void I2CSetMasterClockStretch(unsigned long ulBase, unsigned char enable);
extern void I2CSetDelayBetweenByte(unsigned long ulBase, unsigned char enable, unsigned char delayBetweenByte);
extern void I2CIntEnable(unsigned long ulBase, unsigned long ulIntFlags);
extern void I2CIntDisable(unsigned long ulBase, unsigned long ulIntFlags);
extern void I2CIntClear(unsigned long ulBase, unsigned long ulIntFlags);
extern unsigned long I2CIntStatus(unsigned long ulBase);
extern void I2CIntRegister(unsigned long ulBase, unsigned long *g_pRAMVectors, void (*pfnHandler)(void));
extern void I2CIntUnregister(unsigned long ulBase, unsigned long *g_pRAMVectors);
extern unsigned short I2CStatus(unsigned long ulBase);
extern unsigned char I2CTxFiFoLevel(unsigned long ulBase);
extern unsigned char I2CRxFiFoLevel(unsigned long ulBase);
extern void I2CPutData(unsigned long ulBase, unsigned short data);
extern unsigned char I2CGetData(unsigned long ulBase);
extern unsigned long I2CMasterWriteData(unsigned long ulBase, unsigned char *pdata, unsigned long len);
extern unsigned long I2CSlaveWriteData(unsigned long ulBase, unsigned char *pdata, unsigned long len);
extern unsigned long I2CMasterReadData(unsigned long ulBase, unsigned char *pdata, unsigned long len);
extern unsigned long I2CSlaveReadData(unsigned long ulBase, unsigned char *pdata, unsigned long len);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __I2C_H__

#endif	//old
