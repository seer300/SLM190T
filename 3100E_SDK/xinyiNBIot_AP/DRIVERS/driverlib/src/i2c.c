#if 1	//new
#include "i2c.h"

/**
  * @brief  init the I2C module.
  * @param  ulBase is the base address of the I2C module.
  * @param  workmode is the I2C work mode as following:
  *     @arg I2C_slave
  *     @arg I2C_master_normal_noStartByte
  *     @arg I2C_master_normal_withStartByte
  *     @arg I2C_master_hs_noStartByte
  *     @arg I2C_master_hs_withStartByte
  *     @arg I2C_master_generalCallAddress
  * @param  clockmode is the I2C clock mode as following:
  *     @arg I2C_standard_mode
  *     @arg I2C_fast_mode
  *     @arg I2C_fast_mode_pluse
  *     @arg I2C_highspeed_mode
  * @param  addrbits is the I2C address bits as following:
  *     @arg I2C_addr_7b
  *     @arg I2C_addr_10b
  * @param  pclk is the I2C peripheral clock frequency.
  * @retval None
  */
void I2C_Init(I2C_TypeDef* I2Cx,
                I2C_workmode workmode,
                I2C_clockmode clockmode,
                I2C_addrbits addrbits,
                uint32_t pclk)
{
    uint16_t fs_scl_hcnt, fs_scl_lcnt;
    uint32_t baudrate = 0;
    uint8_t spklen;
    uint16_t pwmcnt;

    (void)spklen;

    ASSERT((I2Cx == I2C1) || (I2Cx == I2C2));

    I2Cx->CTL = (I2Cx->CTL & (~I2C_CTL_WORK_MODE_Msk)) | workmode;

    I2Cx->CTL |= (I2C_CTL_SLV_RXF_CS_EN | I2C_CTL_SLV_TXE_CS_EN | I2C_CTL_MST_CS_EN | I2C_CTL_D_BYTE_EN);

    if (addrbits == I2C_addr_10b) {
        I2Cx->CTL |= I2C_CTL_MST_ADD_10B_Msk;   //10bits addr
    } else {
        I2Cx->CTL &= ~I2C_CTL_MST_ADD_10B_Msk;  //7bits addr
    }

    //set the duration of spike suppression logic
    I2Cx->HS_SPKLEN = 0x00;
    I2Cx->FS_SPKLEN = 0x00;

    I2Cx->TX_FIFO_THD = 0x08;
    I2Cx->RX_FIFO_THD = 0x18;

    if (clockmode == I2C_standard_mode) {
        baudrate = 100000;
    } else if (clockmode == I2C_fast_mode) {
        baudrate = 400000;
    }else if (clockmode == I2C_fast_mode_pluse) {
        baudrate = 1000000;
    }else if (clockmode == I2C_highspeed_mode) {
        baudrate = 3400000;
    }

    pwmcnt = (uint16_t)((pclk/baudrate)>>1);

    spklen = I2Cx->FS_SPKLEN;
    //fs_scl_hcnt = (pwmcnt - 4 - spklen >= 0)? (pwmcnt - 4 - spklen) : 0x14;
    //fs_scl_lcnt = pwmcnt - 1;
    fs_scl_hcnt = pwmcnt - 1-3;
    fs_scl_lcnt = pwmcnt - 1+3;

    //set scl HCNT, LCNT
    I2Cx->FS_SCL_HCNT = fs_scl_hcnt;
    I2Cx->FS_SCL_LCNT = fs_scl_lcnt;

    I2Cx->HS_SCL_HCNT = (uint16_t)((pclk/3400000)>>1);
    I2Cx->HS_SCL_LCNT = (uint16_t)((pclk/3400000)>>1);

    //set sda holdtime
    I2Cx->HS_SDA_HOLD = 0x01;
    I2Cx->FS_SDA_HOLD = 0x08;  //0x1A;  //0x20;

    I2Cx->HS_MC = 0x07;           //hs_mc
    
    // 总线上有1个主机控制器时，设为0，sck拉低超过TIMEOUT可以触发timeout中断
    // 如果总线上有多个主机控制器，不能设为0
    I2Cx->BUS_IDLE_CNT = 0x00;    

    I2Cx->TIMEOUT = 0xFFFF;

    I2Cx->D_SP_SR = 0x05050505;

    I2Cx->D_BUS_FREE = 0x0F;

    I2Cx->D_BYTE = 0x30;
}

/**
  * @brief  reset the I2C module.
  * @param  ulBase is the base address of the I2C module.
  * @retval None
  */
void I2C_Reset(I2C_TypeDef* I2Cx)
{
    I2Cx->CTL |= I2C_CTL_RSTN;
    I2Cx->CTL &= ~I2C_CTL_RSTN;
}

