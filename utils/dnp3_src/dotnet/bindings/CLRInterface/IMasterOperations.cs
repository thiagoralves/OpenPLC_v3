
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
    /// Interface representing a master station
    /// </summary>
    public interface IMasterOperations : ICommandProcessor
    {
        /// <summary>
        /// Retrieves stack statistic information
        /// </summary>
        /// <returns></returns>
        IStackStatistics GetStackStatistics();

        /// <summary>
        /// Set the log filters to a new value
        /// </summary>
        /// <param name="filters">A structure representing the new set of enabled filters</param>
        void SetLogFilters(LogFilter filters);

        /// <summary>
        /// Adds a periodic all objects scan that uses the 0x06 qualifier
        /// </summary>
        /// <param name="group"></param>
        /// <param name="variation"></param>
        /// <param name="period"></param>        
        /// <returns>a new master scan interface</returns>
        IMasterScan AddAllObjectsScan(byte group, byte variation, TimeSpan period, TaskConfig config);

        /// <summary>
        /// Adds a periodic class scan to the master
        /// </summary>
        /// <param name="field">bitfield of class values</param>
        /// <param name="period">period, negative for non-periodic</param>                
        /// <returns>a new master scan interface</returns>
        IMasterScan AddClassScan(ClassField field, TimeSpan period, TaskConfig config);

        /// <summary>        
        /// Adds a periodic range-based (start/stop) scan to the master
        /// </summary>        
        /// </summary>
        /// <param name="group"></param>
        /// <param name="variation"></param>
        /// <param name="start"></param>
        /// <param name="stop"></param>
        /// <param name="period"></param>        
        /// <returns>a new master scan interface</returns>
        IMasterScan AddRangeScan(byte group, byte variation, System.UInt16 start, System.UInt16 stop, TimeSpan period, TaskConfig config);

        /// <summary>
        /// Adds a custom scan to the master.
        /// </summary>                
        IMasterScan AddScan(IEnumerable<Header> headers, TimeSpan period, TaskConfig config);

        /// <summary>
        /// Perform an immediate adhoc scan that uses the 0x06 qualifier
        /// </summary>
        /// <param name="group"></param>
        /// <param name="variation"></param>                
        Task<TaskCompletion> ScanAllObjects(byte group, byte variation, TaskConfig config);

        /// <summary>
        /// Perform an immediate adhoc class scan
        /// </summary>                   
        Task<TaskCompletion> ScanClasses(ClassField field, TaskConfig config);

        /// <summary>
        /// Perform an immediate adhoc range-based (start/stop) scan
        /// </summary>        
        Task<TaskCompletion> ScanRange(byte group, byte variation, System.UInt16 start, System.UInt16 stop, TaskConfig config);

        /// <summary>
        /// Perform an adhoc scan with the designated request headers
        /// </summary>        
        Task<TaskCompletion> Scan(IEnumerable<Header> headers, TaskConfig config);

        /// <summary>
        /// Write a time and value object
        /// </summary>
        Task<TaskCompletion> Write(TimeAndInterval value, System.UInt16 index, TaskConfig config);

        /// <summary>
        /// Write a time and value object
        /// </summary>
        Task<RestartResultType> Restart(RestartType restartType, TaskConfig config);

        /// <summary>
        /// Perform an arbitrary function that doesn't expect a response
        /// </summary>        
        Task<TaskCompletion> PerformFunction(string name, FunctionCode func, IEnumerable<Header> headers, TaskConfig config);
    }
}
