package com.automatak.dnp3;

public class TransportStatistics
{
    public TransportStatistics(
        long numTransportRx,
        long numTransportTx,
        long numTransportErrorRx,
        long numTransportBufferOverflow,
        long numTransportDiscard,
        long numTransportIgnore
    )
    {
        this.numTransportRx = numTransportRx;
        this.numTransportTx = numTransportTx;
        this.numTransportErrorRx = numTransportErrorRx;
        this.numTransportBufferOverflow = numTransportBufferOverflow;
        this.numTransportDiscard = numTransportDiscard;
        this.numTransportIgnore = numTransportIgnore;

    }
    /**
     * Number of valid TPDU's received
     */
    public final long numTransportRx;

    /**
     * Number of valid TPDU's transmitted
     */
    public final long numTransportTx;

    /**
     * Number of TPDUs dropped due to malformed contents
     */
    public final long numTransportErrorRx;

    /**
     * Number of times received data was too big for reassembly buffer
     */
    public final long numTransportBufferOverflow;

    /**
     * number of times transport buffer is discard due to new FIR
     */
    public final long numTransportDiscard;

    /**
     * number of segments ignored due to bad FIR/FIN or SEQ
     */
    public final long numTransportIgnore;
}
