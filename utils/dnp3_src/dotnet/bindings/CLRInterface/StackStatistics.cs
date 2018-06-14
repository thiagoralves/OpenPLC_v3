using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Provides various counters for common transport events
    /// </summary>
    public class StackStatistics : IStackStatistics
    {
        public System.UInt32 numTransportRx = 0;
        public System.UInt32 numTransportTx = 0;
        public System.UInt32 numTransportErrorRx = 0;

        /// <summary>
        /// The number of transport frames received
        /// </summary>
        System.UInt32 IStackStatistics.NumTransportRx
        {
            get { return numTransportRx; }
        }

        /// <summary>
        /// The number of transport frames transmitted
        /// </summary>
        System.UInt32 IStackStatistics.NumTransportTx
        {
            get { return numTransportTx; }
        }

        /// <summary>
        /// The number of transport frames that caused an error
        /// </summary>
        System.UInt32 IStackStatistics.NumTransportErrorRx
        {
            get { return numTransportErrorRx; }
        }
    }
}
