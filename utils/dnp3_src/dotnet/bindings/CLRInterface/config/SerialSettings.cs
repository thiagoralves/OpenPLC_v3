
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

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Settings class for  serial port
    /// </summary>
    public class SerialSettings {

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="port">port name, i.e. COM1</param>
        /// <param name="baud">Baud rate for the port. Allowed values depend on the specific port</param>
        /// <param name="dataBits">Number of data bits. Typically 8.</param>
        /// <param name="stopBits"> Stop bits, typically 1</param>
        /// <param name="parity"> Parity, typically none</param>
        /// <param name="flowControl">Flow control, typically none</param>
        public SerialSettings(String port, int baud, int dataBits, StopBits stopBits, Parity parity, FlowControl flowControl)
        {
            this.port = port;
            this.baud = baud;
            this.dataBits = dataBits;
            this.stopBits = stopBits;
            this.parity = parity;
            this.flowControl = flowControl;
        }
        
        public readonly String port;
        public readonly int baud;
        public readonly int dataBits;
        public readonly StopBits stopBits;
        public readonly Parity parity;
        public readonly FlowControl flowControl;

        /// <summary>
        /// Convert the serial settings to a string
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            var lines = new String[] 
            {
                "port: " + port, 
                "baud: " + baud, 
                "dataBits: " + dataBits, 
                "stopBits: " + stopBits, 
                "parity: " + parity,
                "flow control: " + flowControl
            };

            return String.Join(Environment.NewLine, lines);
        }
    }
}
