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

#ifndef _STM32L4_I2CSW_H_
#define _STM32L4_I2CSW_H_

struct STM32L4_I2CSW_Driver
{
    static const UINT8 c_DirectionRead = 1;
    static const UINT8 c_DirectionWrite = 0;
    I2C_HAL_XACTION *m_currentXAction;
    I2C_HAL_XACTION_UNIT *m_currentXActionUnit;
    BOOL m_initialized;

    //--//

    static BOOL Initialize();
    static BOOL Uninitialize();
    static void MasterXAction_Start(I2C_HAL_XACTION *xAction, bool repeatedStart);
    static void MasterXAction_Stop();
    static void GetClockRate(UINT32 rateKhz, UINT8& clockRate, UINT8& clockRate2);
    static void WriteToSubordinate(I2C_HAL_XACTION_UNIT *unit);
    static void ReadFromSubordinate(I2C_HAL_XACTION_UNIT *unit);
    static void GetPins(GPIO_PIN& scl, GPIO_PIN& sda);
    static void Abort(void);

private:
};

#endif /* _STM32L4_I2CSW_H_ */
