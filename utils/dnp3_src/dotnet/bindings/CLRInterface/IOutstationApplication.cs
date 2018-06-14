
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
    /// Outstation application code implements this interface to interface with the stack
    /// </summary>
    public interface IOutstationApplication : ILinkStatusListener
    {
        /// <summary>
        /// true of the outstation should allow absolute time to be written
        /// </summary>
        bool SupportsWriteAbsoluteTime { get; }

        /// <summary>
        /// Called when the master writes the time
        /// </summary>
        /// <param name="millisecSinceEpoch">milliseconds since Unix epoch</param>
        /// <returns>true, if the wrote is successful</returns>
        bool WriteAbsoluteTime(UInt64 millisecSinceEpoch);

        bool SupportsWriteTimeAndInterval();

        bool WriteTimeAndInterval(IEnumerable<IndexedValue<TimeAndInterval>> values);

        /// <summary>
        /// Query about class assigment support
        /// </summary>
        /// <returns>true if the outstation supports assigning class</returns>
        bool SupportsAssignClass();

        /// <summary>
        /// Called when the master assigns class to a particular type of measurements
        /// </summary>
        /// <param name="type">the type of the assignment</param>
        /// <param name="clazz">the class assigned</param>
        /// <param name="start">the start of the measurement range</param>
        /// <param name="stop">the end of the measurement range</param>
        void RecordClassAssignment(AssignClassType type, PointClass clazz, UInt16 start, UInt16 stop);

        /// <summary>
        /// Application controlled IIN bits
        /// </summary>
        ApplicationIIN ApplicationIndications
        {
            get;
        }

        /// Query the outstation for the cold restart mode it supports
	    RestartMode ColdRestartSupport { get; }

	    /// Query the outstation for the warm restart mode it supports
        RestartMode WarmRestartSupport { get; }

	    /// The outstation should perform a complete restart.
	    /// See the DNP3 specification for a complete descripton of normal behavior
	    /// @return number of seconds or milliseconds until restart is complete. The value
	    /// is interpreted based on the Restart Mode returned from ColdRestartSupport()
	    UInt16 ColdRestart();

	    /// The outstation should perform a partial restart of only the DNP3 application.
	    /// See the DNP3 specification for a complete descripton of normal behavior
	    /// @return number of seconds or milliseconds until restart is complete. The value
	    /// is interpreted based on the Restart Mode returned from WarmRestartSupport()
        UInt16 WarmRestart();
    }

    public class DefaultOutstationApplication : IOutstationApplication
    {
        private static IOutstationApplication instance = new DefaultOutstationApplication();

        public static IOutstationApplication Instance
        {
            get
            {
                return instance;
            }
        }

        private DefaultOutstationApplication() { }

        void ILinkStatusListener.OnStateChange(LinkStatus value) {}

        void ILinkStatusListener.OnKeepAliveInitiated() {}

        void ILinkStatusListener.OnKeepAliveFailure() {}

        void ILinkStatusListener.OnKeepAliveSuccess() {}

        bool IOutstationApplication.SupportsWriteAbsoluteTime
        {
            get {
                return false;
            }
            
        }

        bool IOutstationApplication.WriteAbsoluteTime(UInt64 millisecSinceEpoch)
        {            
            return false;
        }

        bool IOutstationApplication.SupportsWriteTimeAndInterval()
        {
            return false;
        }

        bool IOutstationApplication.WriteTimeAndInterval(IEnumerable<IndexedValue<TimeAndInterval>> values)
        {
            return false;
        }

        bool IOutstationApplication.SupportsAssignClass()
        {
            return false;
        }

        void IOutstationApplication.RecordClassAssignment(AssignClassType type, PointClass clazz, UInt16 start, UInt16 stop)
        { 
        
        }

        ApplicationIIN IOutstationApplication.ApplicationIndications
        {
            get { return new ApplicationIIN(); }
        }


        RestartMode IOutstationApplication.ColdRestartSupport
        {
            get 
            {
                return RestartMode.UNSUPPORTED;
            }
        }

        RestartMode IOutstationApplication.WarmRestartSupport
        {
            get
            {
                return RestartMode.UNSUPPORTED;
            }
        }

        ushort IOutstationApplication.ColdRestart()
        {
            return UInt16.MaxValue;
        }

        ushort IOutstationApplication.WarmRestart()
        {
            return UInt16.MaxValue;
        }
    }
}