/**
  * @brief  deinit the I2C module.
  * @param  ulBase is the base address of the I2C module.
  * @retval None
  */
void I2C_DeInit(I2C_TypeDef* I2Cx)
{
    I2C_Reset(I2Cx);                           //clear fifo and reset FSM.
    I2Cx->CTL = 0;                //reset I2C control
    I2Cx->INT_STAT = 0xFFFFFFFF;  //clear all interrupt status
}

void I2C_SetTxFiFoThreshold(I2C_TypeDef* I2Cx, uint8_t TxFiFoThreshold)
{
    ASSERT((TxFiFoThreshold <= 32));

    I2Cx->TX_FIFO_THD = TxFiFoThreshold;
}

void I2C_SetRxFiFoThreshold(I2C_TypeDef* I2Cx, uint8_t RxFiFoThreshold)
{
    ASSERT((RxFiFoThreshold <= 32));

    I2Cx->RX_FIFO_THD = RxFiFoThreshold;
}
/**
  * @brief  set the I2C FiFo threshold.
  * @param  ulBase is the base address of the I2C module.
  * @param  TxFiFoThreshold is the I2C TX FiFo threshold
  * @param  RxFiFoThreshold is the I2C RX FiFo threshold
  * @retval None
  */
void I2C_SetFiFoThreshold(I2C_TypeDef* I2Cx, uint8_t TxFiFoThreshold, uint8_t RxFiFoThreshold)
{
    ASSERT((TxFiFoThreshold <= 32) && (RxFiFoThreshold <= 32));

    I2Cx->TX_FIFO_THD = TxFiFoThreshold;
    I2Cx->RX_FIFO_THD = RxFiFoThreshold;
}

/**
  * @brief  set the I2C device address.
  * @param  ulBase is the base address of the I2C module.
  * @param  addr is the I2C device address.
  * @retval None
  */
void I2C_SetAddr(I2C_TypeDef* I2Cx, uint16_t addr)
{
    ASSERT((I2Cx == I2C1) || (I2Cx == I2C2));

    I2Cx->ADDR = addr;
}

/**
  * @brief  enable the master clock stretch or not.
  * @param  ulBase is the base address of the I2C module.
  * @param  enable is 1:enable, 0:disable.
  * @retval None
  */
void I2C_SetMasterClockStretch(I2C_TypeDef* I2Cx, uint8_t enable)
{
    if (enable) {
        I2Cx->CTL |= I2C_CTL_MST_CS_EN;
    } else {
        I2Cx->CTL &= ~I2C_CTL_MST_CS_EN;
    }
}

/**
  * @brief  set the I2C delay between two bytes.
  * @param  ulBase is the base address of the I2C module.
  * @param  enable is 1:enable, 0:disable.
  * @param  delayBetweenByte is the delay between two bytes.
  * @retval None
  */
void I2C_SetDelayBetweenByte(I2C_TypeDef* I2Cx, uint8_t enable, uint8_t delayBetweenByte)
{
    if (enable) {
        I2Cx->CTL |= I2C_CTL_D_BYTE_EN;
    } else {
        I2Cx->CTL &= ~I2C_CTL_D_BYTE_EN;
    }

    I2Cx->D_BYTE = delayBetweenByte;
}

/**
  * @brief  enable the I2C interrupt.
  * @param  ulBase is the base address of the I2C module.
  * @param ulIntFlags: the bit mask of the interrupt sources to be enable
  *   This parameter can be any combination of the following values:
  *     @arg I2C_INT_COMP:          transfer complete
  *     @arg I2C_INT_RX_UNF:        RX fifo underflow
  *     @arg I2C_INT_RX_OVF:        RX fifo overflow
  *     @arg I2C_INT_TX_OVF:        TX fifo overflow
  *     @arg I2C_INT_ACT:           I2C activity from idle status
  *     @arg I2C_INT_GEN_CALL:      trigger only when a General Call address is received and it is acknowledged in slave mode.
  *     @arg I2C_INT_ARB_LOST:      arbitration lost
  *     @arg I2C_INT_TIMEOUT:       timeout
  *     @arg I2C_INT_RX_AF:         RX fifo almost full
  *     @arg I2C_INT_TX_AE:         TX fifo almost empty
  *     @arg I2C_INT_REQ_BUSY:      trigger when the slave is busy
  *     @arg I2C_INT_MST_ON_HOLD:   Indicates whether a master is holding the bus
  *     @arg I2C_INT_SLV_ON_HOLD:   Indicates whether a slave is holding the bus
  *     @arg I2C_INT_MST_NACK:      Indicates master receive NACK
  *     @arg I2C_INT_ALL
  * @retval None
  */
