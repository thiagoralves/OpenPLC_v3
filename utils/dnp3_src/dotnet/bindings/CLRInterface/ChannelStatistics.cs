using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Automatak.DNP3.Interface
{
   /// <summary>
   /// Statistics for a channel
   /// </summary>
   public class ChannelStatistics : IChannelStatistics
   {
       /// <summary>
       /// The number of times the channel has sucessfully opened
       /// </summary>
       public System.UInt32 numOpen = 0;

       /// <summary>
       /// The number of times the channel has failed to open
       /// </summary>
       public System.UInt32 numOpenFail = 0;

       /// <summary>
       /// The number of times the channel has closed either due to user intervention or an error
       /// </summary>
       public System.UInt32 numClose = 0;

       /// <summary>
       /// The number of bytes received
       /// </summary>
       public System.UInt32 numBytesRx = 0;

       /// <summary>
       /// The number of bytes transmitted
       /// </summary>
       public System.UInt32 numBytesTx = 0;

       /// <summary>
       /// Number of LPDUs discared due to CRC errors
       /// </summary>
       public System.UInt32 numCrcError = 0;

       /// <summary>
       /// Number of valid LPDUs received
       /// </summary>
       public System.UInt32 numLinkFrameRx = 0;

       /// <summary>
       /// Number of LPDUs transmitted
       /// </summary>
       public System.UInt32 numLinkFrameTx = 0;

       /// <summary>
       /// Number of LPDUs detected with bad / malformed contents
       /// </summary>
       public System.UInt32 numBadLinkFrameRx = 0;

       /// <summary>
       /// The number of times the channel has sucessfully opened
       /// </summary>
       uint IChannelStatistics.NumOpen
       {
           get { return numOpen; }
       }

       /// <summary>
       /// The number of times the channel has failed to open
       /// </summary>
       uint IChannelStatistics.NumOpenFail
       {
           get { return numOpenFail; }
       }

       /// <summary>
       /// The number of times the channel has closed either due to user intervention or an error
       /// </summary>
       uint IChannelStatistics.NumClose
       {
           get { return numClose; }
       }

       /// <summary>
       /// The number of bytes received
       /// </summary>
       uint IChannelStatistics.NumBytesRx
       {
           get { return numBytesRx; }
       }

       /// <summary>
       /// The number of bytes transmitted
       /// </summary>
       uint IChannelStatistics.NumBytesTx
       {
           get { return numBytesTx; }
       }

       // <summary>
       /// Number of frames discared due to CRC errors
       /// </summary>
       uint IChannelStatistics.NumCrcError
       {
           get { return numCrcError; }
       }

       /// <summary>
       /// Number of valid LPDUs received
       /// </summary>
       uint IChannelStatistics.NumLinkFrameRx
       {
           get { return numLinkFrameRx; }
       }

       /// <summary>
       /// Number of valid LPDUs transmitted
       /// </summary>
       uint IChannelStatistics.NumLinkFrameTx
       {
           get { return numLinkFrameTx; }
       }

       /// <summary>
       /// Number of LPDUs detected with bad / malformed contents
       /// </summary>
       uint IChannelStatistics.NumBadLinkFrameRx
       {
           get { return numBadLinkFrameRx; }
       }      
   }
}
