
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

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// A message from the logging framework
    /// </summary>
    public class LogEntry
    {
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="filters">Bitfield for the message</param>
        /// <param name="alias">Logger alias that recorded the message</param>
        /// <param name="location">Location in the code</param>
        /// <param name="message">Body of the message</param>                
        public LogEntry(UInt32 filters, String alias, String location, String message)
        {
            this.filter = new LogFilter(filters);
            this.alias = alias;
            this.location = location;
            this.message = message;                            
        }

        /// <summary>
        /// Level enumeration of the message
        /// </summary>
        public readonly LogFilter filter;
        
        /// <summary>
        /// Logger alias that recorded the message
        /// </summary>
        public readonly String alias;

        /// <summary>
        /// Location in the code
        /// </summary>
	    public readonly String location;

        /// <summary>
        /// Body of the message
        /// </summary>
	    public readonly String message;        
    }

    /// <summary>
    /// Callback interface to handle log messages
    /// </summary>
    public interface ILogHandler
    {        
	    /// <summary>
	    /// log an error message
	    /// </summary>
	    /// <param name="entry">log entry instance</param>
        void Log(LogEntry entry);	  
    }

    /// <summary>
    /// Singleton log adapter that prints messages to the console
    /// </summary>
    public class PrintingLogAdapter : ILogHandler
    {
        public PrintingLogAdapter()
        {}

        public void Log(LogEntry entry)
        {
            var msg = String.Format("{0} - {1} - {2} - {3}",
                DateTime.Now,
                entry.alias,
                LogFilters.GetFilterString(entry.filter.Flags),
                entry.message
            );

            Console.WriteLine(msg);
        }        
    }
}
