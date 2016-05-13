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
//  *** Flash Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "STM32L4_Flash.h"

#include "..\stm32l4xx.h"

////////////////////////////////////////////////////////////////////////////
// For Debugging
////////////////////////////////////////////////////////////////////////////
//#define DEBUG_FLASH
//#define DEBUG_FLASH_SKIP
//#define DEBUG_FLASH_ONLY_ERROR
//#define DEBUG_FLASH_WriteX
//#define DEBUG_FLASH_EraseBlock
//#define DEBUG_FLASH_Read
//#define DEBUG_FLASH_Memset

#ifndef FLASH
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)
#endif

	typedef	unsigned long long	UINT64;
    typedef UINT64 CHIP_WORD;

//#define FLASH_CR_PSIZE_BITS FLASH_CR_PSIZE_0 // 16 bit programming
    
#if SUPPLY_VOLTAGE_MV < 2100
    #error 16 bit Flash programming not allowed for voltages below 2.1V
#endif
#if SYSTEM_CYCLE_CLOCK_HZ < ONE_MHZ
    #error Flash programming not allowed for HCLK below 1MHz
#endif


    static const UINT32 STM32L4_FLASH_KEY1 = 0x45670123;
    static const UINT32 STM32L4_FLASH_KEY2 = 0xcdef89ab;


