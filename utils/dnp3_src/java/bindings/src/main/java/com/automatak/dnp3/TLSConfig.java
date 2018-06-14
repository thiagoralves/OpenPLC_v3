package com.automatak.dnp3;

/**
 * TLS configuration information
 */
public class TLSConfig
{
    /**
     * Construct a TLS configuration
     *
     * @param peerCertFilePath Certificate file used to verify the peer or server. Can be CA file or a self-signed cert provided by other party.
     * @param localCertFilePath File that contains the certificate (or certificate chain) that will be presented to the remote side of the connection
     * @param privateKeyFilePath File that contains the private key corresponding to the local certificate
     * @param maxVerifyDepth The maximum certificate chain verification depth (0 == self-signed only)
     * @param allowTLSv10 Allow TLS version 1.0 (default false)
     * @param allowTLSv11 Allow TLS version 1.1 (default false)
     * @param allowTLSv12 Allow TLS version 1.2 (default true)
     * @param cipherList The openssl cipher-list, defaults to "" which does not modify the default cipher list
     *
     * localCertFilePath and privateKeyFilePath can optionally be the same file, i.e. a PEM that contains both pieces of data.
     *
     */
    public TLSConfig(
	    String peerCertFilePath,
	    String localCertFilePath,
	    String privateKeyFilePath,
        int maxVerifyDepth,
        boolean allowTLSv10,
        boolean allowTLSv11,
        boolean allowTLSv12,
	    String cipherList
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

    /// Certificate file used to verify the peer or server. Can be CA file or a self-signed cert provided by other party.
    public final String peerCertFilePath;

    /// File that contains the certificate (or certificate chain) that will be presented to the remote side of the connection
    public final String localCertFilePath;

    /// File that contains the private key corresponding to the local certificate
    public final String privateKeyFilePath;

    /// max verification depth (defaults to 0 - peer certificate only)
    public final int maxVerifyDepth;

    /// Allow TLS version 1.0 (default false)
    public final boolean allowTLSv10;

    /// Allow TLS version 1.1 (default false)
    public final boolean allowTLSv11;

    /// Allow TLS version 1.2 (default true)
    public final boolean allowTLSv12;

    /// openssl format cipher list
    public final String cipherList;

};