void I2C_IntEnable(I2C_TypeDef* I2Cx, uint32_t ulIntFlags)
{
    I2Cx->INT_MASK &= ~ulIntFlags;
}

/**
  * @brief  disable the I2C interrupt.
  * @param  ulBase is the base address of the I2C module.
  * @param ulIntFlags: the bit mask of the interrupt sources to be disable
  *   This parameter can be any combination of the following values:
  *     @arg I2C_INT_COMP:          transfer complete
  *     @arg I2C_INT_RX_UNF:        RX fifo underflow
  *     @arg I2C_INT_RX_OVF:        RX fifo overflow
  *     @arg I2C_INT_TX_OVF:        TX fifo overflow
  *     @arg I2C_INT_ACT:           I2C activity from idle status
  *     @arg I2C_INT_GEN_CALL:      trigger only when a General Call address is received and it is acknowledged in slave mode.
  *     @arg I2C_INT_ARB_LOST:      arbitration lost
  *     @arg I2C_INT_TIMEOUT:       timeout
  *     @arg I2C_INT_RX_AF:         RX fifo almost full
  *     @arg I2C_INT_TX_AE:         TX fifo almost empty
  *     @arg I2C_INT_REQ_BUSY:      trigger when the slave is busy
  *     @arg I2C_INT_MST_ON_HOLD:   Indicates whether a master is holding the bus
  *     @arg I2C_INT_SLV_ON_HOLD:   Indicates whether a slave is holding the bus
  *     @arg I2C_INT_MST_NACK:      Indicates master receive NACK
  *     @arg I2C_INT_ALL
  * @retval None
  */
void I2C_IntDisable(I2C_TypeDef* I2Cx, uint32_t ulIntFlags)
{
    I2Cx->INT_MASK |= ulIntFlags;
}

/**
  * @brief  clear the I2C interrupt.
  * @param  ulBase is the base address of the I2C module.
  * @param ulIntFlags: the bit mask of the interrupt sources to be cleared
  *   This parameter can be any combination of the following values:
  *     @arg I2C_INT_COMP:          transfer complete
  *     @arg I2C_INT_RX_UNF:        RX fifo underflow
  *     @arg I2C_INT_RX_OVF:        RX fifo overflow
  *     @arg I2C_INT_TX_OVF:        TX fifo overflow
  *     @arg I2C_INT_ACT:           I2C activity from idle status
  *     @arg I2C_INT_GEN_CALL:      trigger only when a General Call address is received and it is acknowledged in slave mode.
  *     @arg I2C_INT_ARB_LOST:      arbitration lost
  *     @arg I2C_INT_TIMEOUT:       timeout
  *     @arg I2C_INT_RX_AF:         RX fifo almost full
  *     @arg I2C_INT_TX_AE:         TX fifo almost empty
  *     @arg I2C_INT_REQ_BUSY:      trigger when the slave is busy
  *     @arg I2C_INT_MST_ON_HOLD:   Indicates whether a master is holding the bus
  *     @arg I2C_INT_SLV_ON_HOLD:   Indicates whether a slave is holding the bus
  *     @arg I2C_INT_MST_NACK:      Indicates master receive NACK
  *     @arg I2C_INT_ALL
  * @retval None
  */
void I2C_IntClear(I2C_TypeDef* I2Cx, uint32_t ulIntFlags)
{
    I2Cx->INT_STAT |= ulIntFlags;
}
/**
  * @brief  get the I2C control status.
  * @param  ulBase is the base address of the I2C module.
  * @retval the I2C control status.
  */
uint16_t I2C_Status(I2C_TypeDef* I2Cx)
{
    return I2Cx->STAT;
}

/**
  * @brief  get the I2C interrupt status.
  * @param  ulBase is the base address of the I2C module.
  * @retval the I2C interrupt status.
  */
uint32_t I2C_IntStatus(I2C_TypeDef* I2Cx)
{
    return I2Cx->INT_STAT;
}

/**
  * @brief  Registers an interrupt handler for I2C interrupt.
  * @param  g_pRAMVectors is the global interrupt vectors table.
  * @param  pfnHandler is a pointer to the function to be called when the I2C interrupt occurs.
  * @retval None
  */
void I2C_IntRegister(I2C_TypeDef* I2Cx, uint32_t *g_pRAMVectors, void (*pfnHandler)(void))
{
    uint32_t ulInt;

    ulInt = (I2Cx == I2C1) ? INT_I2C1 : INT_I2C2;

    IntRegister(ulInt, g_pRAMVectors, pfnHandler);
    IntEnable(ulInt);
}

/**
  * @brief  Unregisters an interrupt handler for the I2C interrupt.
  * @param  g_pRAMVectors is the global interrupt vectors table.
  * @retval None
  */
