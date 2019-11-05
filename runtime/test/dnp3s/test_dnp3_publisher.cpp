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

#ifdef OPLC_DNP3_OUTSTATION

#include <cstdint>
#include <utility>
#include <vector>
#include <asiodnp3/IOutstation.h>

#include "catch.hpp"
#include "fakeit.hpp"

#include "glue.h"
#include "dnp3s/dnp3.h"
#include "dnp3s/dnp3_publisher.h"

using namespace std;
using namespace fakeit;
using namespace opendnp3;

/// An implementation of the update handler that caches the updates that were
/// requested. This imlementation allows us to spy on the behaviour and to know
/// during the tests whether the correct updates were called.
class UpdateCaptureHandler : public opendnp3::IUpdateHandler {
public:
    vector<pair<bool, uint16_t>> binary;
    vector<pair<bool, uint16_t>> binary_output;
    vector<pair<double, uint16_t>> analog;
    vector<pair<double, uint16_t>> analog_output;

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
        binary_output.push_back(std::make_pair(meas.value, index));
        return true;
    }
    virtual bool Update(const AnalogOutputStatus& meas, uint16_t index, EventMode mode) {
        analog_output.push_back(std::make_pair(meas.value, index));
        return true;
    }
    virtual bool Update(const TimeAndInterval& meas, uint16_t index) {
        return true;
    }
    virtual bool Modify(FlagsType type, uint16_t start, uint16_t stop, uint8_t flags) {
        return true;
    }
};

SCENARIO("dnp3 publisher", "ExchangeGlue") {
    Mock<asiodnp3::IOutstation> mock_outstation;
    UpdateCaptureHandler update_handler;

    When(Method(mock_outstation, Apply)).AlwaysDo([&](const asiodnp3::Updates& updates) {
        updates.Apply(update_handler);
    });
    auto outstation = std::shared_ptr<asiodnp3::IOutstation>(&mock_outstation.get(), [](asiodnp3::IOutstation*) {});

    Dnp3MappedGroup measurements = {0};
    Dnp3Publisher publisher(outstation, measurements);

    GIVEN("No glued measurements") {
        auto num_writes = publisher.ExchangeGlue();
        THEN("Writes nothing") {
            REQUIRE(num_writes == 0);
        }
    }

    GIVEN("Boolean input variable at offset 0") {
        IEC_BOOL bool_val(0);
        auto group = GlueBoolGroup { .index=0, .values={ &bool_val, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr } };

        const GlueVariable glue_var = { IECLDT_OUT, IECLST_BIT, 0, 0, IECVT_BOOL, &group };
        DNP3MappedGlueVariable mapped_vars[] = { {
            .group = 1,
            .point_index_number = 0,
            .variable = &glue_var
        } };

        Dnp3MappedGroup measurements;
        measurements.size = 1;
        measurements.items = mapped_vars;
        Dnp3Publisher publisher(outstation, measurements);

        WHEN("value is false") {
            bool_val = false;
            auto num_writes = publisher.ExchangeGlue();

            THEN("Writes binary input false") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(update_handler.binary.size() == 1);
                REQUIRE(update_handler.binary[0].first == false);
                REQUIRE(update_handler.binary[0].second == 0);
            }
        }

        WHEN("value is true") {
            bool_val = true;
            auto num_writes = publisher.ExchangeGlue();

            THEN("Writes binary input true") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(update_handler.binary.size() == 1);
                REQUIRE(update_handler.binary[0].first == true);
                REQUIRE(update_handler.binary[0].second == 0);
            }
        }
    }

    GIVEN("Boolean output status variable at offset 0") {
        IEC_BOOL bool_val(0);
        auto group = GlueBoolGroup { .index=0, .values={ &bool_val, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr } };

        const GlueVariable glue_var = { IECLDT_OUT, IECLST_BIT, 0, 0, IECVT_BOOL, &group };
        DNP3MappedGlueVariable mapped_vars[] = { {
            .group = 10,
            .point_index_number = 0,
            .variable = &glue_var
        } };

        Dnp3MappedGroup measurements;
        measurements.size = 1;
        measurements.items = mapped_vars;
        Dnp3Publisher publisher(outstation, measurements);

        WHEN("value is false") {
            bool_val = false;
            auto num_writes = publisher.ExchangeGlue();

            THEN("Writes binary input false") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(update_handler.binary_output.size() == 1);
                REQUIRE(update_handler.binary_output[0].first == false);
                REQUIRE(update_handler.binary_output[0].second == 0);
            }
        }

        WHEN("value is true") {
            bool_val = true;
            auto num_writes = publisher.ExchangeGlue();

            THEN("Writes binary input true") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(update_handler.binary_output.size() == 1);
                REQUIRE(update_handler.binary_output[0].first == true);
                REQUIRE(update_handler.binary_output[0].second == 0);
            }
        }
    }

    GIVEN("Real variable at offset 0") {
        IEC_REAL real_val(9);

        const GlueVariable glue_var = { IECLDT_OUT, IECLST_DOUBLEWORD, 0, 0, IECVT_REAL, &real_val };
        DNP3MappedGlueVariable mapped_vars[] = { {
            .group = 30,
            .point_index_number = 0,
            .variable = &glue_var
        } };

        Dnp3MappedGroup measurements;
        measurements.size = 1;
        measurements.items = mapped_vars;
        Dnp3Publisher publisher(outstation, measurements);

        WHEN("value is 9") {
            auto num_writes = publisher.ExchangeGlue();

            THEN("Writes binary input false") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(update_handler.analog.size() == 1);
                REQUIRE(update_handler.analog[0].first == 9);
                REQUIRE(update_handler.analog[0].second == 0);
            }
        }
    }

    GIVEN("Real status variable at offset 0") {
        IEC_REAL real_val(9);

        const GlueVariable glue_var = { IECLDT_OUT, IECLST_DOUBLEWORD, 0, 0, IECVT_REAL, &real_val };
        DNP3MappedGlueVariable mapped_vars[] = { {
            .group = 40,
            .point_index_number = 0,
            .variable = &glue_var
        } };

        Dnp3MappedGroup measurements;
        measurements.size = 1;
        measurements.items = mapped_vars;
        Dnp3Publisher publisher(outstation, measurements);

        WHEN("value is 9") {
            auto num_writes = publisher.ExchangeGlue();

            THEN("Writes binary input false") {
                REQUIRE(num_writes == 1);
                Verify(Method(mock_outstation, Apply)).Exactly(Once);
                REQUIRE(update_handler.analog_output.size() == 1);
                REQUIRE(update_handler.analog_output[0].first == 9);
                REQUIRE(update_handler.analog_output[0].second == 0);
            }
        }
    }
}

#endif  // OPLC_DNP3_OUTSTATION
