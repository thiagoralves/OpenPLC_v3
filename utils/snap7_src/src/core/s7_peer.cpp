/*=============================================================================|
|  PROJECT SNAP7                                                         1.3.0 |
|==============================================================================|
|  Copyright (C) 2013, 2015 Davide Nardella                                    |
|  All rights reserved.                                                        |
|==============================================================================|
|  SNAP7 is free software: you can redistribute it and/or modify               |
|  it under the terms of the Lesser GNU General Public License as published by |
|  the Free Software Foundation, either version 3 of the License, or           |
|  (at your option) any later version.                                         |
|                                                                              |
|  It means that you can distribute your commercial software linked with       |
|  SNAP7 without the requirement to distribute the source code of your         |
|  application and without the requirement that your application be itself     |
|  distributed under LGPL.                                                     |
|                                                                              |
|  SNAP7 is distributed in the hope that it will be useful,                    |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of              |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               |
|  Lesser GNU General Public License for more details.                         |
|                                                                              |
|  You should have received a copy of the GNU General Public License and a     |
|  copy of Lesser GNU General Public License along with Snap7.                 |
|  If not, see  http://www.gnu.org/licenses/                                   |
|=============================================================================*/
#include "s7_peer.h"
//---------------------------------------------------------------------------

TSnap7Peer::TSnap7Peer()
{
    PDUH_out=PS7ReqHeader(&PDU.Payload);
    PDURequest=480; // Our request, FPDULength will contain the CPU answer
    LastError=0;
	cntword = 0;
    Destroying = false;
}
//---------------------------------------------------------------------------
TSnap7Peer::~TSnap7Peer()
{
    Destroying = true;
}
//---------------------------------------------------------------------------
int TSnap7Peer::SetError(int Error)
{
    if (Error==0)
       ClrError();
    else
       LastError=Error | LastIsoError | LastTcpError;
    return Error;
}
//---------------------------------------------------------------------------
void TSnap7Peer::ClrError()
{
    LastError=0;
    LastIsoError=0;
    LastTcpError=0;
}
//---------------------------------------------------------------------------
word TSnap7Peer::GetNextWord()
{
     if (cntword==0xFFFF)
        cntword=0;
     return cntword++;
}
//---------------------------------------------------------------------------
int TSnap7Peer::NegotiatePDULength( )
{
    int Result, IsoSize = 0;
    PReqFunNegotiateParams ReqNegotiate;
    PResFunNegotiateParams ResNegotiate;
    PS7ResHeader23 Answer;
    ClrError();
    // Setup Pointers
    ReqNegotiate = PReqFunNegotiateParams(pbyte(PDUH_out) + sizeof(TS7ReqHeader));
    // Header
    PDUH_out->P        = 0x32;            // Always $32
    PDUH_out->PDUType  = PduType_request; // $01
    PDUH_out->AB_EX    = 0x0000;          // Always $0000
    PDUH_out->Sequence = GetNextWord();   // AutoInc
    PDUH_out->ParLen   = SwapWord(sizeof(TReqFunNegotiateParams)); // 8 bytes
    PDUH_out->DataLen  = 0x0000;
    // Params
    ReqNegotiate->FunNegotiate = pduNegotiate;
    ReqNegotiate->Unknown = 0x00;
    ReqNegotiate->ParallelJobs_1 = 0x0100;
    ReqNegotiate->ParallelJobs_2 = 0x0100;
    ReqNegotiate->PDULength = SwapWord(PDURequest);
    IsoSize = sizeof( TS7ReqHeader ) + sizeof( TReqFunNegotiateParams );
    Result = isoExchangeBuffer(NULL, IsoSize);
    if ((Result == 0) && (IsoSize == int(sizeof(TS7ResHeader23) + sizeof(TResFunNegotiateParams))))
    {
        // Setup pointers
        Answer = PS7ResHeader23(&PDU.Payload);
        ResNegotiate = PResFunNegotiateParams(pbyte(Answer) + sizeof(TS7ResHeader23));
        if ( Answer->Error != 0 )
	    Result = SetError(errNegotiatingPDU);
        if ( Result == 0 )
	    PDULength = SwapWord(ResNegotiate->PDULength);
    }
    return Result;
}
//---------------------------------------------------------------------------
void TSnap7Peer::PeerDisconnect( )
{
    ClrError();
    isoDisconnect(true);
}
//---------------------------------------------------------------------------
int TSnap7Peer::PeerConnect( )
{
    int Result;

    ClrError();
	Result = isoConnect();
	if (Result == 0)
	{
		Result = NegotiatePDULength();
		if (Result != 0)
			PeerDisconnect();
	}
    return Result;
}
