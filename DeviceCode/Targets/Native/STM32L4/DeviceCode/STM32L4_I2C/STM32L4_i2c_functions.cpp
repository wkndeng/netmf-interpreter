////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for STM32L4: Copyright (c) Oberon microsystems, Inc.
//  Modification for the NUCLEOL476RG board (STM32L4): Copyright (c) Kentaro Sekimoto
//
//  *** I2C Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\stm32l4xx.h"


#if STM32L4_I2C_PORT == 2
    #define I2Cx                   I2C2 
    #define I2Cx_EV_IRQn           I2C2_EV_IRQn
    #define I2Cx_ER_IRQn           I2C2_ER_IRQn
    #define RCC_APB1ENR1_I2CxEN     RCC_APB1ENR1_I2C2EN
    #define RCC_APB1RSTR1_I2CxRST   RCC_APB1RSTR1_I2C2RST
	#define RCC_CCIPR_I2CxSEL		RCC_CCIPR_I2C2SEL_1		// 0x00002000 (HSI16)
	#define RCC_CCIPR_I2CxMASK		RCC_CCIPR_I2C2SEL		// 0x00003000 (Reserved)
    #if !defined(STM32L4_I2C_SCL_PIN)
        #define STM32L4_I2C_SCL_PIN  32	// PC0
    #endif
    #if !defined(STM32L4_I2C_SDA_PIN)
        #define STM32L4_I2C_SDA_PIN  33	// PC1
    #endif
#elif STM32L4_I2C_PORT == 3
    #define I2Cx                   I2C3 
    #define I2Cx_EV_IRQn           I2C3_EV_IRQn
    #define I2Cx_ER_IRQn           I2C3_ER_IRQn
    #define RCC_APB1ENR1_I2CxEN     RCC_APB1ENR1_I2C3EN
    #define RCC_APB1RSTR1_I2CxRST   RCC_APB1RSTR1_I2C3RST
	#define RCC_CCIPR_I2CxSEL		RCC_CCIPR_I2C3SEL_1		// 0x00002000 (HSI16)
	#define RCC_CCIPR_I2CxMASK		RCC_CCIPR_I2C3SEL		// 0x00003000 (Reserved)
    #if !defined(STM32L4_I2C_SCL_PIN)
        #define STM32L4_I2C_SCL_PIN  8	// PA8
    #endif
    #if !defined(STM32L4_I2C_SDA_PIN)
        #define STM32L4_I2C_SDA_PIN  41	// PC9
    #endif
#else // use I2C1 by default
    #define I2Cx                   I2C1 
    #define I2Cx_EV_IRQn           I2C1_EV_IRQn
    #define I2Cx_ER_IRQn           I2C1_ER_IRQn
    #define RCC_APB1ENR1_I2CxEN     RCC_APB1ENR1_I2C1EN
    #define RCC_APB1RSTR1_I2CxRST   RCC_APB1RSTR1_I2C1RST
    //#define RCC_CCIPR_I2CxSEL       RCC_CCIPR_I2C1SEL_1     // 0x00002000 (HSI16)
    #define RCC_CCIPR_I2CxSEL       RCC_CCIPR_I2C1SEL_0     // 0x00001000 (System Clock)
	#define RCC_CCIPR_I2CxMASK		RCC_CCIPR_I2C1SEL		// 0x00003000 (Reserved)
    #if !defined(STM32L4_I2C_SCL_PIN)
        #define STM32L4_I2C_SCL_PIN  24 // PB8
    #endif
    #if !defined(STM32L4_I2C_SDA_PIN)
        #define STM32L4_I2C_SDA_PIN  25 // PB9
    #endif
#endif

#define I2C_RELOAD_MODE             I2C_CR2_RELOAD
#define I2C_AUTOEND_MODE            I2C_CR2_AUTOEND
#define I2C_SOFTEND_MODE            ((UINT32)0x00000000)
#define I2C_NO_STARTSTOP            ((UINT32)0x00000000)
#define I2C_GENERATE_STOP           I2C_CR2_STOP
#define I2C_GENERATE_START_READ     (UINT32)(I2C_CR2_START | I2C_CR2_RD_WRN)
#define I2C_GENERATE_START_WRITE    I2C_CR2_START
#define I2C_INT_FLAGS	            0x000000fe
#define CR2_MASK    (UINT32)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP)

