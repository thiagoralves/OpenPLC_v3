package com.automatak.dnp3;

/**
 * Maps to g50v1
 */
public class DNPTime {

    public DNPTime(long msSinceEpoch)
    {
        this.msSinceEpoch = msSinceEpoch;
    }

    public final long msSinceEpoch;

    @Override
    public String toString()
    {
        return String.format("DNPTime(%d)", msSinceEpoch);
    }
}
