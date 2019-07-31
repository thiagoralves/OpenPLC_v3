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

#ifndef CORE_DNP3_PUBLISHER_H_
#define CORE_DNP3_PUBLISHER_H_

#include <cstdint>
#include <memory>

#include "dnp3.h"
#include "iec_types.h"

struct GlueVariables;
struct GlueRange;
namespace asiodnp3 {
class IOutstation;
}

/// The publisher defines the interface between the glue arrays
/// of variables that are read from PLC application and written to
/// the DNP3 channel. This published all of the available glue
/// information over DNP3, incuding inputs, outputs, memory.
class Dnp3Publisher {
 public:
    /// Constructs a new instance of the publihser object.
    /// @param outstation The outstation that is ourselves
    /// @param glue_variables The buffers that we use for data transfer
    Dnp3Publisher(
        std::shared_ptr<asiodnp3::IOutstation> outstation,
        std::shared_ptr<GlueVariables> glue_variables,
        Dnp3Range range);

    /// Publish the values from the in-memory buffers to DNP3 points.
    /// Writing to the points executes asynchronously. This function returns
    /// once the points have been queued to write but in general will return
    /// before the write has actually happened.
    /// @return the number of points that were queued to write.
    std::uint32_t WriteToPoints();

 private:
    /// The outstation that we are subscribed to.
    const std::shared_ptr<asiodnp3::IOutstation> outstation;

    /// The buffers for data transfer.
    const std::shared_ptr<GlueVariables> glue_variables;

    /// The range and offsets into the glue that are valid for this instance.
    const Dnp3Range range;
};

#endif  // CORE_DNP3_PUBLISHER_H_
