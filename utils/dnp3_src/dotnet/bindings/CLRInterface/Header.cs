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
    /// Represents a header that can be used to specifiy various types of master tasks
    /// </summary>
    public class Header
    {
        public readonly Byte group;
        public readonly Byte variation;
        public readonly QualifierCode qualifier;        

        protected Header(Byte group, Byte variation, QualifierCode qualifier)
        {
            this.group = group;
            this.variation = variation;
            this.qualifier = qualifier;
        }

        public override string ToString()
        {
            return String.Format("g{0}v{1} - {2}", group, variation, qualifier);
        }

        /// <summary>
        /// Header requesting all objects of the given type or class (in the case of Group 60 objects) are returned.
        /// </summary>
        /// <param name="groupVariation">Group and Variation being requested.</param>
        /// <returns>A Header with Qualifier 0x06.</returns>
        public static Header AllObjects(Byte group, Byte variation)
        {
            return new Header(group, variation, QualifierCode.ALL_OBJECTS);
        }

        /// <summary>
        /// Header requesting that the first "x" indexes are returned, where x is the quantity.
        /// </summary>
        /// <param name="groupVariation">Group and Variation being requested.</param>
        /// <param name="quantity">The number of objects to return.</param>
        /// <returns>A Header with Qualifier 0x07.</returns>
        public static Header Count8(Byte group, Byte variation, Byte quantity)
        {
            return new CountHeader(group, variation, QualifierCode.UINT8_CNT, Convert.ToUInt16(quantity));
        }

        /// <summary>
        /// Header requesting that the first "x" indexes are returned, where x is the quantity.
        /// </summary>
        /// <param name="groupVariation">Group and Variation being requested.</param>
        /// <param name="quantity">The number of objects to return.</param>
        /// <returns>A Header with Qualifier 0x08.</returns>
        public static Header Count16(Byte group, Byte variation, UInt16 quantity)
        {
            return new CountHeader(group, variation, QualifierCode.UINT16_CNT, quantity);
        }        

        /// <summary>
        /// Similar to a Count Header except both the start and stop indexes are provided.
        /// </summary>
        /// <param name="groupVariation">Group and Variation being requested.</param>
        /// <param name="startIndex">The index to start at.</param>
        /// <param name="stopIndex">The index to stop at.</param>
        /// <returns>A Header with Qualifier 0x00</returns>
        public static Header Range8(Byte group, Byte variation, Byte startIndex, Byte stopIndex)
        {
            return new RangeHeader(group, variation, QualifierCode.UINT8_START_STOP, Convert.ToUInt16(startIndex), Convert.ToUInt16(stopIndex));
        }

        /// <summary>
        /// Similar to a Count Header except both the start and stop indexes are provided.
        /// </summary>
        /// <param name="groupVariation">Group and Variation being requested.</param>
        /// <param name="startIndex">The index to start at.</param>
        /// <param name="stopIndex">The index to stop at.</param>
        /// <returns>A Header with Qualifier 0x01</returns>
        public static Header Range16(Byte group, Byte variation, UInt16 startIndex, UInt16 stopIndex)
        {
            return new RangeHeader(group, variation, QualifierCode.UINT16_START_STOP, startIndex, stopIndex);
        }
    }

    public class CountHeader : Header
    {
        public readonly UInt16 count;

        internal CountHeader(Byte group, Byte variation, QualifierCode qualifierCode, UInt16 count)
            : base(group, variation, qualifierCode)
        {
            this.count = count;
        }

        public override string ToString()
        {
            return String.Format("{0} - count: {1}", base.ToString(), count);
        }
    }   

    public class RangeHeader : Header
    {
        public readonly UInt16 start;
        public readonly UInt16 stop;

        internal RangeHeader(Byte group, Byte variation, QualifierCode qualifier, UInt16 start, UInt16 stop)
            : base(group, variation, qualifier)
        {
            this.start = start;
            this.stop = stop;
        }

        public override string ToString()
        {
            return String.Format("{0} - start: {1} stop {2}", base.ToString(), start, stop);
        }
    }
}