//--//
int lmemcmp(const void *p1, const void *p2, size_t len) __attribute__ ((section ("SectionForFlashOperations")));
int lmemcmp(const void *p1, const void *p2, size_t len)
{
    unsigned char *a, *b;
    size_t i;

    a = (unsigned char *)p1;
    b = (unsigned char *)p2;
    for(i = 0; i < len; i++) {
        if (*a != *b) {
        	debug_printf("%08x:%02x!=%02x\r\n", ((UINT32)a), *a, *b);
        	return (*a < *b) ? -1 : 1;
        }
        a++;
        b++;
    }
    return (int)0;
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Initializes a given block device for use
    // 
    // Input:
    //
    // Returns:
    //   true if successful; false if not
    //
    // Remarks:
    //    No other functions in this interface may be called
    //    until after Init returns.
    //
BOOL __section("SectionForFlashOperations")STM32L4_Flash_Driver::ChipInitialize( void* context )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    return TRUE;
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Initializes a given block device for use
    // 
    // Returns:
    //   true if successful; false if not
    //
    // Remarks:
    //   De initializes the device when no longer needed
    //
BOOL __section("SectionForFlashOperations")STM32L4_Flash_Driver::ChipUnInitialize( void* context )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    return TRUE;
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Gets the information describing the device
    //
const BlockDeviceInfo* __section("SectionForFlashOperations")STM32L4_Flash_Driver::GetDeviceInfo( void* context )
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    
    return config->BlockConfig.BlockDeviceInformation;    
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Reads data from a set of sectors
    //
    // Input:
    //    StartSector - Starting Sector for the read
    //    NumSectors  - Number of sectors to read
    //    pSectorBuff - pointer to buffer to read the data into.
    //                  Must be large enough to hold all of the data
    //                  being read.
    //
    // Returns:
    //   true if successful; false if not
    //
    // Remarks:
    //   This function reads the number of sectors specified from the device.
    //   
BOOL  __section("SectionForFlashOperations")STM32L4_Flash_Driver::Read( void* context, ByteAddress StartSector, UINT32 NumBytes, BYTE * pSectorBuff)
{
    // XIP device does not need to read into a buffer
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    if (pSectorBuff == NULL) return FALSE;

    CHIP_WORD* ChipAddress = (CHIP_WORD *)StartSector;
    CHIP_WORD* EndAddress  = (CHIP_WORD *)(StartSector + NumBytes);
    CHIP_WORD *pBuf        = (CHIP_WORD *)pSectorBuff;

    while(ChipAddress < EndAddress)
    {
        *pBuf++ = *ChipAddress++;
    }

    return TRUE;
}


    /////////////////////////////////////////////////////////
    // Description:
    //    Writes data to a set of sectors
    //
    // Input:
    //    StartSector - Starting Sector for the write
    //    NumSectors  - Number of sectors to write
    //    pSectorBuff - pointer to data to write.
    //                  Must be large enough to hold complete sectors
    //                  for the number of sectors being written.
    //
    // Returns:
    //   true if successful; false if not
    //
    // Remarks:
    //   This function reads the number of sectors specified from the device.
    //   
BOOL __section("SectionForFlashOperations")STM32L4_Flash_Driver::Write(void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite)
{
    NATIVE_PROFILE_PAL_FLASH();

    UINT32 error_code = 0;
    // Read-modify-write is used for FAT filesystems only
    if (ReadModifyWrite) return FALSE;
    
    if (FLASH->CR & FLASH_CR_LOCK) { // unlock
        FLASH->KEYR = STM32L4_FLASH_KEY1;
        FLASH->KEYR = STM32L4_FLASH_KEY2;
    }
        
    CHIP_WORD* ChipAddress = (CHIP_WORD *)Address;
    CHIP_WORD* EndAddress  = (CHIP_WORD *)(Address + NumBytes);
    CHIP_WORD *pBuf        = (CHIP_WORD *)pSectorBuff;

    FLASH->SR |= (FLASH_SR_EOP| FLASH_SR_OPERR);
    // enable programming
    FLASH->CR = FLASH_CR_PG;
    
    while(ChipAddress < EndAddress) {
        if (*ChipAddress != *pBuf) {
            // write data
            *ChipAddress = *pBuf;
            // wait for completion
            while (FLASH->SR & FLASH_SR_BSY);
            // check
            if (*ChipAddress != *pBuf) {
                debug_printf( "Flash_WriteToSector failure @ 0x%08x, wrote 0x%08x, read 0x%08x\r\n", (UINT32)ChipAddress, *pBuf, *ChipAddress );
                return FALSE;
            }
            FLASH->SR |= (FLASH_SR_EOP| FLASH_SR_OPERR);
        }
        ChipAddress++;
        pBuf++;
    }
    
    // reset & lock the controller
    FLASH->CR = FLASH_CR_LOCK;

    WriteX_exit:
	error_code = (UINT32)lmemcmp((void *)Address, (void *)pSectorBuff, NumBytes);
#if defined(DEBUG_FLASH) || defined(DEBUG_FLASH_WriteX)
#ifdef DEBUG_FLASH_ONLY_ERROR
    if (error_code != 0)
#endif
    {
        debug_printf("W(%x) %x", Address, NumBytes);
        debug_printf(":%d\r\n", error_code);
    }
#endif
    if (error_code == 0)
        return TRUE;
    return FALSE;
}

BOOL __section("SectionForFlashOperations")STM32L4_Flash_Driver::Memset(void* context, ByteAddress Address, UINT8 Data, UINT32 NumBytes)
{
    NATIVE_PROFILE_PAL_FLASH();

    // used for FAT filesystems only
    return FALSE;

}


BOOL __section("SectionForFlashOperations")STM32L4_Flash_Driver::GetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    // no metadata
    return FALSE;
}

BOOL __section("SectionForFlashOperations")STM32L4_Flash_Driver::SetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    // no metadata
    return FALSE;
}


    /////////////////////////////////////////////////////////
    // Description:
    //    Check a block is erased or not.
    // 
    // Input:
    //    BlockStartAddress - Logical Sector Address
    //
    // Returns:
    //   true if it is erassed, otherwise false
    //
    // Remarks:
    //    Check  the block containing the sector address specified.
    //    
