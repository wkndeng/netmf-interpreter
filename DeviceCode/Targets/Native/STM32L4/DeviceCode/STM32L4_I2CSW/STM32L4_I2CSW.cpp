////////////////////////////////////////////////////////////////////////////
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//  Portions Copyright (c) Kentaro Sekimoto All rights reserved.
//
////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "STM32L4_I2CSW.h"

//#define DEBUG_I2CSW
//#define DEBUG_I2C_TX_DATA
//#define DEBUG_I2C_RX_DATA

#define SCLPIN (GPIO_PIN)0x18
#define SDAPIN (GPIO_PIN)0x19

#define I2C_ACK 0
#define I2C_NACK    1

#define SCL_OUT CPU_GPIO_EnableOutputPin(SCLPIN, true)
#define SDA_IN  CPU_GPIO_EnableInputPin(SDAPIN, false, NULL, GPIO_INT_NONE, RESISTOR_DISABLED)
#define SDA_OUT CPU_GPIO_EnableOutputPin(SDAPIN, true)
#define SCL_DT  CPU_GPIO_GetPinState(SCLPIN)
#define SDA_DT  CPU_GPIO_GetPinState(SDAPIN)
#define SCL_LO  CPU_GPIO_SetPinState(SCLPIN, false)
#define SCL_HI  CPU_GPIO_SetPinState(SCLPIN, true)
#define SDA_LO  CPU_GPIO_SetPinState(SDAPIN, false)
#define SDA_HI  CPU_GPIO_SetPinState(SDAPIN, true)

//#define I2C_FAST

#ifdef I2C_FAST
// 100KHz on DRAM with H8
#ifdef H8
#define setup_delay()   {asm volatile ("nop\n\t" "nop\n\t" ::);}
#define scl_wait()      {asm volatile ("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" ::);}
#else
// 1MHz on internal flash with FM3
#define setup_delay()   {asm volatile ("nop\n\t" "nop\n\t" ::);}
#define scl_wait()      {asm volatile ("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" ::);}
#endif
#else
// 20KHz on DRAM
//#define SCL_HALF_COUNT  2
//#define SCL_COUNT   4
// 30KHz on DRAM
#define SCL_HALF_COUNT  30
#define SCL_COUNT   50

static void wait_by_cnt(volatile int cnt)
{
    while (cnt-- > 0) ;
}

static void setup_delay(void)
{
    wait_by_cnt(SCL_HALF_COUNT);
}

static void scl_wait(void)
{
    wait_by_cnt(SCL_COUNT);
}
#endif

void i2c_init(void)
{
    SCL_OUT;
    SDA_OUT;
    SCL_HI;
    SDA_HI;
}

// A high-to-low transition of SDA with SCL high is a start condition
void i2c_startbit(void)
{
    GLOBAL_LOCK(irq);
    setup_delay();
    if (!SCL_DT || !SDA_DT ) {
        if (SCL_DT ) {
            SCL_LO;
            setup_delay();
        }
        if (!SDA_DT ) {
            SDA_HI;
            setup_delay();
        }
        SCL_HI;
        setup_delay();
    }
    SDA_LO;
    setup_delay();
    SCL_LO;
    setup_delay();
}

// A low-to-high transition of SDA with SCL high is a stop condition. A
void i2c_stopbit(void)
{
    GLOBAL_LOCK(irq);
    SDA_LO;
    SCL_HI;
    setup_delay();
    SDA_HI;
    setup_delay();
}

int i2c_send_byte(unsigned char data)
{
    GLOBAL_LOCK(irq);
    volatile int ack;
    int i;
    for (i = 0; i < 8; i++) {
        if (data & 0x80) {
            SDA_HI;
        } else {
            SDA_LO;
        }
        setup_delay();
        SCL_HI;
        scl_wait();
        SCL_LO;
        data <<= 1;
    }
    setup_delay();
    SDA_IN;
    SCL_HI;
    setup_delay();
    ack = SDA_DT;
    SCL_LO;
    //SDA_OUT;
    return ack;
}

unsigned char i2c_recv_byte(int ack)
{
    GLOBAL_LOCK(irq);
    int i;
    unsigned char data = 0;
    SDA_IN;
    for (i = 0; i < 8; i++) {
        data <<= 1;
        SCL_HI;
        scl_wait();
        if (SDA_DT) {
            data |= 0x01;
        }
        SCL_LO;
        setup_delay();
    }
    SDA_OUT;
    if (ack == I2C_NACK) {
        SDA_HI;
    } else {
        SDA_LO;
    }
    setup_delay();
    SCL_HI;
    scl_wait();
    SCL_LO;
    SDA_LO;
    return data;
}

#if 0
int i2c_write(unsigned char addr, unsigned char* data, size_t size)
{
    if (i2c_send_byte((addr << 1) | 0x00) == I2C_NACK)
        return FALSE;
    while (size-- > 0)
        if (i2c_send_byte(*data++) == I2C_NACK)
            return FALSE;
    return TRUE;
}

