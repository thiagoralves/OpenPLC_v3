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

#include <asiodnp3/IOutstation.h>
#include <asiodnp3/UpdateBuilder.h>
#include <spdlog/spdlog.h>

#include "dnp3_publisher.h"
#include "glue.h"

using namespace opendnp3;
using namespace std;

/** \addtogroup openplc_runtime
 *  @{
 */


/// Initialize a new instance of the publisher.
Dnp3Publisher::Dnp3Publisher(
    shared_ptr<asiodnp3::IOutstation> outstation,
    const Dnp3MappedGroup& measurements) :

    outstation(outstation),
    measurements(measurements)
{ }

template<class T>
T cast_variable(const GlueVariable* var)
{
    T val;
    void* value = var->value;
    switch (var->type)
    {
        case IECVT_SINT:
        {
            IEC_SINT* tval = reinterpret_cast<IEC_SINT*>(value);
            val = *tval;
            break;
        }
        case IECVT_USINT:
        {
            IEC_USINT* tval = reinterpret_cast<IEC_USINT*>(value);
            val = *tval;
            break;
        }
        case IECVT_INT:
        {
            IEC_INT* tval = reinterpret_cast<IEC_INT*>(value);
            val = *tval;
            break;
        }
        case IECVT_UINT:
        {
            IEC_UINT* tval = reinterpret_cast<IEC_UINT*>(value);
            val = *tval;
            break;
        }
        case IECVT_DINT:
        {
            IEC_DINT* tval = reinterpret_cast<IEC_DINT*>(value);
            val = *tval;
            break;
        }
        case IECVT_UDINT:
        {
            IEC_UDINT* tval = reinterpret_cast<IEC_UDINT*>(value);
            val = *tval;
            break;
        }
        case IECVT_REAL:
        {
            IEC_REAL* tval = reinterpret_cast<IEC_REAL*>(value);
            val = *tval;
            break;
        }
        case IECVT_LREAL:
        {
            IEC_LREAL* tval = reinterpret_cast<IEC_LREAL*>(value);
            val = *tval;
            break;
        }
    }

    return val;
}

/// Writes mapped values in the glue variables to the DNP3 outstation channel.
/// This function would normally be called on a timer to write the data.
/// Writing to the DNP3 channel happens asynchronously, so completion of this
/// function doesn't mean anything has been sent.
/// @return the number of values that were sent to the channel.
uint32_t Dnp3Publisher::ExchangeGlue()
{
    uint32_t num_writes(0);
    asiodnp3::UpdateBuilder builder;

    spdlog::trace("Writing glue variables to DNP3 points");

    for (uint16_t i(0); i < measurements.size; ++i)
    {
        const DNP3MappedGlueVariable& mapping = measurements.items[i];
        const uint8_t group = mapping.group;
        const uint16_t point_index_number = mapping.point_index_number;
        const GlueVariable* var = mapping.variable;
        void* value = var->value;

        if (group == GROUP_BINARY_INPUT || group == GROUP_BINARY_OUTPUT_STATUS)
        {
            const GlueBoolGroup* bool_group = reinterpret_cast<const GlueBoolGroup*>(value);
            if (group == GROUP_BINARY_INPUT)
            {
                builder.Update(Binary(*(bool_group->values[0])), point_index_number);
            }
            else
            {
                builder.Update(BinaryOutputStatus(*(bool_group->values[0])), point_index_number);
            }
        }
        else if (group == GROUP_ANALOG_INPUT || group == GROUP_ANALOG_OUTPUT_STATUS)
        {
            double double_val = cast_variable<double>(var);
            if (group == GROUP_ANALOG_INPUT)
            {
                builder.Update(Analog(double_val), point_index_number);
            }
            else
            {
                builder.Update(AnalogOutputStatus(double_val), point_index_number);
            }
        }
        else if (group == GROUP_COUNTER || group == GROUP_FROZEN_COUNTER)
        {
            uint32_t int_val = cast_variable<uint32_t>(var);
            if (group == GROUP_COUNTER)
            {
                builder.Update(Counter(int_val), point_index_number);
            }
            else
            {
                builder.Update(FrozenCounter(int_val), point_index_number);
            }
        }

        num_writes += 1;
    }

    outstation->Apply(builder.Build());

    return num_writes;
}

#endif  // OPLC_DNP3_OUTSTATION

/** @}*/
