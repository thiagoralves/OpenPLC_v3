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

#ifndef ASIOPAL_TLS_CONFIG_H
#define ASIOPAL_TLS_CONFIG_H

#include <string>

namespace asiopal
{

/**
* TLS configuration information
*/
struct TLSConfig
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
	TLSConfig(
	    const std::string& peerCertFilePath,
	    const std::string& localCertFilePath,
	    const std::string& privateKeyFilePath,
	    int maxVerifyDepth = 0,
	    bool allowTLSv10 = false,
	    bool allowTLSv11 = false,
	    bool allowTLSv12 = true,
	    const std::string& cipherList = ""
	) :
		peerCertFilePath(peerCertFilePath),
		localCertFilePath(localCertFilePath),
		privateKeyFilePath(privateKeyFilePath),
		maxVerifyDepth(maxVerifyDepth),
		allowTLSv10(allowTLSv10),
		allowTLSv11(allowTLSv11),
		allowTLSv12(allowTLSv12),
		cipherList(cipherList)
	{}

	/// Certificate file used to verify the peer or server. Can be CA file or a self-signed cert provided by other party.
	std::string peerCertFilePath;

	/// File that contains the certificate (or certificate chain) that will be presented to the remote side of the connection
	std::string localCertFilePath;

	/// File that contains the private key corresponding to the local certificate
	std::string privateKeyFilePath;

	/// max verification depth (defaults to 0 - peer certificate only)
	int maxVerifyDepth;

	/// Allow TLS version 1.0 (default false)
	bool allowTLSv10;

	/// Allow TLS version 1.1 (default false)
	bool allowTLSv11;

	/// Allow TLS version 1.2 (default true)
	bool allowTLSv12;

	/// openssl format cipher list
	std::string cipherList;

};

}

#endif
