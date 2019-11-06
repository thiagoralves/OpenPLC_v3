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

#ifndef CORE_DNP3S_DNP3_PUBLISHER_H_
#define CORE_DNP3s_DNP3_PUBLISHER_H_

#include <cstdint>
#include <memory>

#include "dnp3.h"
#include "iec_types.h"

/** \addtogroup openplc_runtime
 *  @{
 */

namespace asiodnp3 {
class IOutstation;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief The publisher defines the interface between the glue arrays
/// of variables that are read from PLC application and written to
/// the DNP3 channel. This published all of the available glue
/// information over DNP3, incuding inputs, outputs, memory.
////////////////////////////////////////////////////////////////////////////////
class Dnp3Publisher {
 public:
    /// \brief Constructs a new instance of the publisher object.
    /// @param outstation The outstation that is ourselves
    /// @param measurements The buffers that we use for data transfer
    Dnp3Publisher(
        std::shared_ptr<asiodnp3::IOutstation> outstation,
        const Dnp3MappedGroup& measurements);

    /// \brief Publish the values from the in-memory buffers to DNP3 points.
    ///
    /// Writing to the points executes asynchronously. This function returns
    /// once the points have been queued to write but in general will return
    /// before the write has actually happened.
    /// @return the number of points that were queued to write.
    std::uint32_t ExchangeGlue();

 private:
    /// The outstation that we are subscribed to.
    const std::shared_ptr<asiodnp3::IOutstation> outstation;

    /// The buffers for data transfer.
    const Dnp3MappedGroup& measurements;
};

#endif  // CORE_DNP3S_DNP3_PUBLISHER_H_

/** @}*/