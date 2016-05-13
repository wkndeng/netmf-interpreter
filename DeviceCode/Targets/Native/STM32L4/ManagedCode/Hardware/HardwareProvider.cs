using System;
using System.Threading;
using System.Runtime.CompilerServices;
using Microsoft.SPOT.Hardware;

namespace Microsoft.SPOT.Hardware.STM32L4
{
    internal class STM32L4HardwareProvider : HardwareProvider
    {
        static STM32L4HardwareProvider()
        {
            Microsoft.SPOT.Hardware.HardwareProvider.Register(new STM32L4HardwareProvider());
        }
    }
}
