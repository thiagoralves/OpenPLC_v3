package com.automatak.dnp3;

/**
 * Statistics for the link-layer
 */
public class LinkLayerStatistics {

    public LinkLayerStatistics(
        long numUnexpectedFrame,
        long numBadMasterBit,
        long numUnknownDestination,
        long numUnknownSource)
    {
        this.numUnexpectedFrame = numUnexpectedFrame;
        this.numBadMasterBit = numBadMasterBit;
        this.numUnknownDestination = numUnknownDestination;
        this.numUnknownSource = numUnknownSource;
    }

    /**
     * number of unexpected frames
     */
    public final long numUnexpectedFrame;

    /**
     * frames received w/ wrong master bit
     */
    public final long numBadMasterBit;

    /**
     * frames received for an unknown destination
     */
    public final long numUnknownDestination;

    /**
     * frames received for an unknown source
     */
    public final long numUnknownSource;
}
