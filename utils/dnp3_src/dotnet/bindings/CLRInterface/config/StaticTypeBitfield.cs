using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// A bitfield representing a subset of all static types
    /// </summary>
    public struct StaticTypeBitField
    {	    
	    public StaticTypeBitField(UInt16 mask)
	    {
            this.mask = mask;
        }

        public StaticTypeBitField(params StaticTypeBitmask[] masks)
        {
            this.mask = 0;
            foreach (var bit in masks)
            {
                mask |= Convert.ToUInt16(bit);
            }
        }

        public bool IsSet(StaticTypeBitmask type)
	    {
		    return (mask & Convert.ToUInt16(type)) != 0;
	    }

        public static StaticTypeBitField AllTypes()
	    {		
		    return new StaticTypeBitField(~0 & 0xFFFF);
	    }

	    public readonly UInt16 mask;
    };
}
