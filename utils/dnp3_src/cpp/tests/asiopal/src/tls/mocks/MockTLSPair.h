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

#ifndef ASIOPAL_MOCKTLSPAIR_H
#define ASIOPAL_MOCKTLSPAIR_H

#include "../../mocks/MockIO.h"
#include "MockTLSClientHandler.h"
#include "MockTLSServer.h"

#include "asiopal/tls/TLSClient.h"

#include "testlib/MockLogHandler.h"

namespace asiopal
{

class MockTLSPair
{

public:

	MockTLSPair(std::shared_ptr<MockIO> io, uint16_t port, const TLSConfig& client, const TLSConfig& server, std::error_code ec = std::error_code());

	~MockTLSPair();

	void Connect(size_t num = 1);

	bool NumConnectionsEqual(size_t num) const;

	testlib::MockLogHandler log;

private:

	std::shared_ptr<MockIO> io;
	std::shared_ptr<MockTLSClientHandler> chandler;
	std::shared_ptr<TLSClient> client;
	std::shared_ptr<MockTLSServer> server;
};

}

#endif