int i2c_read(unsigned char addr, unsigned char* data, size_t size)
{
    if (i2c_send_byte((addr << 1) | 0x01) == I2C_NACK)
        return FALSE;
    while (size-- > 0)
        *data++ = i2c_recv_byte(size == 0? I2C_NACK : I2C_ACK);
    return TRUE;
}
#endif

STM32L4_I2CSW_Driver g_STM32L4_I2CSW_Driver;

BOOL STM32L4_I2CSW_Driver::Initialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    g_STM32L4_I2CSW_Driver.m_currentXAction = NULL;
    g_STM32L4_I2CSW_Driver.m_currentXActionUnit = NULL;
    i2c_init();
#if defined(DEBUG_I2CSW)
    debug_printf("I2CSW Init\r\n");
#endif
    return TRUE;
}

BOOL STM32L4_I2CSW_Driver::Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    g_STM32L4_I2CSW_Driver.m_currentXAction = NULL;
    g_STM32L4_I2CSW_Driver.m_currentXActionUnit = NULL;
#if defined(DEBUG_I2CSW)
    debug_printf("I2CSW Uninit\r\n");
#endif
    return TRUE;
}

void STM32L4_I2CSW_Driver::Abort(void)
{
    I2C_HAL_XACTION *xAction = g_STM32L4_I2CSW_Driver.m_currentXAction;
    i2c_stopbit();
    xAction->Signal(I2C_HAL_XACTION::c_Status_Aborted);
}

void STM32L4_I2CSW_Driver::WriteToSubordinate(I2C_HAL_XACTION_UNIT *unit)
{
    UINT8 *queueData;
    queueData = unit->m_dataQueue.Pop();
    //ASSERT(queueData);
    if (queueData == NULL) {
#if defined(DEBUG_I2CSW)
        debug_printf("I2C wque err\r\n");
#endif
        return;
    }
#ifdef DEBUG_I2C_TX_DATA
    debug_printf("w%02x", *queueData);
    //debug_printf("w");
#endif
    if (i2c_send_byte(*queueData) == I2C_NACK) {
#if defined(DEBUG_I2CSW)
        debug_printf("werr\r\n");
#endif
    }
    ++unit->m_bytesTransferred;
    --unit->m_bytesToTransfer;
}

void STM32L4_I2CSW_Driver::ReadFromSubordinate(I2C_HAL_XACTION_UNIT *unit)
{
    UINT8* queueData;
    queueData = unit->m_dataQueue.Push();
    //ASSERT(queueData);
    if (queueData == NULL) {
#if defined(DEBUG_I2CSW)
        debug_printf("I2C rque err\r\n");
#endif
        return;
    }
    UINT8 data = i2c_recv_byte(unit->m_bytesToTransfer == 1? I2C_NACK : I2C_ACK);
#ifdef DEBUG_I2C_RX_DATA
    debug_printf("r%02x", data);
    //debug_printf("r");
#endif
    *queueData = data;
    ++unit->m_bytesTransferred;
    --unit->m_bytesToTransfer;
}

void STM32L4_I2CSW_Driver::MasterXAction_Start(I2C_HAL_XACTION* xAction, bool repeatedStart)
{
    UINT8 address;
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    g_STM32L4_I2CSW_Driver.m_currentXAction = xAction;
    g_STM32L4_I2CSW_Driver.m_currentXActionUnit = xAction->m_xActionUnits[xAction->m_current++];
    I2C_HAL_XACTION_UNIT *unit = g_STM32L4_I2CSW_Driver.m_currentXActionUnit;
#ifdef DEBUG_I2CSW
    debug_printf("I2CSW Start\r\n");
#endif
    if (!repeatedStart) {
        i2c_startbit();
    } else {
        i2c_startbit();
    }
    address  = 0xFE & (xAction->m_address << 1);
    address |= unit->IsReadXActionUnit() ? STM32L4_I2CSW_Driver::c_DirectionRead : STM32L4_I2CSW_Driver::c_DirectionWrite;
    if (i2c_send_byte(address) == I2C_NACK) {
        Abort();
        return;
    }
    if (!unit->IsReadXActionUnit()) {
        while (unit->m_bytesToTransfer > 0)
            WriteToSubordinate(unit);
    } else {
        while (unit->m_bytesToTransfer > 0)
            ReadFromSubordinate(unit);
    }
    if (xAction->ProcessingLastUnit()) {
        i2c_stopbit();
        xAction->Signal(I2C_HAL_XACTION::c_Status_Completed );
    } else {
#if (IIC_READ_WRITE_DELAY == 1)
    HAL_Time_Sleep_MicroSeconds_InterruptEnabled(500);
#endif
        MasterXAction_Start(xAction, true);
    }
}

void STM32L4_I2CSW_Driver::MasterXAction_Stop()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
#ifdef DEBUG_I2CSW
    debug_printf("I2CSW Stop\r\n");
#endif
}

void STM32L4_I2CSW_Driver::GetClockRate(UINT32 rateKhz, UINT8& clockRate, UINT8& clockRate2)
{
    // ToDo
}

void STM32L4_I2CSW_Driver::GetPins(GPIO_PIN& scl, GPIO_PIN& sda)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    scl = SCLPIN;
    sda = SDAPIN;
}
