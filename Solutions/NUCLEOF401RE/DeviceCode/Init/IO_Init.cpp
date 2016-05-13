////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for the  board (STM32F4): Copyright (c) Oberon microsystems, Inc.
//
//  ***  Board Specific IO Port Initialization ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\..\..\..\DeviceCode\Targets\Native\STM32F4\DeviceCode\stm32f4xx.h"

// Define the generic port table, only one generic extensionn port type supported
// and that is the ITM hardware trace port on Channel 0.
extern GenericPortTableEntry const Itm0GenericPort;
extern GenericPortTableEntry const* const g_GenericPorts[TOTAL_GENERIC_PORTS] = { &Itm0GenericPort };

extern void STM32F4_GPIO_Pin_Config( GPIO_PIN pin, UINT32 mode, GPIO_RESISTOR resistor, UINT32 alternate );    // Workaround, since CPU_GPIO_DisablePin() does not correctly initialize pin speeds

void __section("SectionForBootstrapOperations") BootstrapCode_GPIO() {

    /* Enable GPIO clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN
                  | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN
                  | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOHEN | RCC_AHB1ENR_GPIOIEN;
#if 0
    //CPU_GPIO_EnableOutputPin(0 * 16 + 5, TRUE); // PA5 = LD2
#else
    //CPU_GPIO_EnableOutputPin(0 * 16 + 5, TRUE); // PA5 = LD2
#endif
}
