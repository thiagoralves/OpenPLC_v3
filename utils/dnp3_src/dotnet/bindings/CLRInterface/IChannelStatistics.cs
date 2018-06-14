using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Statistics for a channel
    /// </summary>
    public interface IChannelStatistics
    {
        /// <summary>
        /// The number of times the channel has sucessfully opened
        /// </summary>
        System.UInt32 NumOpen { get; }

        /// <summary>
        /// The number of times the channel has failed to open
        /// </summary>
        System.UInt32 NumOpenFail { get; }

        /// <summary>
        /// The number of times the channel has closed either due to user intervention or an error
        /// </summary>
        System.UInt32 NumClose { get; }

        /// <summary>
        /// The number of bytes received
        /// </summary>
        System.UInt32 NumBytesRx { get; }

        /// <summary>
        /// The number of bytes transmitted
        /// </summary>
        System.UInt32 NumBytesTx { get; }

        /// <summary>
        /// Number of frames discared due to CRC errors
        /// </summary>
        System.UInt32 NumCrcError { get; }

        /// <summary>
        /// Number of valid LPDUs received
        /// </summary>
        System.UInt32 NumLinkFrameRx { get; }

        /// <summary>
        /// Number of valid LPDUs transmitted
        /// </summary>
        System.UInt32 NumLinkFrameTx { get; }

        /// <summary>
        /// Number of LPDUs detected with bad / malformed contents
        /// </summary>
        System.UInt32 NumBadLinkFrameRx { get; }
    }
}
