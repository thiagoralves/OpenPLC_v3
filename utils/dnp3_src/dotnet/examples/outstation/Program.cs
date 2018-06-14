
//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project was forked on 01/01/2013 by Automatak, LLC and modifications
// may have been made to this file. Automatak, LLC licenses these modifications
// to you under the terms of the License.
//
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Automatak.DNP3.Adapter;
using Automatak.DNP3.Interface;

namespace DotNetOutstationDemo
{   
    class Program
    {
        static int Main(string[] args)
        {
            IDNP3Manager mgr = DNP3ManagerFactory.CreateManager(1, new PrintingLogAdapter());
            
            var channel = mgr.AddTCPServer("server", LogLevels.NORMAL, ChannelRetry.Default, "0.0.0.0", 20000, ChannelListener.Print());            

            var config = new OutstationStackConfig();            

            // configure the various measurements in our database
            config.databaseTemplate = new DatabaseTemplate(4, 1, 1, 1, 1, 1, 1, 0);
            config.databaseTemplate.binaries[0].clazz = PointClass.Class2;               
            // ....           
            config.outstation.config.allowUnsolicited = true;            

            // Optional: setup your stack configuration here
            config.link.localAddr = 10;
            config.link.remoteAddr = 1;

            var outstation = channel.AddOutstation("outstation", RejectingCommandHandler.Instance, DefaultOutstationApplication.Instance, config);

            outstation.Enable(); // enable communications

            Console.WriteLine("Press <Enter> to change a value");            
            bool binaryValue = false;
            double analogValue = 0;
            while (true)
            {
                switch (Console.ReadLine())
                {
                    case("x"):
                        return 0;
                    default:
                        {
                            binaryValue = !binaryValue;
                            ++analogValue;

                            // create a changeset and load it 
                            var changeset = new ChangeSet();
                            changeset.Update(new Binary(binaryValue, 1, DateTime.Now), 0);
                            changeset.Update(new Analog(analogValue, 1, DateTime.Now), 0);
                            outstation.Load(changeset);
                        }
                        break;
                }                              
            }
        }
    }
}
