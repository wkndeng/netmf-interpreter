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
//  *** CPU Power States ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _STM32L4_POWER_FUNCTIONS_H_1
#define _STM32L4_POWER_FUNCTIONS_H_1 1


void STM32L4_SetPowerHandlers(void (*stop)(), void (*restart)());


#endif

