using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Automatak.DNP3.Interface;
using Automatak.DNP3.Adapter;

namespace DotNetMasterGPRSDemo
{
    class Program
    {
        static int Main(string[] args)
        {
            if (args.Length != 3)
            {
                Console.WriteLine("usage: CLRMasterGPRSTLSDemo.exe <ca certificate> <certificate chain> <private key>");
		        return -1;
            }

            var caCertificate = args[0];
	        var certificateChain = args[1];
	        var privateKey = args[2];

            var manager = DNP3ManagerFactory.CreateManager(4, new PrintingLogAdapter());            

            var listener = manager.CreateListener(
                "listener", 
                LogLevels.ALL,
                IPEndpoint.Localhost(20001),
                new TLSConfig(caCertificate, certificateChain, privateKey, 2),
                new DefaultListenCallbacks()
            );

            Console.WriteLine("Enter a command");

            while (true)
            {
                switch (Console.ReadLine())
                {
                    case "x":
                        // The manager does not automatically reclaim resources for stop executing threads in a finalizer
                        // Be sure to call this before exiting to cleanup all resources.
                        manager.Shutdown();
                        return 0;
                    default:
                        break;
                }
            }
        }
    }
}