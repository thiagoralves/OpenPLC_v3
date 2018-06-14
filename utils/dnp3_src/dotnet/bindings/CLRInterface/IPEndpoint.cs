using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// An address-port pair
    /// </summary>
    public class IPEndpoint
    {
        public IPEndpoint(string address, UInt16 port)
        {
            this.address = address;
            this.port = port;
        }

        public static IPEndpoint AddAdapters(UInt16 port)
        {
            return new IPEndpoint("0.0.0.0", port);
        }

        public static IPEndpoint Localhost(UInt16 port)
        {
            return new IPEndpoint("127.0.0.1", port);
        }

        public readonly string address;
        public readonly UInt16 port;
    }
}