void I2C_IntUnregister(I2C_TypeDef* I2Cx, uint32_t *g_pRAMVectors)
{
    uint32_t ulInt;

    ulInt = (I2Cx == I2C1) ? INT_I2C1 : INT_I2C2;

    IntDisable(ulInt);
    IntUnregister(ulInt, g_pRAMVectors);
}


/**
  * @brief  get the I2C TX fifo level.
  * @param  ulBase is the base address of the I2C module.
  * @retval the I2C TX fifo level.
  */
uint8_t I2C_TxFiFoLevel(I2C_TypeDef* I2Cx)
{
    return I2Cx->TX_FIFO_LEVEL;
}

/**
  * @brief  get the I2C RX fifo level.
  * @param  ulBase is the base address of the I2C module.
  * @retval the I2C RX fifo level.
  */
uint8_t I2C_RxFiFoLevel(I2C_TypeDef* I2Cx)
{
    return I2Cx->RX_FIFO_LEVEL;
}

/**
  * @brief  put data into fifo
  * @param  ulBase is the base address of the I2C module.
  * @param  data is to be put into fifo
  * @retval None
  */
void I2C_PutData(I2C_TypeDef* I2Cx, uint16_t data)
{
    while((I2C_Status(I2Cx) & I2C_STAT_TFNF_Msk) != I2C_STAT_TFNF_Msk);

    I2Cx->CMD_DATA = data;
}
/**
  * @brief  get one byte data from fifo
  * @param  ulBase is the base address of the I2C module.
  * @retval one byte data from fifo
  */
uint8_t I2C_GetData(I2C_TypeDef* I2Cx)
{
    while ((I2C_Status(I2Cx) & I2C_STAT_RFNE_Msk) != I2C_STAT_RFNE_Msk);

    return I2Cx->CMD_DATA;
}

uint32_t I2C_GetDataNonBlocking(I2C_TypeDef* I2Cx)
{
    if((I2C_Status(I2Cx) & I2C_STAT_RFNE_Msk))
    {
    	return I2Cx->CMD_DATA;
    }
    else
    {
    	return -1;
    }

}


/**
  * @brief  write data in Master mode
  * @param  ulBase is the base address of the I2C module.
  * @param  pdata is the data buffer point
  * @param  len is the data length
  * @retval data length
  */
uint32_t I2C_MasterWriteData(I2C_TypeDef* I2Cx, uint8_t *pdata, uint32_t len)
{
    uint32_t i;
    uint16_t status;

    ASSERT(pdata != NULL && len > 0);

    for (i=0; i<len-1; i++)
    {
        I2C_PutData(I2Cx, pdata[i]);  //0x0000 | pdata[i]
    }

    I2C_PutData(I2Cx, 0x0200 | pdata[len-1]);    //bit[9]  stop signal + bit[8] write

    status = I2C_Status(I2Cx);

    //wait tx fifo empty
    while((status & I2C_STAT_TFE_Msk) != I2C_STAT_TFE_Msk) {
        status = I2C_Status(I2Cx);
    }

    return len;
}

/**
  * @brief  write data in Slave mode
  * @param  ulBase is the base address of the I2C module.
  * @param  pdata is the data buffer point
  * @param  len is the data length
  * @retval data length
  */
uint32_t I2C_SlaveWriteData(I2C_TypeDef* I2Cx, uint8_t *pdata, uint32_t len)
{
    uint32_t i;

    ASSERT(pdata != NULL && len > 0);

    for (i=0; i<len; i++)
    {
        I2C_PutData(I2Cx, pdata[i]);  //0x0000 | pdata[i]
    }

    return len;
}

/**
  * @brief  read data in Master mode
  * @param  ulBase is the base address of the I2C module.
  * @param  pdata is the data buffer point
  * @param  len is the data length
  * @retval data length
  */
uint32_t I2C_MasterReadData(I2C_TypeDef* I2Cx, uint8_t *pdata, uint32_t len)
{
    uint32_t i;

    ASSERT(pdata != NULL && len > 0);

    for (i=0; i<len-1; i++) {

        I2C_PutData(I2Cx, 0x0100);    //bit[9] read + 8bits dummy data

        pdata[i] = I2C_GetData(I2Cx);
    }

    I2C_PutData(I2Cx, 0x0300);        //bit[9]  stop signal + bit[8] read + 8bits dummy data

    pdata[len-1] = I2C_GetData(I2Cx);

    return len;
}

/**
  * @brief  read data in Slave mode
  * @param  ulBase is the base address of the I2C module.
  * @param  pdata is the data buffer point
  * @param  len is the data length
  * @retval data length
  */
