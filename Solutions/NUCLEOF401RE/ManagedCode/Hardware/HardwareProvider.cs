using System;
using System.Threading;
using System.Runtime.CompilerServices;
using Microsoft.SPOT.Hardware;

namespace Microsoft.SPOT.Hardware.NUCLEOF401RE
{
    internal class NUCLEOF401RE_HardwareProvider : HardwareProvider
    {
        static NUCLEOF401RE_HardwareProvider()
        {
            Microsoft.SPOT.Hardware.HardwareProvider.Register(new NUCLEOF401RE_HardwareProvider());
        }
    }
}
