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

#include <memory>
#include <asiodnp3/IOutstation.h>

#include "catch.hpp"
#include "fakeit.hpp"

#include "glue.h"
#include "glue_test_helpers.h"
#include "dnp3_publisher.h"

using namespace fakeit;
using namespace opendnp3;

/// An implementation of the updat ehandler that caches the updates that were
/// requested. This imlementation allows us to spy on the behaviour and to know
/// during the tests whether the correct updates were called.
class UpdateCaptureHandler : public opendnp3::IUpdateHandler {
public:
    std::vector<std::pair<bool, std::uint16_t>> binary;
    std::vector<std::pair<bool, std::uint16_t>> binaryOutput;
    std::vector<std::pair<double, std::uint16_t>> analog;
    std::vector<std::pair<double, std::uint16_t>> analogOutput;

    virtual ~UpdateCaptureHandler() {}
    virtual bool Update(const Binary& meas, uint16_t index, EventMode mode) {
        binary.push_back(std::make_pair(meas.value, index));
        return true;
    }
    virtual bool Update(const DoubleBitBinary& meas, uint16_t index, EventMode mode) {
        return true;
    }
    virtual bool Update(const Analog& meas, uint16_t index, EventMode mode) {
        analog.push_back(std::make_pair(meas.value, index));
        return true;
    }
    virtual bool Update(const Counter& meas, uint16_t index, EventMode mode) {
        return true;
    }
    virtual bool Update(const FrozenCounter& meas, uint16_t index, EventMode mode) {
        return true;
    }
    virtual bool Update(const BinaryOutputStatus& meas, uint16_t index, EventMode mode) {
        binaryOutput.push_back(std::make_pair(meas.value, index));
        return true;
    }
    virtual bool Update(const AnalogOutputStatus& meas, uint16_t index, EventMode mode) {
        analogOutput.push_back(std::make_pair(meas.value, index));
        return true;
    }
    virtual bool Update(const TimeAndInterval& meas, uint16_t index) {
        return true;
    }
    virtual bool Modify(FlagsType type, uint16_t start, uint16_t stop, uint8_t flags) {
        return true;
    }
};

SCENARIO("dnp3 publisher", "WriteToPoints") {
    Mock<asiodnp3::IOutstation> mock_outstation;
    UpdateCaptureHandler updateHandler;

    When(Method(mock_outstation, Apply)).AlwaysDo([&](const asiodnp3::Updates& updates) {
        updates.Apply(updateHandler);
    });
    auto outstation = std::shared_ptr<asiodnp3::IOutstation>(&mock_outstation.get(), [](asiodnp3::IOutstation*) {});
    auto variables = make_vars();
    auto range = Dnp3Range{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    

    GIVEN("No glue range") {
		Dnp3Publisher mapper(outstation, variables, range);
        auto num_writes = mapper.WriteToPoints();

        THEN("Writes nothing") {
            REQUIRE(num_writes == 0);
        }
    }

	GIVEN("Boolean input variable at offset 0") {
		range.bool_inputs_start = 0;
		range.bool_inputs_end = 1;
		Dnp3Publisher mapper(outstation, variables, range);

		WHEN("value is false") {
			*variables->bool_inputs[0][0] = false;
			auto num_writes = mapper.WriteToPoints();

			THEN("Writes binary input false") {
				REQUIRE(num_writes == 1);
				Verify(Method(mock_outstation, Apply)).Exactly(Once);
				REQUIRE(updateHandler.binary.size() == 1);
				REQUIRE(updateHandler.binary[0].first == false);
				REQUIRE(updateHandler.binary[0].second == 0);
			}
		}

		WHEN("value is true") {
			*variables->bool_inputs[0][0] = true;
			auto num_writes = mapper.WriteToPoints();

			THEN("Writes binary input true") {
				REQUIRE(num_writes == 1);
				Verify(Method(mock_outstation, Apply)).Exactly(Once);
				REQUIRE(updateHandler.binary.size() == 1);
				REQUIRE(updateHandler.binary[0].first == true);
				REQUIRE(updateHandler.binary[0].second == 0);
			}
		}
	}

	GIVEN("Boolean input variable at offset 8") {
        WHEN("Range is set and value is true") {
            range.bool_inputs_start = 8;
            range.bool_inputs_end = 9;
			Dnp3Publisher mapper(outstation, variables, range);

            *variables->bool_inputs[1][0] = true;
            auto num_writes = mapper.WriteToPoints();

            THEN("Writes binary input") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(updateHandler.binary.size() == 1);
                REQUIRE(updateHandler.binary[0].first == true);
                REQUIRE(updateHandler.binary[0].second == 8);
            }
        }
    }

    GIVEN("One boolean output status variable at offset 0") {
        range.bool_outputs_start = 0;
        range.bool_outputs_end = 1;
		Dnp3Publisher mapper(outstation, variables, range);

        WHEN("value is false") {
            *variables->bool_outputs[0][0] = false;
            auto num_writes = mapper.WriteToPoints();

            THEN("Writes binary output false") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(updateHandler.binaryOutput.size() == 1);
                REQUIRE(updateHandler.binaryOutput[0].first == false);
                REQUIRE(updateHandler.binaryOutput[0].second == 0);
            }
        }

        WHEN("value is true") {
            *variables->bool_outputs[0][0] = true;
            auto num_writes = mapper.WriteToPoints();

            THEN("Writes binary output true") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(updateHandler.binaryOutput.size() == 1);
                REQUIRE(updateHandler.binaryOutput[0].first == true);
                REQUIRE(updateHandler.binaryOutput[0].second == 0);
            }
        }
    }

    GIVEN("One input register") {
        range.inputs_start = 0;
        range.inputs_end = 1;
		Dnp3Publisher mapper(outstation, variables, range);

        WHEN("Value is 9") {
            variables->inputs[0].type = IECVT_INT;
            variables->inputs[0].value = new IEC_INT{ 9 };
            auto num_writes = mapper.WriteToPoints();

            THEN("Writes analog output 9") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(updateHandler.analog.size() == 1);
                REQUIRE(updateHandler.analog[0].first == 9);
                REQUIRE(updateHandler.analog[0].second == 0);
            }
        }
    }

    GIVEN("One output register") {
        range.outputs_start = 0;
        range.outputs_end = 1;
		Dnp3Publisher mapper(outstation, variables, range);

        WHEN("Value is 9") {
            variables->outputs[0].type = IECVT_INT;
            variables->outputs[0].value = new IEC_INT{ 9 };
            auto num_writes = mapper.WriteToPoints();

            THEN("Writes analog output 9") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(updateHandler.analogOutput.size() == 1);
                REQUIRE(updateHandler.analogOutput[0].first == 9);
                REQUIRE(updateHandler.analogOutput[0].second == 0);
            }
        }
    }
}
