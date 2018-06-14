
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
    public static class Flags
    {
        public const byte ONLINE = 0x01;
        public const byte RESTART = 0x02;

        const byte DoubleValueMask = 0xC0;
        const byte DoubleQualityMask = 0x3F;

        public static byte FromBinaryValue(bool value, byte quality)
        {
            if (value)
            {
                return (byte) (quality | ((byte) BinaryQuality.STATE));
            }
            else
            { 
                return (byte) (quality & ~((byte) BinaryQuality.STATE));
            }
        }

        public static bool ToBinaryValue(byte quality)
        {
            return (quality & ((byte)BinaryQuality.STATE)) != 0;
        }

        public static byte FromDoubleBitValue(byte quality, DoubleBit state)
        {
	        byte value = (byte) (((byte)(state)) << 6);
            return (byte) ((DoubleQualityMask & quality) | value);
        }

        public static DoubleBit ToDoubleBitValue(byte quality)
        {
	        // the value is the top 2 bits, so downshift 6
	        byte value = (byte) ((quality >> 6) & 0xFF);
	        return (DoubleBit) value;
        }
      
    }

    /// <summary>
    /// A boolean measurement type (i.e. whether a switch is open/closed)
    /// </summary>
    public class Binary : TypedMeasurementBase<bool> 
    {		
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        /// <param name="time">timestamp</param>
	    public Binary(bool value, byte quality, DateTime time) :
            base(value, Flags.FromBinaryValue(value, quality), time)
	    {}

        /// <summary>
        /// Constructor without a timestamp
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        public Binary(bool value, byte quality) :
            base(value, Flags.FromBinaryValue(value, quality))
        {}

        /// <summary>
        /// Constructor with only quality
        /// </summary>
        /// <param name="quality"></param>
        public Binary(byte quality) : base(Flags.ToBinaryValue(quality), quality)
        { }       
    }

    /// <summary>
    /// A boolean measurement type (i.e. whether a switch is open/closed)
    /// </summary>
    public class DoubleBitBinary : TypedMeasurementBase<DoubleBit> 
    {		
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        /// <param name="time">timestamp</param>
	    public DoubleBitBinary(DoubleBit value, byte quality, DateTime time) : 
            base(value, Flags.FromDoubleBitValue(quality, value), time)
	    {}

        /// <summary>
        /// Constructor without a timestamp
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        public DoubleBitBinary(DoubleBit value, byte quality) :
            base(value, Flags.FromDoubleBitValue(quality, value))
        {}

        /// <summary>
        /// Constructor with only a quality
        /// </summary>
        /// <param name="value"></param>
        /// <param name="quality"></param>
        /// <param name="time"></param>
        public DoubleBitBinary(byte quality)
            : base(Flags.ToDoubleBitValue(quality), quality)
        { }
    }

    /// <summary>
    /// A discrete or continuous analog value
    /// </summary>
    public class Analog : TypedMeasurementBase<double>
    {
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        /// <param name="time">timestamp</param>
        public Analog(double value, byte quality, DateTime time)
            : base(value, quality, time)
	    {}

        /// <summary>
        /// Constructor without a timestamp
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        public Analog(double value, byte quality) : base(value, quality)
        {}

        /// <summary>
        /// Constructor with only quality
        /// </summary>
        /// <param name="quality"></param>
        public Analog(byte quality) : base(0.0, quality)
        { }
    }

    /// <summary>
    /// An increasing count of some event
    /// </summary>
    public class Counter : TypedMeasurementBase<System.UInt32>
    {

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        /// <param name="time">timestamp</param>
        public Counter(System.UInt32 value, byte quality, DateTime time): base(value, quality, time)
	    {}

        /// <summary>
        /// Constructor without a timestamp
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        public Counter(System.UInt32 value, byte quality) : base(value, quality)
        {}

        /// <summary>
        /// Constructor with only quality
        /// </summary>
        /// <param name="quality"></param>
        public Counter(byte quality) : base(0, quality)
        { }
    }

    /// <summary>
    /// An increasing count of some event frozen in time
    /// </summary>
    public class FrozenCounter : TypedMeasurementBase<System.UInt32>
    {

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        /// <param name="time">timestamp</param>
        public FrozenCounter(System.UInt32 value, byte quality, DateTime time): base(value, quality, time)
	    {}

        /// <summary>
        /// Constructor without a timestamp
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        public FrozenCounter(System.UInt32 value, byte quality) : base(value, quality)
        {}

        /// <summary>
        /// Constructor with only quality
        /// </summary>
        /// <param name="quality"></param>
        public FrozenCounter(byte quality) : base(0, quality)
        { }
    }

    /// <summary>
    /// Represents the state of a digital output
    /// </summary>
    public class BinaryOutputStatus : TypedMeasurementBase<bool>
    {

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        /// <param name="time">timestamp</param>
	    public BinaryOutputStatus(bool value, byte quality, DateTime time) :
            base(value, Flags.FromBinaryValue(value, quality), time)
	    {}

        /// <summary>
        /// Constructor without a timestamp
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        public BinaryOutputStatus(bool value, byte quality) :
            base(value, Flags.FromBinaryValue(value, quality))
        {}

        /// <summary>
        /// Constructor with only quality
        /// </summary>
        /// <param name="quality"></param>
        public BinaryOutputStatus(byte quality)
            : base(Flags.ToBinaryValue(quality), quality)
        { }
    };

    /// <summary>
    /// Represents the status of analog output on an outstation.
    /// </summary>    
    public class AnalogOutputStatus : TypedMeasurementBase<double>
    {

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        /// <param name="time">timestamp</param>
        public AnalogOutputStatus(double value, byte quality, DateTime time) : base(value, quality, time)
	    {}

        /// <summary>
        /// Constructor without a timestamp
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        public AnalogOutputStatus(double value, byte quality): base(value, quality)
        {}

         /// <summary>
        /// Constructor with only quality
        /// </summary>
        /// <param name="quality"></param>
        public AnalogOutputStatus(byte quality) : base(0.0, quality)
        { }
    }

    public class OctetData
    {
        public OctetData(byte[] bytes)
        {
            if (bytes.Length > 255) throw new ArgumentException("byte array cannot exceed length of 255", "bytes");
            this.bytes = bytes;
        }

        public String AsString()
        {
            return new String(Encoding.ASCII.GetChars(bytes));
        }

        public byte[] Bytes
        {
            get { return bytes; }
        }

        private readonly byte[] bytes;    
    }

    public class OctetString : OctetData
    {
        public OctetString(byte[] bytes) : base(bytes)
        { }
    }

    public class TimeAndInterval
    {
        public TimeAndInterval(UInt64 time, UInt32 interval, Byte units)
        {
            this.time = time;
            this.interval = interval;
            this.units = units;
        }

        public TimeAndInterval(UInt64 time, UInt32 interval, IntervalUnits units)
            : this(time, interval, (Byte)units)
        {}

        IntervalUnits GetUnitsEnum()
        {
            if (Enum.IsDefined(typeof(IntervalUnits), units))
            {
                return (IntervalUnits)Enum.ToObject(typeof(IntervalUnits), units);
            }
            else
            {
                return IntervalUnits.Undefined;
            }
        }

        public override string ToString()
        {
            return String.Format("time: {0} interval: {1} units: {2}", time, interval, units);
        }

        public readonly UInt64 time;
        public readonly UInt32 interval;
        public readonly Byte units;	
    }

    /// <summary>
    /// Represents the status of analog output on an outstation.
    /// </summary>    
    public class SecurityStat : TypedMeasurementBase<System.UInt32>
    {

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="value">value of the measurement</param>
        /// <param name="quality">quality enumeration as a bitfield</param>
        /// <param name="time">timestamp</param>
        public SecurityStat(uint value, ushort assocId, byte quality, DateTime time)
            : base(value, quality, time)
        {
            this.assocId = assocId;
        }        

        UInt16 AssociationId
        {
            get
            {
                return assocId;
            }
        }

        private readonly UInt16 assocId;
    }

    

    public class BinaryCommandEvent
    {
        public BinaryCommandEvent(bool value, CommandStatus status, DateTime time)
        {
            this.value = value;
            this.status = status;
            this.time = time;
        }

        public override string ToString()
        {
            return String.Format("value: {0} status: {1} time: {2}", value, status, time);
        }

        private readonly bool value;
        private readonly CommandStatus status;
        private readonly DateTime time;

        public bool Value { get { return this.value; } }
        public CommandStatus Status { get { return this.status; } }
        public DateTime Timestamp { get { return this.time; } }
    }

    public class AnalogCommandEvent
    {
        public AnalogCommandEvent(double value, CommandStatus status, DateTime time)
        {
            this.value = value;
            this.status = status;
            this.time = time;
        }

        public override string ToString()
        {
            return String.Format("value: {0} status: {1} time: {2}", value, status, time);
        }

        private readonly double value;
        private readonly CommandStatus status;
        private readonly DateTime time;

        public double Value { get { return this.value; } }
        public CommandStatus Status { get { return this.status; } }
        public DateTime Timestamp { get { return this.time; } }
    }
}
