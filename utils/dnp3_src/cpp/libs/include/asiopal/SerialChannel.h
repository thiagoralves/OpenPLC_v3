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
#ifndef ASIOPAL_SERIALCHANNEL_H
#define ASIOPAL_SERIALCHANNEL_H

#include "IAsyncChannel.h"

#include "asiopal/SerialTypes.h"

#include <asio/serial_port.hpp>

namespace asiopal
{

class SerialChannel final : public IAsyncChannel
{

public:

	static std::shared_ptr<SerialChannel> Create(std::shared_ptr<Executor> executor)
	{
		return std::make_shared<SerialChannel>(executor);
	}

	SerialChannel(std::shared_ptr<Executor> executor);

	bool Open(const SerialSettings& settings, std::error_code& ec);

private:

	virtual void BeginReadImpl(openpal::WSlice buffer) override;
	virtual void BeginWriteImpl(const openpal::RSlice& buffer)  override;
	virtual void ShutdownImpl()  override;

	asio::basic_serial_port<> port;

};

}

#endif
