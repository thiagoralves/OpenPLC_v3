using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Aggreate configuration for a master stack
    /// </summary>
    public class MasterStackConfig
    {
        /// <summary>
        /// Reasonable defaults
        /// </summary>
        public MasterStackConfig()
        {
            this.link = new LinkConfig(true, false);
            this.master = new MasterConfig();
        }

        /// <summary>
        /// Configuration for a master
        /// </summary>
        public MasterConfig master;

        /// <summary>
        /// Configuration for the link layer
        /// </summary>
        public LinkConfig link;
    }
}