static I2C_HAL_XACTION* currentI2CXAction;
static I2C_HAL_XACTION_UNIT* currentI2CUnit;


void STM32L4_I2C_ER_Interrupt (void* param) // Error Interrupt Handler
{
    INTERRUPT_START
    
    I2Cx->ISR = 0; // reset errors
    I2C_HAL_XACTION* xAction = currentI2CXAction;
    xAction->Signal(I2C_HAL_XACTION::c_Status_Aborted); // calls XActionStop()
    
    INTERRUPT_END
}
    
void STM32L4_I2C_EV_Interrupt (void* param) // Event Interrupt Handler
{
    INTERRUPT_START
    
    I2C_HAL_XACTION* xAction = currentI2CXAction;
    I2C_HAL_XACTION_UNIT* unit = currentI2CUnit;
    
    UINT32 todo = unit->m_bytesToTransfer;
    UINT32 ISR = I2Cx->ISR;  // read status register
    //UINT32 cr1 = I2Cx->CR1;  // control register1
    //UINT32 cr2 = I2Cx->CR2;	 // control register2
    
    if (unit->IsReadXActionUnit()) { // read transaction
        if (ISR & I2C_ISR_RXNE) {
        	UINT8 data = I2Cx->RXDR; // read data
            *(unit->m_dataQueue.Push()) = data; // save data
            unit->m_bytesTransferred++;
            unit->m_bytesToTransfer = --todo; // update todo
            ISR = I2Cx->ISR;  // update status register copy
        }
    } else { // write transaction
        if (ISR & I2C_ISR_TXIS) {	// write data
        	I2Cx->TXDR = (UINT8)*(unit->m_dataQueue.Pop()); // next data byte;
            unit->m_bytesTransferred++;
            unit->m_bytesToTransfer = --todo; // update todo
        } else if (ISR & I2C_ISR_NACKF) {
        	xAction->Signal(I2C_HAL_XACTION::c_Status_Aborted);
        }
    }
    if (todo == 0) {
        if (!xAction->ProcessingLastUnit()) {
            currentI2CUnit = xAction->m_xActionUnits[ xAction->m_current++ ];
            if (currentI2CUnit->IsReadXActionUnit()) {
                I2Cx->CR2 = (I2Cx->CR2 & ~(CR2_MASK)) |
                    ((UINT32)(xAction->m_address) << 1) |
                    ((UINT32)(currentI2CUnit->m_bytesToTransfer) << 16) |
                    (UINT32)I2C_SOFTEND_MODE |
//                    ((UINT32)I2C_RELOAD_MODE) |
                    (UINT32)I2C_GENERATE_START_READ;
            } else {
                I2Cx->CR2 = (I2Cx->CR2 & ~(CR2_MASK)) |
                    ((UINT32)(xAction->m_address) << 1) |
                    ((UINT32)(currentI2CUnit->m_bytesToTransfer) << 16) |
                    (UINT32)I2C_SOFTEND_MODE |
                    ((UINT32)I2C_RELOAD_MODE) |
                    (UINT32)I2C_GENERATE_START_WRITE;
            }
        } else {
            xAction->Signal(I2C_HAL_XACTION::c_Status_Completed); // calls XActionStop()
        }
    }
    INTERRUPT_END
}


BOOL I2C_Internal_Initialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    
    if (!(RCC->APB1ENR1 & RCC_APB1ENR1_I2CxEN)) { // only once
        currentI2CXAction = NULL;
        currentI2CUnit = NULL;
        // set pins to AF4 and open drain
        CPU_GPIO_DisablePin( STM32L4_I2C_SDA_PIN, RESISTOR_DISABLED, 0, (GPIO_ALT_MODE)0x343 );
        CPU_GPIO_DisablePin( STM32L4_I2C_SCL_PIN, RESISTOR_DISABLED, 0, (GPIO_ALT_MODE)0x343 );
        
        RCC->CCIPR = (RCC->CCIPR & ~RCC_CCIPR_I2CxMASK) | RCC_CCIPR_I2CxSEL;
        RCC->APB1ENR1 |= RCC_APB1ENR1_I2CxEN;       // enable I2C clock
        RCC->APB1RSTR1 = RCC_APB1RSTR1_I2CxRST;     // reset I2C peripheral
        RCC->APB1RSTR1 = 0;                         // release I2C peripheral

        I2Cx->CR1 &= ~I2C_CR1_PE;
        I2Cx->TIMINGR = 0x20602938; // value from MBED library
        I2Cx->OAR1 &= ~I2C_OAR1_OA1EN;  // Disable OAR1
        I2Cx->OAR2 &= ~I2C_OAR2_OA2EN;  // Disable OAR2
        I2Cx->CR2 = (I2C_CR2_AUTOEND | I2C_CR2_NACK);
        I2Cx->CR1 |= I2C_CR1_PE; // enable peripheral
        
        CPU_INTC_ActivateInterrupt(I2Cx_EV_IRQn, STM32L4_I2C_EV_Interrupt, 0);
        CPU_INTC_ActivateInterrupt(I2Cx_ER_IRQn, STM32L4_I2C_ER_Interrupt, 0);
    }
    return TRUE;
}

