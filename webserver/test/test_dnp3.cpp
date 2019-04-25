// Copyright 2019 Smarter Grid Solutions
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.

#include <cstdint>
#include <utility>
#include <mutex>

#include "catch.hpp"
#include "fakeit.hpp"

// The current DNP3 file does not expose anything and my current goal is to minimize
// changes. For now, just include it here so that we can we test it. This is needed
// because the structure currently depends on a number of globals that make testing
// a bit hard.
void sleep_until(timespec*, int) {}
bool run_dnp3(false);
#include "dnp3.cpp"
#include "glue.h"
IEC_BOOL* bool_output[BUFFER_SIZE][8];
IEC_BOOL* bool_input[BUFFER_SIZE][8];
std::mutex bufferLock;
const std::uint16_t OPLCGLUE_INPUT_SIZE(1);
GlueVariable oplc_input_vars[] = {
	{ IECVT_UNASSIGNED, nullptr },
};
const std::uint16_t OPLCGLUE_OUTPUT_SIZE(1);
GlueVariable oplc_output_vars[] = {
	{ IECVT_UNASSIGNED, nullptr },
};

using namespace std;

SCENARIO("create_config", "")
{
	GIVEN("<input stream>")
	{
		WHEN("stream is empty creates default config")
		{
			std::stringstream input_stream;
			pair<OutstationStackConfig, Dnp3Range> config_range(create_config(input_stream));
			OutstationStackConfig config = config_range.first;

			REQUIRE(config.dbConfig.binary.IsEmpty());
			REQUIRE(config.dbConfig.doubleBinary.IsEmpty());
			REQUIRE(config.dbConfig.analog.IsEmpty());
			REQUIRE(config.dbConfig.counter.IsEmpty());
			REQUIRE(config.dbConfig.frozenCounter.IsEmpty());
			REQUIRE(config.dbConfig.boStatus.IsEmpty());
			REQUIRE(config.dbConfig.aoStatus.IsEmpty());
			REQUIRE(config.dbConfig.timeAndInterval.IsEmpty());
			REQUIRE(config.link.IsMaster == false);
			REQUIRE(config.link.UseConfirms == false);
			REQUIRE(config.link.NumRetry == 0);
			REQUIRE(config.link.LocalAddr == 1024);
			REQUIRE(config.link.RemoteAddr == 1);
		}

		WHEN("stream only specifies default size")
		{
			std::stringstream input_stream("database_size=1");
			pair<OutstationStackConfig, Dnp3Range> config_range(create_config(input_stream));
			OutstationStackConfig config = config_range.first;

			REQUIRE(config.dbConfig.binary.Size() == 1);
			REQUIRE(config.dbConfig.doubleBinary.Size() == 1);
			REQUIRE(config.dbConfig.analog.Size() == 1);
			REQUIRE(config.dbConfig.counter.Size() == 1);
			REQUIRE(config.dbConfig.frozenCounter.Size() == 1);
			REQUIRE(config.dbConfig.boStatus.Size() == 1);
			REQUIRE(config.dbConfig.aoStatus.Size() == 1);
			REQUIRE(config.dbConfig.timeAndInterval.Size() == 1);
			REQUIRE(config.link.IsMaster == false);
			REQUIRE(config.link.UseConfirms == false);
			REQUIRE(config.link.NumRetry == 0);
			REQUIRE(config.link.LocalAddr == 1024);
			REQUIRE(config.link.RemoteAddr == 1);
		}
	}
}

SCENARIO("dnp3StartServer", "")
{
	WHEN("provides configuration stream but not run")
	{
		// Configure this to start and then immediately terminate
		// the run flag is set to false. This should just return quickly
		bool run_dnp3(false);
		unique_ptr<istream, std::function<void(istream*)>> cfg_stream(new stringstream(""), [](istream* s) { delete s; });
		dnp3StartServer(20000, cfg_stream, &run_dnp3);
	}
}
