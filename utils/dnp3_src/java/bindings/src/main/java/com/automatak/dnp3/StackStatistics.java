package com.automatak.dnp3;

/**
 * Combined statistics for an outstation or master stack
 */
public class StackStatistics
{
    public StackStatistics(
        LinkLayerStatistics link,
        TransportStatistics transport
    )
    {
        this.link = link;
        this.transport = transport;
    }

    public final LinkLayerStatistics link;
    public final TransportStatistics transport;
}

