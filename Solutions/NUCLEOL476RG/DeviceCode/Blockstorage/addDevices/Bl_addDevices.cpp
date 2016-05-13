////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for the MCBSTM32L400 board (STM32L4): Copyright (c) Oberon microsystems, Inc.
//  Modification for the NUCLEOL476RG board (STM32L4): Copyright (c) Kentaro Sekimoto
//
//  *** NUCLEOL476RG Block Storage AddDevice Configuration ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>


extern struct BlockStorageDevice  g_STM32L4_BS;
extern struct IBlockStorageDevice g_STM32L4_Flash_DeviceTable;
extern struct BLOCK_CONFIG        g_STM32L4_BS_Config;



void BlockStorage_AddDevices()
{
	BlockStorageList::AddDevice( &g_STM32L4_BS, &g_STM32L4_Flash_DeviceTable, &g_STM32L4_BS_Config, FALSE );
}

