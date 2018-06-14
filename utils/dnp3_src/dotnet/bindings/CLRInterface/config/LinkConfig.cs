using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Configuration class for the link layer
    /// </summary>
    public class LinkConfig
    {       
        /// <summary>
        /// Full constructor
        /// </summary>
        /// <param name="isMaster">true if this layer will be used with a master, false otherwise</param>
        /// <param name="useConfirms">true to use link layer confirmations for all data, false otherwise</param>
        /// <param name="numRetry">The number of retry attempts the link will attempt after the initial try</param>
        /// <param name="localAddr">dnp3 address of the local device</param>
        /// <param name="remoteAddr">dnp3 address of the remote device</param>
        /// <param name="responseTimeout">the response timeout for confirmed requests</param>
        /// <param name="keepAliveTimeout">the keep-alive timeout interval</param>
        public LinkConfig(  bool isMaster,
                            bool useConfirms,
                            System.UInt32 numRetry,
                            System.UInt16 localAddr,
                            System.UInt16 remoteAddr,
                            TimeSpan responseTimeout,
                            TimeSpan keepAliveTimeout)
        {
            this.isMaster = isMaster;
            this.useConfirms = useConfirms;
            this.numRetry = numRetry;
            this.localAddr = localAddr;
            this.remoteAddr = remoteAddr;
            this.responseTimeout = responseTimeout;
            this.keepAliveTimeout = keepAliveTimeout;
        }

        /// <summary>
        /// Defaults constructor
        /// </summary>
        /// <param name="isMaster">true if this layer will be used with a master, false otherwise</param>
        /// <param name="useConfirms">true to use link layer confirmations for all data, false otherwise</param>
        public LinkConfig(bool isMaster, bool useConfirms)
            : this(
                isMaster,
                useConfirms,
                DefaultNumRetries,
                GetDefaultSourceAddress(isMaster),
                GetDefaultDestinationAddress(isMaster),
                DefaultResponseTimeout,
                DefaultKeepAliveTimeout
            )
        {

        }

        /// <summary>
        /// Defaults to master
        /// </summary>
        public LinkConfig()
            : this(true, false)
        {

        }

        public static System.UInt16 DefaultOutstationAddress
        {
            get
            {
                return 1024;
            }
        }

        public static System.UInt16 DefaultMasterAddress
        {
            get
            {
                return 1;
            }
        }

        public static System.UInt32 DefaultNumRetries
        {
            get
            {
                return 0;
            }
        }

        public static System.UInt16 GetDefaultSourceAddress(bool isMaster)
        {
            return isMaster ? DefaultMasterAddress : DefaultOutstationAddress;
        }

        public static System.UInt16 GetDefaultDestinationAddress(bool isMaster)
        {
            return isMaster ? DefaultOutstationAddress : DefaultMasterAddress;
        }

        public static System.UInt16 GetDefaultAddress(bool isMaster)
        {
            return isMaster ? DefaultMasterAddress : DefaultOutstationAddress;
        }

        public static TimeSpan DefaultResponseTimeout
        {
            get
            {
                return TimeSpan.FromSeconds(1);
            }
        }

        public static TimeSpan DefaultKeepAliveTimeout
        {
            get
            {
                return TimeSpan.FromSeconds(60);
            }
        }

        /// <summary>
        /// The master/outstation bit set on all messages
        /// </summary>
        public bool isMaster;

        /// <summary>
        /// If true, the link layer will send data requesting confirmation
        /// </summary> 
        public bool useConfirms;

        /// <summary>
        /// The number of retry attempts the link will attempt after the initial try
        /// </summary>
        public System.UInt32 numRetry;

        /// <summary>
        /// dnp3 address of the local device
        /// </summary>
        public System.UInt16 localAddr;

        /// <summary>
        /// dnp3 address of the remote device
        /// </summary>
        public System.UInt16 remoteAddr;

        /// <summary>
        /// the response timeout for confirmed requests
        /// </summary>
        [XmlIgnore]
        public TimeSpan responseTimeout;       

        /// <summary>
        /// the keep-alive timer timeout interval
        /// </summary>
        [XmlIgnore]
        public TimeSpan keepAliveTimeout;

        [XmlElement]
        public System.Int64 ResponeTimeoutMilliseconds
        {
            get
            {
                return (responseTimeout.Ticks / TimeSpan.TicksPerMillisecond);
            }
            set
            {
                responseTimeout = TimeSpan.FromMilliseconds(value);
            }
        }

       
        [XmlElement]
        public System.Int64 KeepAliveTimeoutMilliseconds
        {
            get
            {
                return (keepAliveTimeout.Ticks / TimeSpan.TicksPerMillisecond);
            }
            set
            {
                keepAliveTimeout = TimeSpan.FromMilliseconds(value);
            }
        }
    }
}
