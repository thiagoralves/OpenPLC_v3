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
    /// Constrains the time range used for exponential backoff from some minimum to a maximum
    /// </summary>
    public class ChannelRetry
    {
        /// <summary>
        /// Construct a channel retry class
        /// </summary>
        /// <param name="minRetryDelay">the minimum (first) retry delay</param>
        /// <param name="maxRetryDelay">the maximum delay</param>
        public ChannelRetry(TimeSpan minRetryDelay, TimeSpan maxRetryDelay)
        {
            this.minRetryDelay = minRetryDelay;
            this.maxRetryDelay = maxRetryDelay;
        }

        /// <summary>
        /// Default configuration from 1 second to 1 minute
        /// </summary>
        public static ChannelRetry Default
        {
            get
            {
                return new ChannelRetry(TimeSpan.FromSeconds(1), TimeSpan.FromMinutes(1));
            }
        }

        public readonly TimeSpan minRetryDelay;
        public readonly TimeSpan maxRetryDelay;
    }
}
