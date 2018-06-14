package com.automatak.dnp3;

/**
 * Statistics for the link layer parser
 */
public class ParserStatistics
{
    public ParserStatistics(
            long numHeaderCrcError,
            long numBodyCrcError,
            long numLinkFrameRx,
            long numBadLength,
            long numBadFunctionCode,
            long numBadFCV,
            long numBadFCB
    )
    {
        this.numHeaderCrcError = numHeaderCrcError;
        this.numBodyCrcError = numBodyCrcError;
        this.numLinkFrameRx = numLinkFrameRx;
        this.numBadLength = numBadLength;
        this.numBadFunctionCode = numBadFunctionCode;
        this.numBadFCV = numBadFCV;
        this.numBadFCB = numBadFCB;
    }

    /**
     * Number of frames discarded due to header CRC errors
     */
    public final long numHeaderCrcError;

    /**
     * Number of frames discarded due to body CRC errors
     */
    public final long numBodyCrcError;

    /**
     * Number of frames received
     */
    public final long numLinkFrameRx;

    /**
     * number of bad LEN fields received (malformed frame)
     */
    public final long numBadLength;

    /**
     * number of bad function codes (malformed frame)
     */
    public final long numBadFunctionCode;

    /**
     * number of FCV / function code mismatches (malformed frame)
     */
    public final long numBadFCV;

    /**
     * number of frames w/ unexpected FCB bit set (malformed frame)
     */
    public final long numBadFCB;
}
