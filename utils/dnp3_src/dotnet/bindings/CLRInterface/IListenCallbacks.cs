//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the terms of the License.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    public interface IListenCallbacks
    {        
        /// <summary>
        /// Ask user code if the following connection should be accepted
        /// </summary>
        /// <param name="sessionid">Incrementing id used to uniquely identify the session</param>
        /// <param name="ipaddress">The IP address of the connecting host. Can optionally be used for connection filtering</param>
        /// <returns>If true, the connection is accepted and a link frame parser is created to handle incoming frame data</returns>
        bool AcceptConnection(UInt64 sessionid, string ipaddress);

        /// <summary>
        /// Ask user code if the following preverified certificate should be accepted
        /// </summary>
        /// <param name="sessionid">Incrementing id used to uniquely identify the session</param>
        /// <param name="info">Information from the x509 certificate</param>
        /// <returns>If true, the certificate is accepted and a link frame parser is created to handle incoming frame data</returns>
        bool AcceptCertificate(UInt64 sessionid, X509Info info);

        /// <summary>
        /// Retrieve the first frame timeout
        /// </summary>
        /// <returns>The amount of time the session should wait for the first frame</returns>
        TimeSpan GetFirstFrameTimeout();

        /// <summary>
        /// Called when the first link-layer frame is received for a session
        /// </summary>
        /// <param name="sessionid">Incrementing id used to uniquely identify the session</param>
        /// <param name="header">The link-layer header including source and destination addresses</param>
        /// <param name="acceptor">Interface used to create a master on the session</param>
        void OnFirstFrame(UInt64 sessionid, LinkHeader header, ISessionAcceptor acceptor);

        /// <summary>
        /// Called when a socket/session closes.
        /// </summary>
        /// <param name="sessionid">Incrementing id used to uniquely identify the session</param>
        /// <param name="session">Possibly null reference to the master session</param>
        void OnConnectionClose(UInt64 sessionid, IMasterSession session);

        /// <summary>
        /// Called when a certificate fails verification
        /// </summary>
        /// <param name="sessionid">Incrementing id used to uniquely identify the session</param>
        /// <param name="info">Information from the x509 certificate</param>
        /// <param name="error">Error code with reason for failed verification</param>
        void OnCertificateError(UInt64 sessionid, X509Info info, int error);
    }   
}
