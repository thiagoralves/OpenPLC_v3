using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Structure that records which events are scanned / evented
    /// </summary>
    [Serializable]
    public struct ClassField
    {       
        public ClassField(byte mask)
        {
            this.bitfield =  mask;
        }

        public static ClassField None
        {
            get
            {
                return new ClassField();
            }
        }

        public static ClassField From(params PointClass[] classes)
        {
            byte mask = 0;
            foreach(var pc in classes)
            {
                mask |= (byte)pc;
            }
            return new ClassField(mask);
        }

        public static ClassField AllClasses
        {
            get
            {
                return ClassField.From(PointClass.Class0, PointClass.Class1, PointClass.Class2, PointClass.Class3);
            }
        }

        public static ClassField AllEventClasses
        {
            get
            {
                return ClassField.From(PointClass.Class1, PointClass.Class2, PointClass.Class3);
            }
        }

        public byte ClassMask
        {
            get
            {
                return bitfield;
            }
        }

        [XmlElement("Class0")]
        public  bool Class0
        {
            get
            {
                return (((byte)PointClass.Class0) & bitfield) != 0;
            }
            set
            {
                if (value)
                {
                    bitfield |= (byte) PointClass.Class0;
                }
                else
                {
                    bitfield &= (~(byte)(PointClass.Class0)) & 0xFF;
                }
            }
        }

        [XmlElement("Class1")]
        public bool Class1
        {
            get
            {
                return (((byte)PointClass.Class1) & bitfield) != 0;
            }
            set
            {
                if (value)
                {
                    bitfield |= (byte)PointClass.Class1;
                }
                else
                {
                    bitfield &= (~(byte)(PointClass.Class1)) & 0xFF;
                }
            }
        }

        [XmlElement("Class2")]
        public bool Class2
        {
            get
            {
                return (((byte)PointClass.Class2) & bitfield) != 0;
            }
            set
            {
                if (value)
                {
                    bitfield |= (byte)PointClass.Class2;
                }
                else
                {
                    bitfield &= (~(byte)(PointClass.Class2)) & 0xFF;
                }
            }
        }

        [XmlElement("Class3")]
        public bool Class3
        {
            get
            {
                return (((byte)PointClass.Class3) & bitfield) != 0;
            }
            set
            {
                if (value)
                {
                    bitfield |= (byte)PointClass.Class3;
                }
                else
                {
                    bitfield &= (~(byte)(PointClass.Class3)) & 0xFF;
                }
            }
        }

        [XmlIgnore]
        private byte bitfield;
    }   
}
