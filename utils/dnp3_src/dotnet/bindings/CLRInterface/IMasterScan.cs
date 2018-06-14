using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Interface for controlling a permanently bound scan
    /// </summary>
    public interface IMasterScan
    {        
        /// <summary>
        /// Ask the scan to run ASAP
        /// </summary>
        void Demand();
    }
}