BOOL I2C_Internal_Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    
    CPU_INTC_DeactivateInterrupt(I2Cx_EV_IRQn);
    CPU_INTC_DeactivateInterrupt(I2Cx_ER_IRQn);
    I2Cx->CR1 = 0;	// disable peripheral
    RCC->APB1ENR1 &= ~RCC_APB1ENR1_I2CxEN; // disable I2C clock
    return TRUE;
}

void I2C_Internal_XActionStart( I2C_HAL_XACTION* xAction, bool repeatedStart )
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();

    currentI2CXAction = xAction;
    currentI2CUnit = xAction->m_xActionUnits[ xAction->m_current++ ];

    I2Cx->CR1 &= ~I2C_CR1_PE;
    I2Cx->ISR = 0; // reset error flags
    UINT32 khz = xAction->m_clockRate + (xAction->m_clockRate2 << 8);
    // PRESC[31:28]
    // SCLDEL[23:21]
    // SDADEL[20:17]
    // SCLH[16:8]
    // SCLL[7:0:]
#if 0
    if (khz <= 10) {
        I2Cx->TIMINGR = 0x3024c3c7;
    } else if (khz <= 100) {
        I2Cx->TIMINGR = 0x30240f13;
    } else if (khz <= 400) {
        I2Cx->TIMINGR = 0x10230309;
    } else {
        I2Cx->TIMINGR = 0x30240f13;
    }
#else
    I2Cx->TIMINGR = 0x20602938;
#endif
    I2Cx->CR1 = I2C_INT_FLAGS + I2C_CR1_PE;     // enable and reset special flag
    if (currentI2CUnit->IsReadXActionUnit()) {
        I2Cx->CR2 = (I2Cx->CR2 & ~(CR2_MASK)) |
            ((UINT32)(xAction->m_address) << 1) |
            ((UINT32)(currentI2CUnit->m_bytesToTransfer) << 16) |
            (UINT32)I2C_SOFTEND_MODE |
//            ((UINT32)I2C_RELOAD_MODE) |
            (UINT32)I2C_GENERATE_START_READ;
    } else {
        I2Cx->CR2 = (I2Cx->CR2 & ~(CR2_MASK)) |
            ((UINT32)(xAction->m_address) << 1) |
            ((UINT32)(currentI2CUnit->m_bytesToTransfer) << 16) |
            (UINT32)I2C_SOFTEND_MODE |
//            ((UINT32)I2C_RELOAD_MODE) |
            (UINT32)I2C_GENERATE_START_WRITE;
    }
}

void I2C_Internal_XActionStop()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();

    if (I2Cx->ISR & I2C_ISR_BUSY && !(I2Cx->CR2 & I2C_CR2_STOP)) {
        I2Cx->CR2 |= I2C_CR2_STOP; // send stop
    }
    I2Cx->CR1 &= ~(I2C_INT_FLAGS + I2C_CR1_PE);
    currentI2CXAction = NULL;
    currentI2CUnit = NULL;
}

void I2C_Internal_GetClockRate( UINT32 rateKhz, UINT8& clockRate, UINT8& clockRate2)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    clockRate = (UINT8)rateKhz; // low byte
    clockRate2 = (UINT8)(rateKhz >> 8); // high byte
}

void I2C_Internal_GetPins(GPIO_PIN& scl, GPIO_PIN& sda)
{
    scl = STM32L4_I2C_SCL_PIN;
    sda = STM32L4_I2C_SDA_PIN;
}
