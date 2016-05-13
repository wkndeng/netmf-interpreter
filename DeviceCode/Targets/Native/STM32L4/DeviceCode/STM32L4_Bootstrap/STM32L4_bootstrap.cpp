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
//  *** Bootstrap ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>

#define STM32L476xx
#include "stm32l4xx.h"

///////////////////////////////////////////////////////////////////////////////

#pragma arm section code = "SectionForBootstrapOperations"


/* IO initialization implemented in solution DeviceCode\Init */
void BootstrapCode_GPIO();

extern "C"
{

void HAL_Init(void);
void SystemClock_Config(void);
void HAL_SuspendTick(void);

void __section("SectionForBootstrapOperations") STM32L4_BootstrapCode()
{
    // FPU
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
    // RCC Clock
    //RCC->CR |= RCC_CR_MSION;
    // Reset CFGR register
    //RCC->CFGR = 0x00000000;
    // Reset HSEON, CSSON , HSION, and PLLON bits
    //RCC->CR &= (uint32_t)0xEAF6FFFF;
    // Reset PLLCFGR register
    //RCC->PLLCFGR = 0x00000800;
    // Reset HSEBYP bit
    //RCC->CR &= (uint32_t)0xFFFBFFFF;
    // Disable all interrupts
    //RCC->CIER = 0x00000000;
    // Configure the Vector Table location add offset address
//#define FLASH_BASE            ((uint32_t)0x08000000) /*!< FLASH(up to 1 MB) base address */
//#define SRAM1_BASE            ((uint32_t)0x20000000) /*!< SRAM1(96 KB) base address*/
#define VECT_TAB_OFFSET  0x00
    #ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
  #else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
  #endif
    // Configure the Cube driver
    //SystemCoreClock = MSI_VALUE; // At this stage the MSI is used as system clock
    HAL_Init();
    SystemClock_Config();
    HAL_SuspendTick();

    // Reset the timer to avoid issues after the RAM initialization
    //TIM_MST_RESET_ON;
    //TIM_MST_RESET_OFF;
}

__section("SectionForBootstrapOperations") void BootstrapCode()
{
    STM32L4_BootstrapCode();
    
    PrepareImageRegions();
    
    BootstrapCode_GPIO();

}

__section("SectionForBootstrapOperations") void BootstrapCodeMinimal()
{
    STM32L4_BootstrapCode();
    
    BootstrapCode_GPIO();
}

}

