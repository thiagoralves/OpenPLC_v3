
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
    public class PointRecord
    {
        public PointRecord(System.UInt16 index)
        {
            this.index = index;
        }

        public System.UInt16 index;
    }
    
    /// <summary>
    /// Point record type that is assigned an event class 
    /// </summary>
    public class EventRecord : PointRecord
    {
        public EventRecord(System.UInt16 index) : base(index)
        {
            this.clazz = PointClass.Class1;
        }

        public EventRecord(System.UInt16 index, PointClass clazz)
            : base(index)
        {
            this.clazz = clazz;            
        }        

        public PointClass clazz;        
    };

    /// <summary>
    /// Point record type that is assigned an event class and deadband tolerance
    /// </summary>
    public class DeadbandRecord<T> : EventRecord
    {
        public DeadbandRecord(System.UInt16 index, PointClass pointClass, T deadband) : base(index, pointClass)
        {
            this.deadband = deadband;
        }

        public DeadbandRecord(System.UInt16 index, PointClass pointClass) : base(index, pointClass)
        {
            this.deadband = default(T);
        }
        
        public DeadbandRecord(System.UInt16 index) : base(index)
        {
            this.deadband = default(T);
        }

        /// <summary>
        /// Value can change within this tolerance without producing an event
        /// </summary>
        public T deadband;
    };

    public class BinaryRecord : EventRecord
    {
        public BinaryRecord(System.UInt16 index) : base(index)
        {
            this.eventVariation = EventBinaryVariation.Group2Var1;
            this.staticVariation = StaticBinaryVariation.Group1Var2;         
        }

        public EventBinaryVariation eventVariation;
        public StaticBinaryVariation staticVariation;        
    };

    public class DoubleBinaryRecord : EventRecord
    {
        public DoubleBinaryRecord(System.UInt16 index) : base(index)
        {
            this.eventVariation = EventDoubleBinaryVariation.Group4Var1;
            this.staticVariation = StaticDoubleBinaryVariation.Group3Var2;
        }

        public EventDoubleBinaryVariation eventVariation;
        public StaticDoubleBinaryVariation staticVariation;
    };

    public class BinaryOutputStatusRecord : EventRecord
    {
        public BinaryOutputStatusRecord(System.UInt16 index) : base(index)
        {
            this.eventVariation = EventBinaryOutputStatusVariation.Group11Var1;
            this.staticVariation = StaticBinaryOutputStatusVariation.Group10Var2;
        }

        public EventBinaryOutputStatusVariation eventVariation;
        public StaticBinaryOutputStatusVariation staticVariation;
    };

    public class CounterRecord : DeadbandRecord<System.UInt32>
    {
        public CounterRecord(System.UInt16 index) : base(index)
        {
            this.eventVariation = EventCounterVariation.Group22Var1;
            this.staticVariation = StaticCounterVariation.Group20Var1;
        }

        public EventCounterVariation eventVariation;
        public StaticCounterVariation staticVariation;
    };

    public class FrozenCounterRecord : DeadbandRecord<System.UInt32>
    {
        public FrozenCounterRecord(System.UInt16 index) : base(index)
        {
            this.eventVariation = EventFrozenCounterVariation.Group23Var1;
            this.staticVariation = StaticFrozenCounterVariation.Group21Var1;
        }

        public EventFrozenCounterVariation eventVariation;
        public StaticFrozenCounterVariation staticVariation;
    };

    public class AnalogRecord : DeadbandRecord<double>
    {
        public AnalogRecord(System.UInt16 index) : base(index)
        {
            this.eventVariation = EventAnalogVariation.Group32Var1;
            this.staticVariation = StaticAnalogVariation.Group30Var1;
        }

        public EventAnalogVariation eventVariation;
        public StaticAnalogVariation staticVariation;
    };

    public class AnalogOutputStatusRecord : DeadbandRecord<double>
    {
        public AnalogOutputStatusRecord(System.UInt16 index) : base(index)
        {
            this.eventVariation = EventAnalogOutputStatusVariation.Group42Var1;
            this.staticVariation = StaticAnalogOutputStatusVariation.Group40Var1;
        }

        public EventAnalogOutputStatusVariation eventVariation;
        public StaticAnalogOutputStatusVariation staticVariation;
    };

    public class TimeAndIntervalRecord : PointRecord
    {
        public TimeAndIntervalRecord(System.UInt16 index) : base(index)
        {
            this.staticVariation = StaticTimeAndIntervalVariation.Group50Var4;
        }

        public StaticTimeAndIntervalVariation staticVariation;
    };
}
