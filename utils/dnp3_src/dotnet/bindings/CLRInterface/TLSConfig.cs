using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    public class TLSConfig
    {     
        /// <summary>
        /// Construct a TLS configuration. All files are in PEM format.	    
        /// </summary>
        /// <param name="peerCertFilePath">Certificate file used to verify the server. Can be CA file or a self - signed cert provided by other party</param>
        /// <param name="localCertFilePath">File that contains the certificate that will be presented to the remote side of the connection</param>
        /// <param name="privateKeyFilePath">File that contains the private key corresponding to the local certificate</param>
        /// <param name="maxVerifyDepth">Maximum certificate verify depth (defaults 0, self signed)</param>
        /// <param name="allowTLSv10">Allow TLS version 1.0 (default true)</param>
        /// <param name="allowTLSv11">Allow TLS version 1.1 (default true)</param>
        /// <param name="allowTLSv12">Allow TLS version 1.2 (default true)</param>
        /// <param name="cipherList">List of ciphers in openssl's format. Defaults to empty string which will use the default cipher list</param>
        
        public TLSConfig(
            string peerCertFilePath,
            string localCertFilePath,
            string privateKeyFilePath,
            int maxVerifyDepth = 0,
            bool allowTLSv10 = false,
            bool allowTLSv11 = false,
            bool allowTLSv12 = true,
            string cipherList = ""
        )
        {
            this.peerCertFilePath = peerCertFilePath;
            this.localCertFilePath = localCertFilePath;
            this.privateKeyFilePath = privateKeyFilePath;
            this.maxVerifyDepth = maxVerifyDepth;
            this.allowTLSv10 = allowTLSv10;
            this.allowTLSv11 = allowTLSv11;
            this.allowTLSv12 = allowTLSv12;
            this.cipherList = cipherList;            
        }

        public readonly string peerCertFilePath;
        public readonly string localCertFilePath;
        public readonly string privateKeyFilePath;
        
        public readonly int maxVerifyDepth;

        /// Allow TLS version 1.0 (default false)
        public readonly bool allowTLSv10;

        /// Allow TLS version 1.1 (default false)
        public readonly bool allowTLSv11;

        /// Allow TLS version 1.2 (default true)
        public readonly bool allowTLSv12;

        public readonly string cipherList;
    }
}
