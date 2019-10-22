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

#include "catch.hpp"

#include "dnp3_receiver.h"
#include "glue.h"

using namespace std;
using namespace opendnp3;

SCENARIO("dnp3 receiver", "Receiver") {
    Dnp3IndexedGroup binary_commands = {0};
    Dnp3IndexedGroup analog_commands = {0};
    GIVEN("No glue range") {
        Dnp3Receiver receiver(binary_commands, analog_commands);
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

    GIVEN("One boolean command") {
        IEC_BOOL bool_val(0);
        auto group = GlueBoolGroup { .index=0, .values={ &bool_val, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr } };

        const GlueVariable glue_var = { IECLDT_IN, IECLST_BIT, 0, 0, IECVT_BOOL, &group };
        const GlueVariable* glue_vars[] = { &glue_var };
        binary_commands.size = 1;
        binary_commands.items = glue_vars;

    Dnp3Receiver receiver(binary_commands, analog_commands);
        WHEN("Select ControlRelayOutputBlock") {
            ControlRelayOutputBlock crob;
            REQUIRE(receiver.Select(crob, 0) == CommandStatus::SUCCESS);
        }

        WHEN("Operate ControlRelayOutputBlock latch on") {
            ControlRelayOutputBlock crob(ControlCode::LATCH_ON);

            THEN("sets output to true") {
                REQUIRE(receiver.Select(crob, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(crob, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);

                receiver.ExchangeGlue();

                REQUIRE(bool_val);
            }
        }

        WHEN("Operate ControlRelayOutputBlock latch off") {
            ControlRelayOutputBlock crob(ControlCode::LATCH_OFF);

            THEN("sets output to false") {
                REQUIRE(receiver.Select(crob, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(crob, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);

                receiver.ExchangeGlue();

                REQUIRE(!bool_val);
            }
        }
    }

    GIVEN("One analog 16 output glue") {
        IEC_SINT int_val(0);
        const GlueVariable glue_var = { IECLDT_IN, IECLST_BYTE, 0, 0, IECVT_SINT, &int_val };
        const GlueVariable* glue_vars[] = { &glue_var };
        analog_commands.size = 1;
        analog_commands.items = glue_vars;

    Dnp3Receiver receiver(binary_commands, analog_commands);
        WHEN("Select AnalogOutputInt16") {
            AnalogOutputInt16 aoi;
            REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
        }

        WHEN("Operate AnalogOutputInt16 int value") {
            AnalogOutputInt16 aoi(9);

            THEN("sets output to 9") {
                REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(aoi, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);

                receiver.ExchangeGlue();

                REQUIRE(int_val == 9);
            }
        }
    }

    GIVEN("One analog 32 output glue") {
        IEC_INT int_val(0);
        const GlueVariable glue_var = { IECLDT_IN, IECLST_WORD, 0, 0, IECVT_INT, &int_val };
        const GlueVariable* glue_vars[] = { &glue_var };
        analog_commands.size = 1;
        analog_commands.items = glue_vars;

    Dnp3Receiver receiver(binary_commands, analog_commands);

        WHEN("Select AnalogOutputInt32") {
            AnalogOutputInt32 aoi;
            REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
        }

        WHEN("Operate AnalogOutputInt32 int value") {
            AnalogOutputInt32 aoi(9);

            THEN("sets output to true") {
                REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(aoi, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);

                receiver.ExchangeGlue();

                REQUIRE(int_val == 9);
            }
        }
    }

    GIVEN("One float 32 output glue") {
        IEC_LINT int_val(0);
        const GlueVariable glue_var = { IECLDT_IN, IECLST_DOUBLEWORD, 0, 0, IECVT_LINT, &int_val };
        const GlueVariable* glue_vars[] = { &glue_var };
        analog_commands.size = 1;
        analog_commands.items = glue_vars;

    Dnp3Receiver receiver(binary_commands, analog_commands);

        WHEN("Select AnalogOutputFloat32") {
            AnalogOutputFloat32 aoi;
            REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
        }

        WHEN("Operate AnalogOutputFloat32 value") {
            AnalogOutputFloat32 aoi(9);

            THEN("sets output to true") {
                REQUIRE(receiver.Select(aoi, 0) == CommandStatus::SUCCESS);
                REQUIRE(receiver.Operate(aoi, 0, OperateType::DirectOperate) == CommandStatus::SUCCESS);

                receiver.ExchangeGlue();

                REQUIRE(int_val == 9);
            }
        }
    }
}

#endif  // OPLC_DNP3_OUTSTATION
