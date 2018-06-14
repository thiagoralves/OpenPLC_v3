using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    public class X509Info
    {
        public X509Info(int depth, byte[] sha1fingerprint, string subjectName)
        {
            this.depth = depth;
            this.sha1fingerprint = sha1fingerprint;
            this.subjectName = subjectName;
        }

        readonly int depth;
        readonly byte[] sha1fingerprint;
        readonly string subjectName;

        public int Depth { get { return this.depth; } }
        public IEnumerable<byte> SHA1Fingerprint { get { return this.sha1fingerprint; } }
        public string SubjectName { get { return this.subjectName; } }
    }
}
