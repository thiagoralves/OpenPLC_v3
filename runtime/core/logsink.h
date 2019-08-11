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

#ifndef CORE_LOGSINK_H
#define CORE_LOGSINK_H

#include <spdlog/sinks/base_sink.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>

/** \addtogroup openplc_runtime
 *  @{
 */

////////////////////////////////////////////////////////////////////////////////
/// \brief Implements a fixed-size in-memory buffer for log messages.
///
/// This buffer automatically resets itself when it fills up. This sink allows
/// use to query logs in memory so that they can be provided to a front
/// end in an efficient (but not necessarily complete) manner.
////////////////////////////////////////////////////////////////////////////////
class buffered_sink : public spdlog::sinks::base_sink <std::mutex>
{
public:
	/// \brief Initialize a new instance of the sink with the provided buffer.
	///
	/// We require supplying the buffer as an argument so that in normal use
	/// this can be statically allocated, but for the purpose of testing
	/// the size is easy to configure.
	/// \param buffer The buffer to write this. this sink maintains a mutex
	///               will only read/write the buffer when the lock is acquired.
	/// \param buffer_size The number of characters in the buffer.
	buffered_sink(unsigned char* buffer, std::uint32_t buffer_size) :
		buffer(buffer),
		buffer_size(buffer_size)
	{}

    /// \brief Gets the data from this sink.
	/// \return The data formatted as a string.
    std::string data() {
        return std::string(reinterpret_cast<char*>(this->buffer));
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        fmt::memory_buffer formatted;
        sink::formatter_->format(msg, formatted);

        std::size_t msg_size = formatted.size();

        // Where will our message end?
        std::size_t next_end_pos = end_pos + msg_size;
        if (next_end_pos > buffer_size - 1) {
            // If adding this message would put us past the end
            // of the buffer, then start back at the beginning.
            end_pos = 0;
        }

        // Add this message onto the buffer
		std::size_t copy_len = std::min(msg_size, buffer_size - end_pos - 1);
        std::memcpy(buffer + end_pos, formatted.data(), copy_len);
		end_pos += copy_len;
		// Just make sure that the string is null terminated so we don't run off the end.
        buffer[end_pos + 1] = '\0';
    }

    void flush_() override
    {}

private:
    
	// The current end of the string position. This is where we start inserting.
    std::size_t end_pos = 0;

	// The buffer that we are writing to.
    unsigned char* const buffer;

	// The size of the buffer in characters.
	const std::size_t buffer_size;;
};

#endif // CORE_LOGSINK_H

/** @}*/