uint32_t I2C_SlaveReadData(I2C_TypeDef* I2Cx, uint8_t *pdata, uint32_t len)
{
    uint32_t i;

    ASSERT(pdata != NULL && len > 0);

    for (i=0; i<len; i++) {
        pdata[i] = I2C_GetData(I2Cx);
    }

    return len;
}

#endif	//new



#if 0	//old
#include "i2c.h"

/**
  * @brief  init the I2C module.
  * @param  ulBase is the base address of the I2C module.
  * @param  workmode is the I2C work mode as following:
  *     @arg I2C_slave
  *     @arg I2C_master_normal_noStartByte
  *     @arg I2C_master_normal_withStartByte
  *     @arg I2C_master_hs_noStartByte
  *     @arg I2C_master_hs_withStartByte
  *     @arg I2C_master_generalCallAddress
  * @param  clockmode is the I2C clock mode as following:
  *     @arg I2C_standard_mode
  *     @arg I2C_fast_mode
  *     @arg I2C_fast_mode_pluse
  *     @arg I2C_highspeed_mode
  * @param  addrbits is the I2C address bits as following:
  *     @arg I2C_addr_7b
  *     @arg I2C_addr_10b
  * @param  pclk is the I2C peripheral clock frequency.
  * @retval None
  */
void I2CInit(unsigned long ulBase,
                I2C_workmode workmode,
                I2C_clockmode clockmode,
                I2C_addrbits addrbits,
                unsigned long pclk)
{
    unsigned short fs_scl_hcnt = 0, fs_scl_lcnt = 0;
    unsigned long baudrate = 0;
    unsigned char spklen = 0;
    unsigned short pwmcnt = 0;

    ASSERT((ulBase == I2C1_BASE) || (ulBase == I2C2_BASE));

    HWREG(ulBase + I2C_CTL) = (HWREG(ulBase + I2C_CTL) & (~I2C_CTL_WORK_MODE_Msk)) | workmode;

    HWREG(ulBase + I2C_CTL) |= (I2C_CTL_SLV_RXF_CS_EN | I2C_CTL_SLV_TXE_CS_EN | I2C_CTL_MST_CS_EN | I2C_CTL_D_BYTE_EN);

    if (addrbits == I2C_addr_10b) {
        HWREG(ulBase + I2C_CTL) |= I2C_CTL_MST_ADD_10B_Msk;   //10bits addr
    } else {
        HWREG(ulBase + I2C_CTL) &= ~I2C_CTL_MST_ADD_10B_Msk;  //7bits addr
    }

    //set the duration of spike suppression logic
    HWREGB(ulBase + I2C_HS_SPKLEN) = 0x00;
    HWREGB(ulBase + I2C_FS_SPKLEN) = 0x00;

    HWREGB(ulBase + I2C_TX_FIFO_THD) = 0x08;
    HWREGB(ulBase + I2C_RX_FIFO_THD) = 0x18;

    if (clockmode == I2C_standard_mode) {
        baudrate = 100000;
    } else if (clockmode == I2C_fast_mode) {
        baudrate = 400000;
    }else if (clockmode == I2C_fast_mode_pluse) {
        baudrate = 1000000;
    }else if (clockmode == I2C_highspeed_mode) {
        baudrate = 1000000; //3400000;
    }

    pwmcnt = (unsigned short)((pclk/baudrate)>>1);

    spklen = HWREGB(ulBase + I2C_FS_SPKLEN);
    fs_scl_hcnt = (pwmcnt - 4 - spklen >= 0)? (pwmcnt - 4 - spklen) : 0x14;
    fs_scl_lcnt = pwmcnt - 1;

    //set scl HCNT, LCNT
    HWREGH(ulBase + I2C_FS_SCL_HCNT) = fs_scl_hcnt;
    HWREGH(ulBase + I2C_FS_SCL_LCNT) = fs_scl_lcnt;

    HWREGH(ulBase + I2C_HS_SCL_HCNT) = (unsigned short)((pclk/3400000)>>1);
    HWREGH(ulBase + I2C_HS_SCL_LCNT) = (unsigned short)((pclk/3400000)>>1);

    //set sda holdtime
    HWREGB(ulBase + I2C_HS_SDA_HOLD) = 0x01;
    HWREGB(ulBase + I2C_FS_SDA_HOLD) = 0x08;  //0x1A;  //0x20;

    HWREGB(ulBase + I2C_HS_MC) = 0x07;           //hs_mc
    HWREGB(ulBase + I2C_BUS_IDLE_CNT) = 0x03;    //bus_idle_cnt

    HWREGH(ulBase + I2C_TIMEOUT) = 0xFFFF;

    HWREG(ulBase + I2C_D_SP_SR) = 0x05050505;

    HWREGB(ulBase + I2C_D_BUS_FREE) = 0x0F;

    HWREGB(ulBase + I2C_D_BYTE) = 0x30;
}

