using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Provides various counters for common transport events
    /// </summary>
    public interface IStackStatistics
    {
        /// <summary>
        /// The number of transport frames received
        /// </summary>
        System.UInt32 NumTransportRx { get; }

        /// <summary>
        /// The number of transport frames transmitted
        /// </summary>
        System.UInt32 NumTransportTx { get; }

        /// <summary>
        /// The number of transport frames that caused an error
        /// </summary>
        System.UInt32 NumTransportErrorRx { get; }
    }
}
