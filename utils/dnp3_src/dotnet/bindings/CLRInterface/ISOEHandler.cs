
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
    public static class IndexedValue
    { 
        public static IndexedValue<T> From<T>(T value, System.UInt16 index)
        {
            return new IndexedValue<T>(value, index);
        }
    }

    /// <summary>
    /// A measurement value and index pair
    /// </summary>
    /// <typeparam name="T">Type of the measurement</typeparam>
    public class IndexedValue<T>
    {
        public IndexedValue(T value, System.UInt16 index)
        {
            this.value = value;
            this.index = index;
        }

        private readonly T value;
        private readonly System.UInt16 index;

        public T Value
        {
            get { return value;  }
        }

        public UInt16 Index
        {
            get { return index; }
        }
    }

    public class HeaderInfo
    {
        public HeaderInfo(GroupVariation variation, QualifierCode qualifier, TimestampMode tsmode, bool isEvent, bool flagsValid, UInt32 headerIndex)
        {
            this.variation = variation;
            this.qualifier = qualifier;
            this.tsmode = tsmode;
            this.isEvent = isEvent;
            this.flagsValid = flagsValid;
            this.headerIndex = headerIndex;
        }
        
        /// <summary>
        /// An enum representing the group and variation for this header
        /// </summary>
        public readonly GroupVariation variation;

        /// <summary>
        /// The qualifier code used for this header
        /// </summary>
        public readonly QualifierCode qualifier;

        /// <summary>
        /// An enumeration describing the validity of the timestamp on the measurements
        /// </summary>
        public readonly TimestampMode tsmode;

        /// <summary>
        /// True if this qualifier is an event type
        /// </summary>
        public readonly bool isEvent;
        
        /// <summary>
        /// True if the flags/quality on the measurements are valid
        /// </summary>
        public readonly bool flagsValid;

        /// <summary>
        /// The index of the header (0-based) within the ASDU
        /// </summary>
        public readonly UInt32 headerIndex;
    }

    /// <summary>
    /// Interface called to receive measurement callbacks from the master
    /// </summary>
    public interface ISOEHandler
	{
            void Start();
            void End();

            void Process(HeaderInfo info, IEnumerable<IndexedValue<Binary>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<DoubleBitBinary>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<Analog>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<Counter>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<FrozenCounter>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<BinaryOutputStatus>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<AnalogOutputStatus>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<OctetString>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<TimeAndInterval>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<BinaryCommandEvent>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<AnalogCommandEvent>> values);
            void Process(HeaderInfo info, IEnumerable<IndexedValue<SecurityStat>> values);
	}
}