/**
  * @brief  reset the I2C module.
  * @param  ulBase is the base address of the I2C module.
  * @retval None
  */
void I2CReset(unsigned long ulBase)
{
    HWREG(ulBase + I2C_CTL) |= I2C_CTL_RSTN;
    HWREG(ulBase + I2C_CTL) &= ~I2C_CTL_RSTN;
}

/**
  * @brief  deinit the I2C module.
  * @param  ulBase is the base address of the I2C module.
  * @retval None
  */
void I2CDeInit(unsigned long ulBase)
{
    I2CReset(ulBase);                           //clear fifo and reset FSM.
    HWREG(ulBase + I2C_CTL) = 0;                //reset I2C control
    HWREG(ulBase + I2C_INT_STAT) = 0xFFFFFFFF;  //clear all interrupt status
}

/**
  * @brief  set the I2C FiFo threshold.
  * @param  ulBase is the base address of the I2C module.
  * @param  TxFiFoThreshold is the I2C TX FiFo threshold
  * @param  RxFiFoThreshold is the I2C RX FiFo threshold
  * @retval None
  */
void I2CSetFiFoThreshold(unsigned long ulBase, unsigned char TxFiFoThreshold, unsigned char RxFiFoThreshold)
{
    ASSERT((TxFiFoThreshold <= 32) && (RxFiFoThreshold <= 32));

    HWREGB(ulBase + I2C_TX_FIFO_THD) = TxFiFoThreshold;
    HWREGB(ulBase + I2C_RX_FIFO_THD) = RxFiFoThreshold;
}

/**
  * @brief  set the I2C device address.
  * @param  ulBase is the base address of the I2C module.
  * @param  addr is the I2C device address.
  * @retval None
  */
void I2CSetAddr(unsigned long ulBase, unsigned short addr)
{
    ASSERT((ulBase == I2C1_BASE) || (ulBase == I2C2_BASE));

    HWREGH(ulBase + I2C_ADDR) = addr;
}

/**
  * @brief  enable the master clock stretch or not.
  * @param  ulBase is the base address of the I2C module.
  * @param  enable is 1:enable, 0:disable.
  * @retval None
  */
void I2CSetMasterClockStretch(unsigned long ulBase, unsigned char enable)
{
    if (enable) {
        HWREG(ulBase + I2C_CTL) |= I2C_CTL_MST_CS_EN;
    } else {
        HWREG(ulBase + I2C_CTL) &= ~I2C_CTL_MST_CS_EN;
    }
}

/**
  * @brief  set the I2C delay between two bytes.
  * @param  ulBase is the base address of the I2C module.
  * @param  enable is 1:enable, 0:disable.
  * @param  delayBetweenByte is the delay between two bytes.
  * @retval None
  */
void I2CSetDelayBetweenByte(unsigned long ulBase, unsigned char enable, unsigned char delayBetweenByte)
{
    if (enable) {
        HWREG(ulBase + I2C_CTL) |= I2C_CTL_D_BYTE_EN;
    } else {
        HWREG(ulBase + I2C_CTL) &= ~I2C_CTL_D_BYTE_EN;
    }

    HWREGB(ulBase + I2C_D_BYTE) = delayBetweenByte;
}

/**
  * @brief  enable the I2C interrupt.
  * @param  ulBase is the base address of the I2C module.
  * @param ulIntFlags: the bit mask of the interrupt sources to be enable
  *   This parameter can be any combination of the following values:
  *     @arg I2C_INT_COMP:          transfer complete
  *     @arg I2C_INT_RX_UNF:        RX fifo underflow
  *     @arg I2C_INT_RX_OVF:        RX fifo overflow
  *     @arg I2C_INT_TX_OVF:        TX fifo overflow
  *     @arg I2C_INT_ACT:           I2C activity from idle status
  *     @arg I2C_INT_GEN_CALL:      trigger only when a General Call address is received and it is acknowledged in slave mode.
  *     @arg I2C_INT_ARB_LOST:      arbitration lost
  *     @arg I2C_INT_TIMEOUT:       timeout
  *     @arg I2C_INT_RX_AF:         RX fifo almost full
  *     @arg I2C_INT_TX_AE:         TX fifo almost empty
  *     @arg I2C_INT_REQ_BUSY:      trigger when the slave is busy
  *     @arg I2C_INT_MST_ON_HOLD:   Indicates whether a master is holding the bus
  *     @arg I2C_INT_SLV_ON_HOLD:   Indicates whether a slave is holding the bus
  *     @arg I2C_INT_MST_NACK:      Indicates master receive NACK
  *     @arg I2C_INT_ALL
  * @retval None
  */
