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

#include "asiopal/tls/TLSStreamChannel.h"

namespace asiopal
{

TLSStreamChannel::TLSStreamChannel(const std::shared_ptr<Executor>& executor, const std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>>& stream) :
	IAsyncChannel(executor),
	stream(stream)
{}

void TLSStreamChannel::BeginReadImpl(openpal::WSlice dest)
{
	auto callback = [this](const std::error_code & ec, size_t num)
	{
		this->OnReadCallback(ec, num);
	};

	stream->async_read_some(asio::buffer(dest, dest.Size()), this->executor->strand.wrap(callback));
}

void TLSStreamChannel::BeginWriteImpl(const openpal::RSlice& data)
{
	auto callback = [this](const std::error_code & ec, size_t num)
	{
		this->OnWriteCallback(ec, num);
	};

	asio::async_write(*stream, asio::buffer(data, data.Size()), this->executor->strand.wrap(callback));
}

void TLSStreamChannel::ShutdownImpl()
{
	// Don't bother with a TLS shutdown. Just shutdown the socket.
	std::error_code ec;
	stream->lowest_layer().shutdown(asio::socket_base::shutdown_both, ec);
	stream->lowest_layer().close(ec);
}

}


