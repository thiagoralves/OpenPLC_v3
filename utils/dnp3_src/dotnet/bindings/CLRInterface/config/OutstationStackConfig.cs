using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Aggreate configuration for an outstation stack
    /// </summary>
    public class OutstationStackConfig
    {
        /// <summary>
        /// Constructor with reasonable defaults
        /// </summary>
        public OutstationStackConfig()
        {
            this.outstation = new OutstationConfig();            
            this.databaseTemplate = new DatabaseTemplate(10);
            this.link = new LinkConfig(false, false);
        }
      
        /// <summary>
        /// Outstation config
        /// </summary>
        public OutstationConfig outstation;        

        /// <summary>
        /// Device template that specifies database layout, control behavior
        /// </summary>
        public DatabaseTemplate databaseTemplate;

        /// <summary>
        /// Link layer config
        /// </summary>
        public LinkConfig link;
    }

}
