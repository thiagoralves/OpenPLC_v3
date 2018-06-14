using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Represent a start-stop range
    /// </summary>
    public struct Range
    {        
        public static Range From(UInt16 start, UInt16 stop)
        {
            return new Range(start, stop);
        }

        public static Range All()
        {
            return new Range(0, UInt16.MaxValue);
        }

        public bool IsDefined()
        {
            return start <= stop;
        }

        public bool IsAllObjects()
        {
            return start == 0 && stop == UInt16.MaxValue;
        }

        Range(UInt16 start, UInt16 stop)
        {
            this.start = start;
            this.stop = stop;
        }

        public UInt16 start;
        public UInt16 stop;     
    }

    public struct ClassAssignment
    {
        public ClassAssignment(byte group, byte variation, PointClass clazz, Range range)
        {
            this.group = group;
            this.variation = variation;
            this.clazz = clazz;
            this.range = range;
        }

        public byte group;
        public byte variation;
        public PointClass clazz;
        
        // if range is full, this implies 0x06
        public Range range;
    }
}
