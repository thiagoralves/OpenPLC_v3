using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Configuration information for a dnp3 outstation (outstation)
    /// Used as both input describing the startup configuration of the outstation, and as configuration state of mutable properties (i.e. unsolicited responses).
    /// Major feature areas are unsolicited responses, time synchronization requests, event buffer limits, and the DNP3 object/variations to use by default
    /// when the master requests class data or variation 0.
    /// </summary>
    public class OutstationParams
    {        
        /// <summary>
        /// The maximum number of controls the outstation will attempt to process from a single APDU
        /// </summary>
        public System.Byte maxControlsPerRequest = 16;

        /// <summary>
        ///  How long the outstation will allow an operate to proceed after a prior select
        /// </summary>
        public TimeSpan selectTimeout = TimeSpan.FromSeconds(10);

        /// <summary>
        /// Timeout for solicited confirms
        /// </summary>
        public TimeSpan solicitedConfirmTimeout = TimeSpan.FromSeconds(5);

        /// <summary>
        /// Timeout for unsolicited confirms
        /// </summary>
        public TimeSpan unsolicitedConfirmTimeout = TimeSpan.FromSeconds(5);

        /// <summary>
        /// Retry period for unsolicited failures
        /// </summary>
        public TimeSpan unsolicitedRetryPeriod = TimeSpan.FromSeconds(5);

        /// <summary>
        /// The maximum fragment size the outstation will use for fragments it sends
        /// </summary>
        public System.UInt32 maxTxFragSize = 2048;

        /// <summary>
        /// Global enabled / disable for unsolicted messages. If false, the NULL unsolicited message is not even sent
        /// </summary>
        public bool allowUnsolicited = false;

        /// <summary>
        /// Specifies which types are allowed in Class0 repsones. Defaults to all types.
        /// Use this field to disable some types for compatibility reasons.
        /// </summary>
        public StaticTypeBitField typesAllowedInClass0 = StaticTypeBitField.AllTypes();
       
        /// <summary>
        /// Class mask for unsolicted, default to 0 as unsolicited has to be enabled by master
        /// </summary>
        public ClassField unsolClassMask = ClassField.None;
    }  
}
