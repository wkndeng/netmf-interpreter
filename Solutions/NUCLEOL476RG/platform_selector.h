/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This file is part of the Microsoft .NET Micro Framework Porting Kit Code Samples and is unsupported. 
// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use these files except in compliance with the License.
// You may obtain a copy of the License at:
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing
// permissions and limitations under the License.
// 
//  Based on the Implementation for (STM32F4) by Oberon microsystems, Inc.
//  Implementation for NUCLEOL476RG board (STM32L4): Portions Copyright (c) Kentaro Sekimoto
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_NUCLEOL476RG_SELECTOR_H_
#define _PLATFORM_NUCLEOL476RG_SELECTOR_H_

/////////////////////////////////////////////////////////
//
// processor and features
//

#if defined(PLATFORM_ARM_NUCLEOL476RG)
#define HAL_SYSTEM_NAME "NUCLEOL476RG"

#define PLATFORM_ARM_STM32L4 1 // STM32L476 cpu
#define USB_ALLOW_CONFIGURATION_OVERRIDE 1
#define STM32L476xx

//
// processor and features
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// constants
//

#define GPIO_PORTA 0
#define GPIO_PORTB 1
#define GPIO_PORTC 2
#define GPIO_PORTD 3
#define GPIO_PORTE 4
// The remaining ports are not broken out - except PH0 and PH1,
// which are deliberately omitted to keep the range continuous.

#define PORT_PIN(port,pin) ( ( (int)port) * 16 + ( pin ) )

// System clock
#define SYSTEM_CLOCK_HZ                  80000000   //  80 MHz
#define SYSTEM_CYCLE_CLOCK_HZ            80000000   //  80 MHz
#define SYSTEM_APB1_CLOCK_HZ             80000000   //  80 MHz
#define SYSTEM_APB2_CLOCK_HZ             80000000   //  80 MHz

//#define SYSTEM_CRYSTAL_CLOCK_HZ           8000000   // 8 MHz external clock
#define SYSTEM_CRYSTAL_CLOCK_HZ          0          // 8 MHz external clock

#define SUPPLY_VOLTAGE_MV                    3300   // 3.3V supply

#define CLOCK_COMMON_FACTOR               1000000   // GCD(SYSTEM_CLOCK_HZ, 1M)

#define SLOW_CLOCKS_PER_SECOND            1000000   // 1 MHz
#define SLOW_CLOCKS_TEN_MHZ_GCD           1000000   // GCD(SLOW_CLOCKS_PER_SECOND, 10M)
#define SLOW_CLOCKS_MILLISECOND_GCD          1000   // GCD(SLOW_CLOCKS_PER_SECOND, 1k)

#define FLASH_MEMORY_Base               0x08000000
#define FLASH_MEMORY_Size               0x00100000
#define SRAM1_MEMORY_Base               0x20000000
#define SRAM1_MEMORY_Size               0x00018000

#define TXPROTECTRESISTOR               RESISTOR_DISABLED
#define RXPROTECTRESISTOR               RESISTOR_DISABLED
#define CTSPROTECTRESISTOR              RESISTOR_DISABLED
#define RTSPROTECTRESISTOR              RESISTOR_DISABLED

#define TOTAL_GPIO_PORT                 (GPIO_PORTE + 1)
#define TOTAL_GPIO_PINS                 (TOTAL_GPIO_PORT*16)

#define INSTRUMENTATION_H_GPIO_PIN      GPIO_PIN_NONE

#define TOTAL_USART_PORT                5 // 5 physical UARTS

#define USART_DEFAULT_PORT              COM2	// USART2 - MBED Virtual COM Port
#define USART_DEFAULT_BAUDRATE          115200

#define TOTAL_GENERIC_PORTS             1 // 1 generic port extensions (ITM channel 0 )
#define ITM_GENERIC_PORTNUM             0 // ITM0 is index 0 in generic port interface table

#define DEBUG_TEXT_PORT                 COM2	//ITM0
#define STDIO                           COM2	//ITM0
#define DEBUGGER_PORT                   COM2
#define MESSAGING_PORT                  COM2

#define TOTAL_USB_CONTROLLER            1  // Silicon has 2, but only one supported in this port at this time...
#define USB_MAX_QUEUES                  4  // 4 endpoints (EP0 + 3)

// System Timer Configuration
#define STM32L4_32B_TIMER 2
#define STM32L4_16B_TIMER 3

// Pin Configuration
#define STM32L4_ADC 3
#define STM32L4_AD_CHANNELS {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}

#define STM32L4_PWM_TIMER {4,4,4,4}
#define STM32L4_PWM_CHNL  {0,1,2,3}
#define STM32L4_PWM_PINS  {60,61,62,63} // PD12-PD15

#define STM32L4_SPI_SCLK_PINS {5, 26, 42} // PA5, PB10, PC10
#define STM32L4_SPI_MISO_PINS {6, 34, 43} // PA6, PC2, PC11
#define STM32L4_SPI_MOSI_PINS {7, 35, 44} // PA7, PC3, PC12

#define STM32L4_I2C_PORT     1
#define STM32L4_I2C_SCL_PIN  PORT_PIN( GPIO_PORTB, 8 ) // PB8
#define STM32L4_I2C_SDA_PIN  PORT_PIN( GPIO_PORTB, 9 ) // PB9

#define STM32L4_UART_RXD_PINS {10,  3, 57, 43, 50} // PA10, PA3, PD9, PC11, PD2
#define STM32L4_UART_TXD_PINS {22,  2, 56, 42, 44} // PB6, PA2, PD8, PC10, PC12
#define STM32L4_UART_CTS_PINS {20,  0,  6} // PB4, PA0, PA6
#define STM32L4_UART_RTS_PINS {19,  1, 17} // PB3, PA1, PB1

// User LEDs
#define LED3 PORT_PIN(GPIO_PORTD, 13) // PD.13 (orange)
#define LED4 PORT_PIN(GPIO_PORTD, 12) // PD.12 (green)
#define LED5 PORT_PIN(GPIO_PORTD, 14) // PD.14 (red)
#define LED6 PORT_PIN(GPIO_PORTD, 15) // PD.15 (blue)

// TinyBooter entry using GPIO
#define TINYBOOTER_ENTRY_GPIO_PIN       PORT_PIN(GPIO_PORTA, 0) // 'User' button
#define TINYBOOTER_ENTRY_GPIO_STATE     TRUE                    // Active high
#define TINYBOOTER_ENTRY_GPIO_RESISTOR  RESISTOR_DISABLED       // No internal resistor, there is external pull-down (R39)

//
// constants
/////////////////////////////////////////////////////////

#include <processor_selector.h>

#endif // PLATFORM_ARM_NUCLEOL476RG

#endif // _PLATFORM_NUCLEOL476RG_SELECTOR_H_
