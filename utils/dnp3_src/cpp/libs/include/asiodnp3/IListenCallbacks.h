/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#ifndef ASIODNP3_ILISTENCALLBACKS_H
#define ASIODNP3_ILISTENCALLBACKS_H

#include <openpal/executor/TimeDuration.h>
#include <opendnp3/link/LinkHeaderFields.h>

#include "asiodnp3/ISessionAcceptor.h"
#include "asiodnp3/X509Info.h"

namespace asiodnp3
{

/**
* Callback interface invoked when a new connection is accepted
*/
class IListenCallbacks
{
public:

	virtual ~IListenCallbacks() {}

	/**
	* Ask user code if the following connection should be accepted
	*
	* @param sessionid Incrementing id used to uniquely identify the session
	* @param ipaddress The IP address of the connecting host. Can optionally be used for connection filtering
	*
	* @return If true, the connection is accepted and a link frame parser is created to handle incoming frame data
	*/
	virtual bool AcceptConnection(uint64_t sessionid, const std::string& ipaddress) = 0;

	/**
	* Ask user code if the following preverified certificate should be accepted
	*
	* @param sessionid Incrementing id used to uniquely identify the session
	* @param info Information from the x509 certificate
	*
	* @return If true, if the certificate should be accepted, false otherwise.
	*/
	virtual bool AcceptCertificate(uint64_t sessionid, const X509Info& info) = 0;

	/// return the amount of time the session should wait for the first frame
	virtual openpal::TimeDuration GetFirstFrameTimeout() = 0;

	/**
	* Called when the first link-layer frame is received for a session
	*/
	virtual void OnFirstFrame(
	    uint64_t sessionid,
	    const opendnp3::LinkHeaderFields& header,
	    ISessionAcceptor& acceptor
	) = 0;

	/**
	* Called when a socket closes
	*
	* @param sessionid Incrementing id used to uniquely identify the session
	* @param session Possibly NULL shared_ptr to the master session if it was created
	*/
	virtual void OnConnectionClose(uint64_t sessionid, const std::shared_ptr<IMasterSession>& session) = 0;

	/**
	* Called when a certificate fails verification
	*
	* @param sessionid Incrementing id used to uniquely identify the session
	* @param info Information from the x509 certificate
	* @param error Error code with reason for failed verification
	*/
	virtual void OnCertificateError(uint64_t sessionid, const X509Info& info, int error) = 0;
};

}

#endif
