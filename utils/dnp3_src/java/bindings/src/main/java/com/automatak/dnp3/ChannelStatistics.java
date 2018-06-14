package com.automatak.dnp3;

public class ChannelStatistics
{
    public ChannelStatistics(
        long numOpen,
        long numOpenFail,
        long numClose,
        long numBytesRx,
        long numBytesTx,
        long numLinkFrameTx
    )
    {
        this.numOpen = numOpen;
        this.numOpenFail = numOpenFail;
        this.numClose = numClose;
        this.numBytesRx = numBytesRx;
        this.numBytesTx = numBytesTx;
        this.numLinkFrameTx = numLinkFrameTx;
    }

    /// The number of times the channel has successfully opened
    public final long numOpen;

    /// The number of times the channel has failed to open
    public final long numOpenFail;

    /// The number of times the channel has closed either due to user intervention or an error
    public final long numClose;

    /// The number of bytes received
    public final long numBytesRx;

    /// The number of bytes transmitted
    public final long numBytesTx;

    /// Number of frames transmitted
    public final long numLinkFrameTx;
}
