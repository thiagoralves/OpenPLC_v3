
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
    /// Aggregate result of command operation.  Check the Result before checking the status.
    /// Status is only valid when Result == RESPONSE_OK
    /// </summary>
    public class CommandPointResult
    {
        public CommandPointResult(UInt16 headerIndex, UInt16 requestIndex, CommandPointState state, CommandStatus status)
        {
            this.headerIndex = headerIndex;
            this.requestIndex = requestIndex;
            this.state = state;
            this.status = status;
        }

        public UInt16 HeaderIndex
        {
            get { return headerIndex; }
        }

        public UInt16 RequestIndex
        {
            get { return RequestIndex; }
        }

        public CommandPointState PointState
        {
            get { return state; }
        }

        public CommandStatus Status
        {
            get { return status; }
        }

        public override string ToString() 
        {
            return String.Format("Header: {0} Index: {1} State: {2} Status: {3}", headerIndex, requestIndex, state, status);
        }

        readonly UInt16 headerIndex;
        readonly UInt16 requestIndex;
        readonly CommandPointState state;
        readonly CommandStatus status;
    }
    
    /// <summary>
    /// Result type returned for CommandSet based command requests
    /// </summary>
    public class CommandTaskResult
    {
        public CommandTaskResult(TaskCompletion summary, IEnumerable<CommandPointResult> results)
        {
            this.summary = summary;
            this.results = results;
        }

        /// <summary>
        /// Describes the result of the task as a whole
        /// </summary>
        public TaskCompletion TaskSummary
        {
            get { return summary; }
        }

        public IEnumerable<CommandPointResult> Results
        {
            get { return results; }
        }

        public override string ToString()
        {
            return String.Format("Summary: {0} Results: [{1}]", summary, String.Join("; ", results));
        }

        readonly TaskCompletion summary;
        readonly IEnumerable<CommandPointResult> results;
    }   
}