void I2CIntEnable(unsigned long ulBase, unsigned long ulIntFlags)
{
    HWREG(ulBase + I2C_INT_MASK) &= ~ulIntFlags;
}

/**
  * @brief  disable the I2C interrupt.
  * @param  ulBase is the base address of the I2C module.
  * @param ulIntFlags: the bit mask of the interrupt sources to be disable
  *   This parameter can be any combination of the following values:
  *     @arg I2C_INT_COMP:          transfer complete
  *     @arg I2C_INT_RX_UNF:        RX fifo underflow
  *     @arg I2C_INT_RX_OVF:        RX fifo overflow
  *     @arg I2C_INT_TX_OVF:        TX fifo overflow
  *     @arg I2C_INT_ACT:           I2C activity from idle status
  *     @arg I2C_INT_GEN_CALL:      trigger only when a General Call address is received and it is acknowledged in slave mode.
  *     @arg I2C_INT_ARB_LOST:      arbitration lost
  *     @arg I2C_INT_TIMEOUT:       timeout
  *     @arg I2C_INT_RX_AF:         RX fifo almost full
  *     @arg I2C_INT_TX_AE:         TX fifo almost empty
  *     @arg I2C_INT_REQ_BUSY:      trigger when the slave is busy
  *     @arg I2C_INT_MST_ON_HOLD:   Indicates whether a master is holding the bus
  *     @arg I2C_INT_SLV_ON_HOLD:   Indicates whether a slave is holding the bus
  *     @arg I2C_INT_MST_NACK:      Indicates master receive NACK
  *     @arg I2C_INT_ALL
  * @retval None
  */
void I2CIntDisable(unsigned long ulBase, unsigned long ulIntFlags)
{
    HWREG(ulBase + I2C_INT_MASK) |= ulIntFlags;
}

/**
  * @brief  clear the I2C interrupt.
  * @param  ulBase is the base address of the I2C module.
  * @param ulIntFlags: the bit mask of the interrupt sources to be cleared
  *   This parameter can be any combination of the following values:
  *     @arg I2C_INT_COMP:          transfer complete
  *     @arg I2C_INT_RX_UNF:        RX fifo underflow
  *     @arg I2C_INT_RX_OVF:        RX fifo overflow
  *     @arg I2C_INT_TX_OVF:        TX fifo overflow
  *     @arg I2C_INT_ACT:           I2C activity from idle status
  *     @arg I2C_INT_GEN_CALL:      trigger only when a General Call address is received and it is acknowledged in slave mode.
  *     @arg I2C_INT_ARB_LOST:      arbitration lost
  *     @arg I2C_INT_TIMEOUT:       timeout
  *     @arg I2C_INT_RX_AF:         RX fifo almost full
  *     @arg I2C_INT_TX_AE:         TX fifo almost empty
  *     @arg I2C_INT_REQ_BUSY:      trigger when the slave is busy
  *     @arg I2C_INT_MST_ON_HOLD:   Indicates whether a master is holding the bus
  *     @arg I2C_INT_SLV_ON_HOLD:   Indicates whether a slave is holding the bus
  *     @arg I2C_INT_MST_NACK:      Indicates master receive NACK
  *     @arg I2C_INT_ALL
  * @retval None
  */
void I2CIntClear(unsigned long ulBase, unsigned long ulIntFlags)
{
    HWREG(ulBase + I2C_INT_STAT) |= ulIntFlags;
}

/**
  * @brief  get the I2C interrupt status.
  * @param  ulBase is the base address of the I2C module.
  * @retval the I2C interrupt status.
  */
unsigned long I2CIntStatus(unsigned long ulBase)
{
    return HWREG(ulBase + I2C_INT_STAT);
}

/**
  * @brief  Registers an interrupt handler for I2C interrupt.
  * @param  g_pRAMVectors is the global interrupt vectors table.
  * @param  pfnHandler is a pointer to the function to be called when the I2C interrupt occurs.
  * @retval None
  */
void I2CIntRegister(unsigned long ulBase, unsigned long *g_pRAMVectors, void (*pfnHandler)(void))
{
    unsigned long ulInt;

    ulInt = (ulBase == I2C1_BASE) ? INT_I2C1 : INT_I2C2;

    IntRegister(ulInt, g_pRAMVectors, pfnHandler);
    IntEnable(ulInt);
}

/**
  * @brief  Unregisters an interrupt handler for the I2C interrupt.
  * @param  g_pRAMVectors is the global interrupt vectors table.
  * @retval None
  */
