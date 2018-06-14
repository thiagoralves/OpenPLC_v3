
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

namespace DotNetMasterDemo
{
    class Program
    {
        // demonstrates how to build a set of command headers for a complex command request
        static ICommandHeaders GetCommandHeaders()
        {
            var crob = new ControlRelayOutputBlock(ControlCode.PULSE_ON, 1, 100, 100);
            var ao = new AnalogOutputDouble64(1.37);
            
            return CommandSet.From(
                CommandHeader.From(IndexedValue.From(crob, 0)),
                CommandHeader.From(IndexedValue.From(ao, 1))
            );
        }

        static int Main(string[] args)
        {
            IDNP3Manager mgr = DNP3ManagerFactory.CreateManager(1, new PrintingLogAdapter());
            
            var channel = mgr.AddTCPClient("client", LogLevels.NORMAL | LogLevels.APP_COMMS, ChannelRetry.Default, "127.0.0.1", 20000, ChannelListener.Print());                       

            var config = new MasterStackConfig();                      

            //setup your stack configuration here.
            config.link.localAddr = 1;
            config.link.remoteAddr = 10;

            var key = new byte[16];
            for(int i=0; i < key.Length; ++i)
            {
                key[i] = 0xFF;
            }

            var master = channel.AddMaster("master", PrintingSOEHandler.Instance, DefaultMasterApplication.Instance, config);
            
            // you a can optionally add various kinds of polls
            var integrityPoll = master.AddClassScan(ClassField.AllClasses, TimeSpan.FromMinutes(1), TaskConfig.Default);
            var rangePoll = master.AddRangeScan(30, 2, 5, 7, TimeSpan.FromSeconds(20), TaskConfig.Default);
            var classPoll = master.AddClassScan(ClassField.AllEventClasses, TimeSpan.FromSeconds(5), TaskConfig.Default);                                   
            
            /* you can also do very custom scans
            var headers = new Header[] { Header.Range8(1, 2, 7, 8), Header.Count8(2, 3, 7) };
            var weirdPoll = master.AddScan(headers, TimeSpan.FromSeconds(20));
            */
            
            master.Enable(); // enable communications

            Console.WriteLine("Enter a command");

            while (true)
            {                
                switch (Console.ReadLine())
                { 
                    case "a":
                        // perform an ad-hoc scan of all analogs
                        master.ScanAllObjects(30, 0, TaskConfig.Default);
                        break;
                    case "c":                        
                        var task = master.SelectAndOperate(GetCommandHeaders(), TaskConfig.Default);
                        task.ContinueWith((result) => Console.WriteLine("Result: " + result.Result));
                        break;                        
                    case "o":
                        var crob = new ControlRelayOutputBlock(ControlCode.PULSE_ON, 1, 100, 100);
                        var single = master.SelectAndOperate(crob, 1, TaskConfig.Default);
                        single.ContinueWith((result) => Console.WriteLine("Result: " + result.Result));
                        break;
                    case "l":
                        // add interpretation to the current logging level
                        var filters = channel.GetLogFilters();
                        channel.SetLogFilters(filters.Add(LogFilters.TRANSPORT_TX | LogFilters.TRANSPORT_RX));
                        break;
                    case "i":
                        integrityPoll.Demand();
                        break;
                    case "r":
                        rangePoll.Demand();
                        break;
                    case "e":
                        classPoll.Demand();                        
                        break;                   
                    case "x":
                        return 0;                        
                    default:
                        break;
                }                           
            }                            
        }
    }
}
