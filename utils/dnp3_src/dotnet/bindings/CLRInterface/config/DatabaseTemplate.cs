
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
    /// Defines the database layout for an outstation
    /// </summary>
    public class DatabaseTemplate
    {
        /// <summary>
        /// Constructor that sets up the size of the types
        /// </summary>
        /// <param name="numBinary">numer of binary values starting at index 0</param>
        /// <param name="numAnalog">numer of analog values starting at index 0</param>
        /// <param name="numCounter">numer of counter values starting at index 0</param>
        /// <param name="numCounter">numer of frozen counter values starting at index 0</param>
        /// <param name="numBinaryOutputStatus">numer of control status values starting at index 0</param>
        /// <param name="numAnalogOutputStatus">numer of setpoint status values starting at index 0</param>
        /// /// <param name="numTimeAndInterval">numer of TimeAndInterval values starting at index 0</param>
        public DatabaseTemplate(  System.UInt16 numBinary,
                                  System.UInt16 numDoubleBinary,
                                  System.UInt16 numAnalog,
                                  System.UInt16 numCounter,
                                  System.UInt16 numFrozenCounter,
                                  System.UInt16 numBinaryOutputStatus,
                                  System.UInt16 numAnalogOutputStatus,
                                  System.UInt16 numTimeAndInterval)
        {
            this.binaries = Enumerable.Range(0, numBinary).Select(i => new BinaryRecord(Convert.ToUInt16(i))).ToList();

            this.doubleBinaries = Enumerable.Range(0, numDoubleBinary).Select(i => new DoubleBinaryRecord(Convert.ToUInt16(i))).ToList();

            this.counters = Enumerable.Range(0, numCounter).Select(i => new CounterRecord(Convert.ToUInt16(i))).ToList();

            this.frozenCounters = Enumerable.Range(0, numFrozenCounter).Select(i => new FrozenCounterRecord(Convert.ToUInt16(i))).ToList();

            this.analogs = Enumerable.Range(0, numAnalog).Select(i => new AnalogRecord(Convert.ToUInt16(i))).ToList();

            this.binaryOutputStatii = Enumerable.Range(0, numBinaryOutputStatus).Select(i => new BinaryOutputStatusRecord(Convert.ToUInt16(i))).ToList();

            this.analogOutputStatii = Enumerable.Range(0, numAnalogOutputStatus).Select(i => new AnalogOutputStatusRecord(Convert.ToUInt16(i))).ToList();

            this.timeAndIntervals = Enumerable.Range(0, numTimeAndInterval).Select(i => new TimeAndIntervalRecord(Convert.ToUInt16(i))).ToList();
        }

        /// <summary>
        /// Default constructor that sets every value to the same count
        /// </summary>
        public DatabaseTemplate(System.UInt16 count) : this(count, count, count, count, count, count, count, count)
        { }

        /// <summary>
        /// Default constructor that sets up 10 of every type
        /// </summary>
        public DatabaseTemplate() : this(0)
        { }

        public IndexMode GetIndexMode()
        {
            var lists = new Tuple<IReadOnlyList<PointRecord>, string>[] {
                CreateTuple(binaries, "binary"),
                CreateTuple(doubleBinaries, "analog"),
                CreateTuple(counters, "counter"),
                CreateTuple(frozenCounters, "frozen counter"),
                CreateTuple(analogs, "analog"),
                CreateTuple(binaryOutputStatii, "binary output status"),
                CreateTuple(analogOutputStatii, "analog output status"),
                CreateTuple(timeAndIntervals, "time and interval")
            };

            // this can throw an exception if any of the sub lists have bad discontiguous indices
            var allContiguous = lists.Aggregate(true, (sum, pair) => sum && IsContiguous(pair.Item1, pair.Item2));

            return allContiguous ? IndexMode.Contiguous : IndexMode.Discontiguous;
        }

        /// <summary>
        /// Modify individual binary configuration here
        /// </summary>
        public IReadOnlyList<BinaryRecord> binaries;
        /// <summary>
        /// Modify individual double binary configuration here
        /// </summary>
        public IReadOnlyList<DoubleBinaryRecord> doubleBinaries;
        /// <summary>
        /// Modify individual analog configuration here
        /// </summary>
        public IReadOnlyList<CounterRecord> counters;
        /// <summary>
        /// Modify individual analog configuration here
        /// </summary>
        public IReadOnlyList<FrozenCounterRecord> frozenCounters;
        /// <summary>
        /// Modify individual counter configuration here
        /// </summary>
        public IReadOnlyList<AnalogRecord> analogs;
        /// <summary>
        /// Modify individual binary output status configuration here
        /// </summary>
        public IReadOnlyList<BinaryOutputStatusRecord> binaryOutputStatii;
        /// <summary>
        /// Modify individual analog output status configuration here
        /// </summary>
        public IReadOnlyList<AnalogOutputStatusRecord> analogOutputStatii;
        /// <summary>
        ///  Modify individual time and interval configuration here
        /// </summary>
        public IReadOnlyList<TimeAndIntervalRecord> timeAndIntervals;

        private Tuple<IReadOnlyList<PointRecord>, string> CreateTuple(IReadOnlyList<PointRecord> list, string name)
        {
            return Tuple.Create(list, name);
        }

        private static bool IsContiguous(IReadOnlyList<PointRecord> records, string name)
        {                       
            int lastIndex = -1;             // will always detect non-zero based indices (even if contiguous)
            bool contiguous = true;         // empty collections will always be contiguous
            System.UInt16 rawIndex = 0;
            
            foreach (var rec in records)
            {
                // this condition must not be violated for either mode
                if (rec.index <= lastIndex)
                {
                    throw new ArgumentException(String.Format("Non-increasing index {0} found at raw index {1} for type {2}", rec.index, rawIndex, name));
                }
                                
                if(rec.index != (lastIndex + 1))
                {
                    contiguous = false;
                }                        

                lastIndex = rec.index;
                ++rawIndex;
            }

            return contiguous;
        }
    };    
}