void I2CIntUnregister(unsigned long ulBase, unsigned long *g_pRAMVectors)
{
    unsigned long ulInt;

    ulInt = (ulBase == I2C1_BASE) ? INT_I2C1 : INT_I2C2;

    IntDisable(ulInt);
    IntUnregister(ulInt, g_pRAMVectors);
}

/**
  * @brief  get the I2C control status.
  * @param  ulBase is the base address of the I2C module.
  * @retval the I2C control status.
  */
unsigned short I2CStatus(unsigned long ulBase)
{
    return HWREGH(ulBase + I2C_STAT);
}

/**
  * @brief  get the I2C TX fifo level.
  * @param  ulBase is the base address of the I2C module.
  * @retval the I2C TX fifo level.
  */
unsigned char I2CTxFiFoLevel(unsigned long ulBase)
{
    return HWREGB(ulBase + I2C_TX_FIFO_LEVEL);
}

/**
  * @brief  get the I2C RX fifo level.
  * @param  ulBase is the base address of the I2C module.
  * @retval the I2C RX fifo level.
  */
unsigned char I2CRxFiFoLevel(unsigned long ulBase)
{
    return HWREGB(ulBase + I2C_RX_FIFO_LEVEL);
}

/**
  * @brief  put data into fifo
  * @param  ulBase is the base address of the I2C module.
  * @param  data is to be put into fifo
  * @retval None
  */
void I2CPutData(unsigned long ulBase, unsigned short data)
{
    while((I2CStatus(ulBase) & I2C_STAT_TFNF_Msk) != I2C_STAT_TFNF_Msk);

    HWREGH(ulBase + I2C_CMD_DATA) = data;
}

/**
  * @brief  get one byte data from fifo
  * @param  ulBase is the base address of the I2C module.
  * @retval one byte data from fifo
  */
unsigned char I2CGetData(unsigned long ulBase)
{
    while ((I2CStatus(ulBase) & I2C_STAT_RFNE_Msk) != I2C_STAT_RFNE_Msk);

    return HWREGB(ulBase + I2C_CMD_DATA);
}

/**
  * @brief  write data in Master mode
  * @param  ulBase is the base address of the I2C module.
  * @param  pdata is the data buffer point
  * @param  len is the data length
  * @retval data length
  */
unsigned long I2CMasterWriteData(unsigned long ulBase, unsigned char *pdata, unsigned long len)
{
    unsigned long i;
    unsigned short status;

    ASSERT(pdata != NULL && len > 0);

    for (i=0; i<len-1; i++)
    {
        I2CPutData(ulBase, pdata[i]);  //0x0000 | pdata[i]
    }

    I2CPutData(ulBase, 0x0200 | pdata[len-1]);    //bit[9]  stop signal + bit[8] write

    status = I2CStatus(ulBase);

    //wait tx fifo empty
    while((status & I2C_STAT_TFE_Msk) != I2C_STAT_TFE_Msk) {
        status = I2CStatus(ulBase);
    }

    return len;
}

/**
  * @brief  write data in Slave mode
  * @param  ulBase is the base address of the I2C module.
  * @param  pdata is the data buffer point
  * @param  len is the data length
  * @retval data length
  */
unsigned long I2CSlaveWriteData(unsigned long ulBase, unsigned char *pdata, unsigned long len)
{
    unsigned long i;

    ASSERT(pdata != NULL && len > 0);

    for (i=0; i<len; i++)
    {
        I2CPutData(ulBase, pdata[i]);  //0x0000 | pdata[i]
    }

    return len;
}

/**
  * @brief  read data in Master mode
  * @param  ulBase is the base address of the I2C module.
  * @param  pdata is the data buffer point
  * @param  len is the data length
  * @retval data length
  */
unsigned long I2CMasterReadData(unsigned long ulBase, unsigned char *pdata, unsigned long len)
{
    unsigned long i;

    ASSERT(pdata != NULL && len > 0);

    for (i=0; i<len-1; i++) {

        I2CPutData(ulBase, 0x0100);    //bit[9] read + 8bits dummy data

        pdata[i] = I2CGetData(ulBase);
    }

    I2CPutData(ulBase, 0x0300);        //bit[9]  stop signal + bit[8] read + 8bits dummy data

    pdata[len-1] = I2CGetData(ulBase);

    return len;
}

/**
  * @brief  read data in Slave mode
  * @param  ulBase is the base address of the I2C module.
  * @param  pdata is the data buffer point
  * @param  len is the data length
  * @retval data length
  */
unsigned long I2CSlaveReadData(unsigned long ulBase, unsigned char *pdata, unsigned long len)
{
    unsigned long i;

    ASSERT(pdata != NULL && len > 0);

    for (i=0; i<len; i++) {
        pdata[i] = I2CGetData(ulBase);
    }

    return len;
}

#endif	//old
