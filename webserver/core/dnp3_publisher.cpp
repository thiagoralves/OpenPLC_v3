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

#include <asiodnp3/IOutstation.h>
#include <asiodnp3/UpdateBuilder.h>

#include "dnp3_publisher.h"
#include "glue.h"

using namespace opendnp3;

/// Initialize a new instance of the publilsher.
Dnp3Publisher::Dnp3Publisher(
    std::shared_ptr<asiodnp3::IOutstation> outstation,
    std::shared_ptr<GlueVariables> glue_variables,
    Dnp3Range range) :

    outstation(outstation),
    glue_variables(glue_variables),
    range(range)
{ }

/// Writes mapped values in the glue variables to the DNP3 oustation channel.
/// This function would normally be called on a timer to write the data.
/// Writing to the DNP3 channel happens asynchronously, so completion of this
/// function doesn't mean anything has been sent.
/// @return the number of values that were sent to the channel.
std::uint32_t Dnp3Publisher::WriteToPoints() {
    std::uint32_t num_writes(0);
    asiodnp3::UpdateBuilder builder;

    pthread_mutex_lock(glue_variables->bufferLock);

    // Writes data points to the outstation. We support two capabilities here:
    //
    // * A set of ranges that we will map so that not all of the data needs to
    //   be available over DNP3
    // * An offset of the index in the glue variables to the index of the DNP3 points.
    //   That is, the offset can take glue variable at index 10 and offset by -5 so that
    //   it is written to point 5.

    // Update Discrete input (Binary input)
    for (auto i = range.bool_inputs_start; i < range.bool_inputs_end; i++) {

        builder.Update(Binary(*glue_variables->bool_inputs[i/8][i%8]), i - range.bool_inputs_offset);
        num_writes += 1;
    }

    // Update Coils (Binary Output)
    for (auto i = range.bool_outputs_start; i < range.bool_outputs_end; i++) {
        builder.Update(BinaryOutputStatus(*glue_variables->bool_outputs[i/8][i%8]), i - range.bool_outputs_offset);
        num_writes += 1;
    }

    // Write the generic types
    for (auto i = range.inputs_start; i < range.inputs_end; i++) {
        void* value = glue_variables->inputs[i].value;
        if (!value) {
            // If this slot in the glue is not mapped to a memory location, then
            // it is not a defined address and we must skip it.
            continue;
        }

        // We allow the user to arbitrarily shift the point index for DNP3. This
        // might allow someone to have two outstations, both starting at index
        // 0, but referring to different points.
        std::uint16_t point_index = i - range.inputs_offset;

        num_writes += 1;
        switch(glue_variables->inputs[i].type) {
            case IECVT_SINT:
            {
                IEC_SINT* tval = reinterpret_cast<IEC_SINT*>(value);
                builder.Update(Analog(*tval), point_index);
                break;
            }
            case IECVT_USINT:
            {
                IEC_USINT* tval = reinterpret_cast<IEC_USINT*>(value);
                builder.Update(Counter(*tval), point_index);
                break;
            }
            case IECVT_INT:
            {
                IEC_INT* tval = reinterpret_cast<IEC_INT*>(value);
                builder.Update(Analog(*tval), point_index);
                break;
            }
            case IECVT_UINT:
            {
                IEC_UINT* tval = reinterpret_cast<IEC_UINT*>(value);
                builder.Update(Counter(*tval), point_index);
                break;
            }
            case IECVT_DINT:
            {
                IEC_DINT* tval = reinterpret_cast<IEC_DINT*>(value);
                builder.Update(Analog(*tval), point_index);
                break;
            }
            case IECVT_UDINT:
            {
                IEC_UDINT* tval = reinterpret_cast<IEC_UDINT*>(value);
                builder.Update(Counter(*tval), point_index);
                break;
            }
            case IECVT_REAL:
            {
                IEC_REAL* tval = reinterpret_cast<IEC_REAL*>(value);
                builder.Update(Analog(*tval), point_index);
                break;
            }
            case IECVT_LREAL:
            {
                IEC_LREAL* tval = reinterpret_cast<IEC_LREAL*>(value);
                builder.Update(Analog(*tval), point_index);
                break;
            }
            default:
                // We assumed above that we successfully found a value to write, but
                // we were wrong. This undoes the increment since we don't actually
                // support this particular type.
                num_writes -= 1;
                break;
        }
    }

	for (auto i = range.outputs_start; i < range.outputs_end; i++) {
		void* value = glue_variables->outputs[i].value;
		if (!value) {
			// If this slot in the glue is not mapped to a memory location, then
			// it is not a defined address and we must skip it.
			continue;
		}

		// We allow the user to arbitrarily shift the point index for DNP3. This
		// might allow someone to have two outstations, both starting at index
		// 0, but referring to different points.
		std::uint16_t point_index = i - range.outputs_offset;

		num_writes += 1;
		switch (glue_variables->outputs[i].type) {
		case IECVT_SINT:
		{
			IEC_SINT* tval = reinterpret_cast<IEC_SINT*>(value);
			builder.Update(AnalogOutputStatus(*tval), point_index);
			break;
		}
		case IECVT_USINT:
		{
			IEC_USINT* tval = reinterpret_cast<IEC_USINT*>(value);
			builder.Update(Counter(*tval), point_index);
			break;
		}
		case IECVT_INT:
		{
			IEC_INT* tval = reinterpret_cast<IEC_INT*>(value);
			builder.Update(AnalogOutputStatus(*tval), point_index);
			break;
		}
		case IECVT_UINT:
		{
			IEC_UINT* tval = reinterpret_cast<IEC_UINT*>(value);
			builder.Update(Counter(*tval), point_index);
			break;
		}
		case IECVT_DINT:
		{
			IEC_DINT* tval = reinterpret_cast<IEC_DINT*>(value);
			builder.Update(AnalogOutputStatus(*tval), point_index);
			break;
		}
		case IECVT_UDINT:
		{
			IEC_UDINT* tval = reinterpret_cast<IEC_UDINT*>(value);
			builder.Update(Counter(*tval), point_index);
			break;
		}
		case IECVT_REAL:
		{
			IEC_REAL* tval = reinterpret_cast<IEC_REAL*>(value);
			builder.Update(AnalogOutputStatus(*tval), point_index);
			break;
		}
		case IECVT_LREAL:
		{
			IEC_LREAL* tval = reinterpret_cast<IEC_LREAL*>(value);
			builder.Update(AnalogOutputStatus(*tval), point_index);
			break;
		}
		default:
			// We assumed above that we successfully found a value to write, but
			// we were wrong. This undoes the increment since we don't actually
			// support this particular type.
			num_writes -= 1;
			break;
		}
	}

    pthread_mutex_unlock(glue_variables->bufferLock);

    outstation->Apply(builder.Build());

    return num_writes;
}
