using System;
using System.Threading;
using System.Runtime.CompilerServices;
using Microsoft.SPOT.Hardware;

namespace Microsoft.SPOT.Hardware.NUCLEOL476RG
{
    internal class NUCLEOL476RG_HardwareProvider : HardwareProvider
    {
        static NUCLEOL476RG_HardwareProvider()
        {
            Microsoft.SPOT.Hardware.HardwareProvider.Register(new NUCLEOL476RG_HardwareProvider());
        }
    }
}
