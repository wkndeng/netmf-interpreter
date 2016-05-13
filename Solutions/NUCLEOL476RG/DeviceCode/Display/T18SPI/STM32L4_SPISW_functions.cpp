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
//  Copyright (c) Kentaro Sekimoto All rights reserved.
//
////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\..\..\..\DeviceCode\Targets\Native\STM32L4\DeviceCode\stm32l4xx.h"

void SPISW_GPIO_Initialize(void);

#define SPISWPIN    0

#if (SPISWPIN == 0)
// GR_KAEDE + Aitendo T18SPI LCD Shield
// 4 line SPI mode
// CLK  PA5
// DI   PA7
// CS   PB6
// RS   PA6
// Res  PA9

#define CS_LOW  (GPIOB->BSRR |= (0x00010000 << 6))
#define CS_HIGH (GPIOB->BSRR |= (0x00000001 << 6))
#define CK_LOW  (GPIOA->BSRR |= (0x00010000 << 5))
#define CK_HIGH (GPIOA->BSRR |= (0x00000001 << 5))
#define DI_LOW  (GPIOA->BSRR |= (0x00010000 << 7))
#define DI_HIGH (GPIOA->BSRR |= (0x00000001 << 7))
#define RS_LOW  (GPIOA->BSRR |= (0x00010000 << 6))
#define RS_HIGH (GPIOA->BSRR |= (0x00000001 << 6))
#define RS_IN   (GPIOA->IDR & (0x00000001 << 6))
#define RES_LOW  (GPIOA->BSRR |= (0x00010000 << 9))
#define RES_HIGH (GPIOA->BSRR |= (0x00000001 << 9))
#endif

void SPISW_GPIO_Initialize(void)
{
#if (SPISWPIN == 0)
    GPIOA->MODER &= ~((3UL << 2*5));    // PA5: Clear to input
    GPIOA->MODER &= ~((3UL << 2*7));    // PA7: Clear to input
    GPIOB->MODER &= ~((3UL << 2*6));    // PB6: Clear to input
    GPIOA->MODER &= ~((3UL << 2*6));    // PA6: Clear to input
    GPIOA->MODER &= ~((3UL << 2*9));    // PA9: Clear to input

    GPIOA->MODER |= ((1UL << 2*5));     // PA5: Output
    GPIOA->MODER |= ((1UL << 2*7));     // PA7: Output
    GPIOB->MODER |= ((1UL << 2*6));     // PB6: Output
    GPIOA->MODER |= ((1UL << 2*6));     // PA6: Output
    GPIOA->MODER |= ((1UL << 2*9));     // PA9: Output

    GPIOA->OSPEEDR |= ((3UL << 2*5));   // PA5: Very high speed
    GPIOA->OSPEEDR |= ((3UL << 2*7));   // PA7: Very high speed
    GPIOB->OSPEEDR |= ((3UL << 2*6));   // PB6: Very high speed
    GPIOA->OSPEEDR |= ((3UL << 2*6));   // PA6: Very high speed
    GPIOA->OSPEEDR |= ((3UL << 2*9));   // PA9: Very high speed

#endif
}

void SPISW_Initialize(void)
{
    SPISW_GPIO_Initialize();
    CS_HIGH;
    CK_HIGH;
}

void SPISW_Reset(UINT32 v)
{
    if (v) {
        RES_HIGH;
    } else {
        RES_LOW;
    }
}

void SPISW_Write(UINT8 dat)
{
    UINT8 i = 8;
    while (i-- > 0) {
        if (dat & 0x80)
            DI_HIGH;
        else
            DI_LOW;
        CK_LOW;
        CK_HIGH;
        dat <<= 1;
    }
}

void SPISW_LCD_cmd(UINT8 dat)
{
    // Enter command mode: SDATA=LOW at rising edge of 1st SCLK
    CS_LOW;
    RS_LOW;
    SPISW_Write(dat);
    CS_HIGH;
}

void SPISW_LCD_dat(UINT8 dat)
{
    // Enter data mode: SDATA=HIGH at rising edge of 1st SCLK
    CS_LOW;
    RS_HIGH;
    SPISW_Write(dat);
    CS_HIGH;
}
