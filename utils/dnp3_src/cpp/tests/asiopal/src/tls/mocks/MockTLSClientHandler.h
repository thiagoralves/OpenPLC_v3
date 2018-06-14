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

#ifndef ASIOPAL_MOCKTLSCLIENTHANDLER_H
#define ASIOPAL_MOCKTLSCLIENTHANDLER_H

#include "asiopal/IAsyncChannel.h"

#include "asiopal/tls/TLSStreamChannel.h"

#include <deque>
#include <asio/ssl.hpp>

namespace asiopal
{

class MockTLSClientHandler final
{

public:

	void OnConnect(const std::shared_ptr<Executor>& executor, const std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>>& stream, const std::error_code& ec)
	{
		if (ec)
		{
			++num_error;
			throw std::logic_error(ec.message());
		}
		else
		{
			channels.push_back(TLSStreamChannel::Create(executor, stream));
		}
	}

	~MockTLSClientHandler()
	{
		for (auto& channel : channels)
		{
			channel->Shutdown();
		}
	}

	size_t num_error = 0;

	std::deque<std::shared_ptr<IAsyncChannel>> channels;

};

}

#endif








