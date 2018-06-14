using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Configuration of maximum event counts per event type.
    /// 
    /// The underlying implementation uses a *preallocated* heap buffer to store events
    /// until they are transmitted to the master. The size of this buffer is proportional
    /// to the TotalEvents() method, i.e. the sum of the maximum events for each type.
    /// Implementations should configure the buffers to store a reasonable number events
    /// given the polling frequency and memory restrictions of the target platform.
    /// 
    /// </summary>
    public class EventBufferConfig
    {
        /// <summary>
        /// All events set to same count
        /// </summary>
        public EventBufferConfig(UInt16 count)
        {
            this.maxBinaryEvents = count;
            this.maxDoubleBinaryEvents = count;
            this.maxAnalogEvents = count;
            this.maxCounterEvents = count;
            this.maxFrozenCounterEvents = count;
            this.maxBinaryOutputStatusEvents = count;
            this.maxAnalogOutputStatusEvents = count;
        }


        /// <summary>
        /// All events set to 100
        /// </summary>
        public EventBufferConfig() : this(100)
        {
           
        }

        /// <summary>
        /// The number of binary events the outstation will buffer before overflowing
        /// </summary>
        public System.UInt16 maxBinaryEvents;

        /// <summary>
        /// The number of double-bit binary events the outstation will buffer before overflowing
        /// </summary>
        public System.UInt16 maxDoubleBinaryEvents;

        /// <summary>
        /// The number of analog events the outstation will buffer before overflowing
        /// </summary>
        public System.UInt16 maxAnalogEvents;

        /// <summary>
        /// The number of counter events the outstation will buffer before overflowing
        /// </summary>
        public System.UInt16 maxCounterEvents;

        /// <summary>
        /// The number of frozen counter events the outstation will buffer before overflowing
        /// </summary>
        public System.UInt16 maxFrozenCounterEvents;

        /// <summary>
        /// The number of binary output status events the outstation will buffer before overflowing
        /// </summary>
        public System.UInt16 maxBinaryOutputStatusEvents;

        /// <summary>
        /// The number of analog output status events the outstation will buffer before overflowing
        /// </summary>
        public System.UInt16 maxAnalogOutputStatusEvents;
    }

}
