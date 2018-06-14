using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Interface used to create an ephemeral master session
    /// </summary>
    public interface ISessionAcceptor
    {
        IMasterSession AcceptSession(string loggerid, ISOEHandler SOEHandler, IMasterApplication application, MasterStackConfig config);
    }
}
