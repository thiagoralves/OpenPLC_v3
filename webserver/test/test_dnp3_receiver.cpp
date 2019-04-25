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

#include "catch.hpp"

#include "dnp3_receiver.h"
#include "glue.h"
#include "glue_test_helpers.h"

using namespace opendnp3;

SCENARIO("dnp3 receiver", "Receiver") {

    auto variables = make_vars();
    auto range = Dnp3Range{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    GIVEN("No glue range") {
		Dnp3Receiver receiver(variables, range);
        WHEN("Select ControlRelayOutputBlock") {
            ControlRelayOutputBlock crob;
            REQUIRE(receiver.Select(crob, 0) == CommandStatus::OUT_OF_RANGE);
        }

        WHEN("Select AnalogOutputInt16") {
            AnalogOutputInt16 aob;
            REQUIRE(receiver.Select(aob, 0) == CommandStatus::OUT_OF_RANGE);
        }

        WHEN("Select AnalogOutputInt32") {
            AnalogOutputInt32 aob;
            REQUIRE(receiver.Select(aob, 0) == CommandStatus::OUT_OF_RANGE);
        }

        WHEN("Select AnalogOutputFloat32") {
            AnalogOutputFloat32 aob;
            REQUIRE(receiver.Select(aob, 0) == CommandStatus::OUT_OF_RANGE);
        }

        WHEN("Select AnalogOutputDouble64") {
            AnalogOutputDouble64 aob;
            REQUIRE(receiver.Select(aob, 0) == CommandStatus::OUT_OF_RANGE);
        }
    }

    GIVEN("One boolean output glue") {
        range.bool_outputs_start = 0;
        range.bool_outputs_end = 1;
		Dnp3Receiver receiver(variables, range);
        WHEN("Select ControlRelayOutputBlock") {
            ControlRelayOutputBlock crob;
            REQUIRE(receiver.Select(crob, 0) == CommandStatus::SUCCESS);
        }

        WHEN("Operate ControlRelayOutputBlock latch on") {
            ControlRelayOutputBlock crob(ControlCode::LATCH_ON);

            THEN("sets output to true") {
                REQUIRE(receiver.Select(crob, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(crob, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);
                REQUIRE(*variables->bool_outputs[0]);
            }
        }

        WHEN("Operate ControlRelayOutputBlock latch off") {
            ControlRelayOutputBlock crob(ControlCode::LATCH_OFF);

            THEN("sets output to false") {
                REQUIRE(receiver.Select(crob, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(crob, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);
                REQUIRE(!(*variables->bool_outputs[0]));
            }
        }
    }

    GIVEN("One analog 16 output glue") {
        range.outputs_start = 0;
        range.outputs_end = 1;
		variables->outputs[0].type = IECVT_INT;
		variables->outputs[0].value = new IEC_INT{ 0 };
		Dnp3Receiver receiver(variables, range);

        WHEN("Select AnalogOutputInt16") {
            AnalogOutputInt16 aoi;
            REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
        }

        WHEN("Operate AnalogOutputInt16 int value") {
            AnalogOutputInt16 aoi(9);

            THEN("sets output to true") {
                REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(aoi, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);
                REQUIRE(*reinterpret_cast<IEC_INT*>(variables->outputs[0].value) == 9);
            }
        }
    }

    GIVEN("One analog 32 output glue") {
        range.outputs_start = 0;
        range.outputs_end = 1;
		variables->outputs[0].type = IECVT_INT;
		variables->outputs[0].value = new IEC_INT{ 0 };
		Dnp3Receiver receiver(variables, range);

        WHEN("Select AnalogOutputInt32") {
            AnalogOutputInt32 aoi;
            REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
        }

        WHEN("Operate AnalogOutputInt32 int value") {
            AnalogOutputInt32 aoi(9);

            THEN("sets output to true") {
                REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(aoi, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);
                REQUIRE(*reinterpret_cast<IEC_INT*>(variables->outputs[0].value) == 9);
            }
        }
    }

    GIVEN("One float 32 output glue") {
        range.outputs_start = 0;
        range.outputs_end = 1;
		variables->outputs[0].type = IECVT_INT;
		variables->outputs[0].value = new IEC_INT{ 0 };
		Dnp3Receiver receiver(variables, range);

        WHEN("Select AnalogOutputFloat32") {
            AnalogOutputFloat32 aoi;
            REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
        }

        WHEN("Operate AnalogOutputFloat32 value") {
            AnalogOutputFloat32 aoi(9);

            THEN("sets output to true") {
                REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(aoi, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);
                REQUIRE(*reinterpret_cast<IEC_INT*>(variables->outputs[0].value) == 9);
            }
        }
    }

    GIVEN("One double 64 output glue") {
        range.outputs_start = 0;
        range.outputs_end = 1;
		variables->outputs[0].type = IECVT_INT;
		variables->outputs[0].value = new IEC_INT{ 0 };
		Dnp3Receiver receiver(variables, range);

        WHEN("Select AnalogOutputDouble64") {
            AnalogOutputDouble64 aoi;
            REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
        }

        WHEN("Operate AnalogOutputDouble64 value") {
            AnalogOutputDouble64 aoi(9);

            THEN("sets output to true") {
                REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(aoi, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);
                REQUIRE(*reinterpret_cast<IEC_INT*>(variables->outputs[0].value) == 9);
            }
        }
    }
}
