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
//  *** NUCLEOL476RG Block Storage Configuration ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>

#define FLASH_MANUFACTURER_CODE              0x0000  // not used
#define FLASH_DEVICE_CODE                    0x0000  // not used
#define FLASH_BASE_ADDRESS1                  0x08000000
#define FLASH_BLOCK_COUNT1                   512
#define FLASH_BYTES_PER_BLOCK1               0x00000800
#define FLASH_BYTES_PER_SECTOR               0x800
#define FLASH_BLOCK_ERASE_TYPICAL_TIME_USEC  1000000 // not used
#define FLASH_SECTOR_WRITE_TYPICAL_TIME_USEC 10      // not used
#define FLASH_BLOCK_ERASE_MAX_TIME_USEC      4000000 // not used
#define FLASH_SECTOR_WRITE_MAX_TIME_USEC     100     // not used
#define FLASH_BLOCK_ERASE_ACTUAL_TIME_USEC   10000   // not used

// EBIU Information

#define STM32L4__CHIP_SELECT       0
#define STM32L4__WAIT_STATES       5 // not used
#define STM32L4__RELEASE_COUNTS    0
#define STM32L4__BIT_WIDTH         64
#define STM32L4__BASE_ADDRESS      0x08000000
#define STM32L4__SIZE_IN_BYTES     0x00100000
#define STM32L4__WP_GPIO_PIN       GPIO_PIN_NONE
#define STM32L4__WP_ACTIVE         FALSE

// BlockDeviceInformation

#define STM32L4__IS_REMOVABLE      FALSE
#define STM32L4__SUPPORTS_XIP      TRUE
#define STM32L4__WRITE_PROTECTED   FALSE
#define STM32L4__SUPP_COPY_BACK    FALSE
#define STM32L4__NUM_REGIONS       1

const BlockRange g_STM32L4_BlockRange1[] =
{
    { BlockRange::BLOCKTYPE_BOOTSTRAP ,    0, 31 },	// 08000000 bootloader   64k
    { BlockRange::BLOCKTYPE_CODE      ,   32,383 }, // 08010000 CLR         704k
    { BlockRange::BLOCKTYPE_DEPLOYMENT,  384,503 }, // 080C0000 deployment  240k
    { BlockRange::BLOCKTYPE_CONFIG    ,  504,505 }, // 080FC000 config        4k
                                                    // 080FD000-080FFFFF     12K workaround of mbed drive copy issue on Windows 10
};

const BlockRegionInfo  g_STM32L4_BlkRegion[STM32L4__NUM_REGIONS] =
{
    {
        FLASH_BASE_ADDRESS1,    // ByteAddress   Start;           // Starting Sector address
        FLASH_BLOCK_COUNT1,     // UINT32        NumBlocks;       // total number of blocks in this region
        FLASH_BYTES_PER_BLOCK1, // UINT32        BytesPerBlock;   // Total number of bytes per block
        ARRAYSIZE_CONST_EXPR(g_STM32L4_BlockRange1),
        g_STM32L4_BlockRange1,
    }
};

const BlockDeviceInfo g_STM32L4_DeviceInfo=
{
    {  
        STM32L4__IS_REMOVABLE,             // BOOL Removable;
        STM32L4__SUPPORTS_XIP,             // BOOL SupportsXIP;
        STM32L4__WRITE_PROTECTED,          // BOOL WriteProtected;
        STM32L4__SUPP_COPY_BACK            // BOOL SupportsCopyBack
    },
    FLASH_SECTOR_WRITE_TYPICAL_TIME_USEC,  // UINT32 MaxSectorWrite_uSec;
    FLASH_BLOCK_ERASE_ACTUAL_TIME_USEC,    // UINT32 MaxBlockErase_uSec;
    FLASH_BYTES_PER_SECTOR,                // UINT32 BytesPerSector;     

    FLASH_MEMORY_Size,                     // UINT32 Size;

    STM32L4__NUM_REGIONS,                  // UINT32 NumRegions;
    g_STM32L4_BlkRegion,                   // const BlockRegionInfo* pRegions;
};

struct MEMORY_MAPPED_NOR_BLOCK_CONFIG g_STM32L4_BS_Config =
{
    { // BLOCK_CONFIG
        {
            STM32L4__WP_GPIO_PIN,          // GPIO_PIN             Pin;
            STM32L4__WP_ACTIVE,            // BOOL                 ActiveState;
        },

        &g_STM32L4_DeviceInfo,             // BlockDeviceinfo
    },

    { // CPU_MEMORY_CONFIG
        STM32L4__CHIP_SELECT,              // UINT8  CPU_MEMORY_CONFIG::ChipSelect;
        TRUE,                              // UINT8  CPU_MEMORY_CONFIG::ReadOnly;
        STM32L4__WAIT_STATES,              // UINT32 CPU_MEMORY_CONFIG::WaitStates;
        STM32L4__RELEASE_COUNTS,           // UINT32 CPU_MEMORY_CONFIG::ReleaseCounts;
        STM32L4__BIT_WIDTH,                // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        STM32L4__BASE_ADDRESS,             // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        STM32L4__SIZE_IN_BYTES,            // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,                                 // UINT8  CPU_MEMORY_CONFIG::XREADYEnable 
        0,                                 // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                                 // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0,                                     // UINT32 ChipProtection;
    FLASH_MANUFACTURER_CODE,               // UINT32 ManufacturerCode;
    FLASH_DEVICE_CODE,                     // UINT32 DeviceCode;
};

struct BlockStorageDevice g_STM32L4_BS;

