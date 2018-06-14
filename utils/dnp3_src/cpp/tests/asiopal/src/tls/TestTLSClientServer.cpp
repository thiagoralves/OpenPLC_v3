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
#include <catch.hpp>

#include "mocks/MockTLSPair.h"

#include <iostream>
#include <fstream>

using namespace asiopal;

#define SUITE(name) "TLS client/server suite - " name

template <class F>
void WithIO(const F& fun)
{
	auto io = std::make_shared<MockIO>();
	fun(io);
	io->RunUntilOutOfWork();
}

std::string get_path(const std::string& file)
{
	std::ostringstream oss;
	oss << "../cpp/tests/asiopal/tls-certs/" << file;
	return oss.str();
}

bool exists(const std::string& file)
{
	std::ifstream infile(file);
	return infile.good();
}

TEST_CASE(SUITE("client and server can connect"))
{
	const auto key1 = get_path("entity1_key.pem");
	const auto key2 = get_path("entity2_key.pem");
	const auto cert1 = get_path("entity1_cert.pem");
	const auto cert2 = get_path("entity2_cert.pem");

	if (!(exists(key1) && exists(key2) && exists(cert1) && exists(cert2)))
	{
		std::cout << "Could not locate one or more of the test TLS certificates. Expected to be run from the project root directory." << std::endl;
		std::cout << "This test will be skipped." << std::endl;
		return;
	}

	auto iteration = [ = ]()
	{
		auto test = [ = ](const std::shared_ptr<MockIO>& io)
		{


			TLSConfig cfg1(cert2, cert1, key1);
			TLSConfig cfg2(cert1, cert2, key2);

			MockTLSPair pair(io, 20001, cfg1, cfg2);

			pair.Connect(1);
		};

		WithIO(test);
	};

	// run multiple times to ensure the test is cleaning up after itself in terms of system resources
	for (int i = 0; i < 5; ++i)
	{
		iteration();
	}
}










