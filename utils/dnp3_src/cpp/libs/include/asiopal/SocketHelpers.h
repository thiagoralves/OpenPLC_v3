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

#ifndef ASIOPAL_SOCKET_HELPERS_H
#define ASIOPAL_SOCKET_HELPERS_H

#include <openpal/util/Uncopyable.h>

#include <system_error>
#include <asio.hpp>

namespace asiopal
{
class SocketHelpers : private openpal::StaticOnly
{

public:
	/**
	* Bind a socket object to a local endpoint given an address. If the address is empty, 0.0.0.0 is used
	*/
	template <class SocketType>
	static void BindToLocalAddress(const std::string& address, asio::ip::tcp::endpoint& endpoint, SocketType& socket, std::error_code& ec)
	{
		auto string = address.empty() ? "0.0.0.0" : address;
		auto addr = asio::ip::address::from_string(string, ec);
		if (!ec)
		{
			endpoint.address(addr);
			socket.open(asio::ip::tcp::v4(), ec);
			if (!ec)
			{
				socket.bind(endpoint, ec);
			}
		}
	}
};

}

#endif

