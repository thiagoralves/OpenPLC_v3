using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Represents the set of IIN bits that can be controlled by the application
    /// </summary>
    public struct ApplicationIIN
    {        
        public bool needTime;
        public bool localControl;
        public bool deviceTrouble;
        public bool configCorrupt;
    }
}
