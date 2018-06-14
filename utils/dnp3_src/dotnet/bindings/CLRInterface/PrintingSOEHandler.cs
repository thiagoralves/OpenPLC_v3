
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
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the terms of the License.
//
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{

    /// <summary>
    /// Singleton that prints all received values to the console.
    /// </summary>
    public class PrintingSOEHandler : ISOEHandler
    {
        private static readonly ISOEHandler instance = new PrintingSOEHandler();

        public static ISOEHandler Instance
        {
            get
            {
                return instance;
            }
        }

        public PrintingSOEHandler()
        {}

        void ISOEHandler.Start()
        {}

        void ISOEHandler.End()
        {}

        private static void PrintHeaderInfo(HeaderInfo info)
        {
            Console.WriteLine(String.Format("{0} : {1} : timestamps {2}", info.variation, info.qualifier, info.tsmode));
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<Binary>> values)
        {
            foreach(var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<DoubleBitBinary>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<Analog>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<Counter>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<FrozenCounter>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<BinaryOutputStatus>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<AnalogOutputStatus>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<OctetString>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<TimeAndInterval>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<BinaryCommandEvent>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<AnalogCommandEvent>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        void ISOEHandler.Process(HeaderInfo info, IEnumerable<IndexedValue<SecurityStat>> values)
        {
            foreach (var pair in values)
            {
                Print(pair.Value, pair.Index);
            }
        }

        private static void Print(Binary value, UInt16 index)
        {
            Console.WriteLine("Binary[" + index + "] " + value.ToString());                         
        }

        private static void Print(DoubleBitBinary value, UInt16 index)
        {            
            Console.WriteLine("DoubleBit[" + index + "] " + value.ToString());            
        }

        private static void Print(Analog value, UInt16 index)
        {          
            Console.WriteLine("Analog[" + index + "] " + value.ToString());         
        }

        private static void Print(Counter value, UInt16 index)
        {
            Console.WriteLine("Counter[" + index + "] " + value.ToString());            
        }

        private static void Print(FrozenCounter value, UInt16 index)
        {            
            Console.WriteLine("FrozenCounter[" + index + "] " + value.ToString());            
        }

        private static void Print(BinaryOutputStatus value, UInt16 index)
        {            
            Console.WriteLine("BinaryOutputStatus[" + index + "] " + value.ToString());            
        }

        private static void Print(AnalogOutputStatus value, UInt16 index)
        {            
            Console.WriteLine("AnalogOutputStatus[" + index + "] " + value.ToString());            
        }

        private static void Print(OctetString value, UInt16 index)
        {            
            Console.WriteLine("OctetString[" + index + "] lemgth: " + value.Bytes.Length);            
        }

        private static void Print(TimeAndInterval value, UInt16 index)
        {            
            Console.WriteLine(String.Format("TimeAndInterval[{0}] {1}", index, value));           
        }

        private static void Print(BinaryCommandEvent value, UInt16 index)
        {            
            Console.WriteLine(String.Format("BinaryCommandEvent[{0}] {1}", index, value));            
        }

        private static void Print(AnalogCommandEvent value, UInt16 index)
        {            
            Console.WriteLine(String.Format("AnalogCommandEvent[{0}] {1}", index, value));            
        }

        private static void Print(SecurityStat value, UInt16 index)
        {
            Console.WriteLine(String.Format("SecurityStat[{0}] {1}", index, value.Value));
        }
        
    }    
}
