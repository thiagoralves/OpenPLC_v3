using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Configuration information for the dnp3 master
    /// </summary>
    [Serializable]
    public class MasterConfig
    {

        /// <summary>
        /// Constructor with reasonable defaults
        /// </summary>
        public MasterConfig()
        {            
            timeSyncMode = TimeSyncMode.None;
            disableUnsolOnStartup = true;
            unsolClassMask = ClassField.AllEventClasses;
            startupIntegrityClassMask = ClassField.AllClasses;
            integrityOnEventOverflowIIN = true;
            eventScanOnEventsAvailableClassMask = ClassField.None;
            responseTimeout = TimeSpan.FromSeconds(5);
            taskRetryPeriod = TimeSpan.FromSeconds(5);
            taskStartTimeout = TimeSpan.FromSeconds(10);
        }
        
        /// <summary>
        /// If true, the master will do time syncs when it sees the time IIN bit from the outstation
        /// </summary>
        public TimeSyncMode timeSyncMode;

        /// <summary>
        /// If true, the master will disable unsol on startup for all 3 classes
        /// </summary>
        public bool disableUnsolOnStartup;

        /// <summary>
        /// Bitwise mask used determine which classes are enabled for unsol, if 0 unsol is not enabled
        /// </summary>
        public ClassField unsolClassMask;

        /// <summary>
        /// Which classes should be requested in a startup integrity scan, defaults to 3/2/1/0
        /// A mask equal to 0 means no startup integrity scan will be performed
        /// </summary>
        public ClassField startupIntegrityClassMask;

        /// <summary>
        /// Defines whether an integrity scan will be performed when the EventBufferOverflow IIN is detected
        /// </summary>
        public bool integrityOnEventOverflowIIN;

        /// <summary>
        /// Which classes should be requested in an event scan when detecting corresponding events available IIN
        /// </summary>
        public ClassField eventScanOnEventsAvailableClassMask;

        /// <summary>
        /// Application layer response timeout
        /// </summary>
        [XmlIgnore]
        public TimeSpan responseTimeout;

        /// <summary>
        /// Time delay beforce retrying a failed task
        /// </summary>
        [XmlIgnore]
        public TimeSpan taskRetryPeriod;

        /// <summary>
        /// Time delay beforce retrying a failed task
        /// </summary>
        [XmlIgnore]
        public TimeSpan taskStartTimeout;


        [XmlElement]
        public long TaskResponseTimeoutMilliseconds
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
        public long TaskRetryPeriodMilliseconds
        {
            get
            {
                return (taskRetryPeriod.Ticks / TimeSpan.TicksPerMillisecond);
            }
            set
            {
                taskRetryPeriod = TimeSpan.FromMilliseconds(value);
            }
        }

        [XmlElement]
        public long TaskStartTimeoutMilliseconds
        {
            get
            {
                return (taskStartTimeout.Ticks / TimeSpan.TicksPerMillisecond);
            }
            set
            {
                taskStartTimeout = TimeSpan.FromMilliseconds(value);
            }
        }
    }

}