BOOL __section("SectionForFlashOperations")STM32L4_Flash_Driver::IsBlockErased( void* context, ByteAddress BlockStart, UINT32 BlockLength )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    
    CHIP_WORD* ChipAddress = (CHIP_WORD *) BlockStart;
    CHIP_WORD* EndAddress  = (CHIP_WORD *)(BlockStart + BlockLength);
    
    while(ChipAddress < EndAddress)
    {
        if(*ChipAddress != (CHIP_WORD)-1)
        {
            return FALSE;
        }
        ChipAddress++;
    }

    return TRUE;
}


    /////////////////////////////////////////////////////////
    // Description:
    //    Erases a block
    // 
    // Input:
    //    Address - Logical Sector Address
    //
    // Returns:
    //   true if successful; false if not
    //
    // Remarks:
    //    Erases the block containing the sector address specified.
    //    
BOOL __section("SectionForFlashOperations")STM32L4_Flash_Driver::EraseBlock( void* context, ByteAddress Sector )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    BOOL flag;
    UINT32 error_code = 0;
    UINT32 num = (Sector - FLASH_BASE) / 0x800;
    
    if (FLASH->CR & FLASH_CR_LOCK) { // unlock
        FLASH->KEYR = STM32L4_FLASH_KEY1;
        FLASH->KEYR = STM32L4_FLASH_KEY2;
    }
        
    // enable erasing
    UINT32 cr = (num << 3) | FLASH_CR_PER;
    FLASH->CR = cr;
    // start erase
    cr |= FLASH_CR_STRT;
    FLASH->CR = cr;
    // assure busy flag is set up (see STM32L4 errata)
    FLASH->CR = cr;
    // wait for completion
    while (FLASH->SR & FLASH_SR_BSY);
    
    // reset & lock the controller
    FLASH->CR = FLASH_CR_LOCK;

    flag = IsBlockErased(context, (num * 0x800 + FLASH_BASE), 0x800);
    if (flag == FALSE) {
        error_code = 6;
    }
#if defined(DEBUG_FLASH) || defined(DEBUG_FLASH_EraseBlock)
	if (error_code != 0)
		debug_printf("E(%x):NG\r\n", Sector);
#ifndef DEBUG_FLASH_ONLY_ERROR
	else
		debug_printf("E(%x):OK\r\n", Sector);
#endif
#endif
    if (error_code == 0)
        return TRUE;
    return FALSE;
}



    /////////////////////////////////////////////////////////
    // Description:
    //   Changes the power state of the device
    // 
    // Input:
    //    State   - true= power on; false = power off
    //
    // Remarks:
    //   This function allows systems to conserve power by 
    //   shutting down the hardware when the system is 
    //   going into low power states.
    //
void  __section("SectionForFlashOperations")STM32L4_Flash_Driver::SetPowerState( void* context, UINT32 State )
{
}


//--// ---------------------------------------------------

#pragma arm section code = "SectionForFlashOperations"

UINT32 __section("SectionForFlashOperations")STM32L4_Flash_Driver::MaxSectorWrite_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    
    return config->BlockConfig.BlockDeviceInformation->MaxSectorWrite_uSec;
}


UINT32 __section("SectionForFlashOperations")STM32L4_Flash_Driver::MaxBlockErase_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();
    
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;

    return config->BlockConfig.BlockDeviceInformation->MaxBlockErase_uSec;
    
}


#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata = "g_STM32L4_Flash_DeviceTable"
#endif

struct IBlockStorageDevice g_STM32L4_Flash_DeviceTable =
{                          
    &STM32L4_Flash_Driver::ChipInitialize,
    &STM32L4_Flash_Driver::ChipUnInitialize,
    &STM32L4_Flash_Driver::GetDeviceInfo,
    &STM32L4_Flash_Driver::Read,
    &STM32L4_Flash_Driver::Write,
    &STM32L4_Flash_Driver::Memset,
    &STM32L4_Flash_Driver::GetSectorMetadata,
    &STM32L4_Flash_Driver::SetSectorMetadata,
    &STM32L4_Flash_Driver::IsBlockErased,
    &STM32L4_Flash_Driver::EraseBlock,
    &STM32L4_Flash_Driver::SetPowerState,
    &STM32L4_Flash_Driver::MaxSectorWrite_uSec,
    &STM32L4_Flash_Driver::MaxBlockErase_uSec,
};

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata 
#endif 






