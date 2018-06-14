
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
    /// Result type returned for CommandSet based command requests
    /// </summary>
    public class RestartResultType
    {
        public RestartResultType(TaskCompletion summary, TimeSpan restartTime)
        {
            this.summary = summary;
            this.restartTime = restartTime;
        }        

        public override string ToString()
        {
            return String.Format("Summary: {0} Milliseconds: {1}", summary, restartTime.TotalMilliseconds);
        }

        /// <summary>
        /// Summary value for the task
        /// </summary>
        public readonly TaskCompletion summary;

        /// <summary>
        /// The time-to-restart sent from the outstation
        /// </summary>
        public readonly TimeSpan restartTime;
    }   
}
