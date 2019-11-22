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

#include "logsink.h"

#include "catch.hpp"

using Catch::Matchers::Contains;
using namespace spdlog;

SCENARIO("logsink", "")
{
    GIVEN("initialized sink")
    {
        unsigned char buffer[50];
        buffer[0] = '\0';
        auto sink = std::make_shared<buffered_sink>(buffer, 50);
        sink->set_pattern("%v");

        WHEN("get data before any messages")
        {
            std::string data = sink->data();
            REQUIRE(data.size() == 0);
        }

        WHEN("has one message published then returns the message")
        {
            std::string logger_name = "test";

            fmt::memory_buffer buf;
            fmt::format_to(buf, "Hello");
            details::log_msg msg(&logger_name, level::info,
                                 string_view_t(buf.data(), buf.size()));

            sink->log(msg);
            std::string data = sink->data();
            REQUIRE_THAT(data, Contains("Hello\n"));
        }

        WHEN("has two messages published then return both messages")
        {
            std::string logger_name = "test";

            fmt::memory_buffer buf1;
            fmt::format_to(buf1, "Hello");
            details::log_msg msg1(&logger_name, level::info,
                                  string_view_t(buf1.data(), buf1.size()));

            fmt::memory_buffer buf2;
            fmt::format_to(buf2, "There");
            details::log_msg msg2(&logger_name, level::info,
                                  string_view_t(buf2.data(), buf2.size()));

            sink->log(msg1);
            sink->log(msg2);
            std::string data = sink->data();
            REQUIRE_THAT(data, Contains("Hello\n"));
            REQUIRE_THAT(data, Contains("There\n"));
        }

        WHEN("has two messages published then return both messages")
        {
            std::string logger_name = "test";

            fmt::memory_buffer buf1;
            fmt::format_to(buf1, "Hello");
            details::log_msg msg1(&logger_name, level::info,
                                  string_view_t(buf1.data(), buf1.size()));

            fmt::memory_buffer buf2;
            fmt::format_to(buf2, "There");
            details::log_msg msg2(&logger_name, level::info,
                                  string_view_t(buf2.data(), buf2.size()));

            sink->log(msg1);
            sink->log(msg2);
            std::string data = sink->data();
            REQUIRE_THAT(data, Contains("Hello\n"));
            REQUIRE_THAT(data, Contains("There\n"));
        }

        WHEN("message is longer than buffer size truncates still has newline")
        {
            std::string logger_name = "test";

            fmt::memory_buffer buf;
            fmt::format_to(buf, "012345678901234567890123456789"\
                           "01234567890123456789ABCDEFG");
            details::log_msg msg(&logger_name, level::info,
                                 string_view_t(buf.data(), buf.size()));

            sink->log(msg);
            std::string data = sink->data();
            REQUIRE_THAT(data, Contains("012345678901234567890123456789"\
                         "0123456789012345678"));
        }

        WHEN("multiple messages exhaust buffer then starts from beginning")
        {
            std::string logger_name = "test";

            fmt::memory_buffer buf;
            fmt::format_to(buf, "0123456789");
            details::log_msg msg(&logger_name, level::info,
                                 string_view_t(buf.data(), buf.size()));

            fmt::memory_buffer buf2;
            fmt::format_to(buf2, "ABCDEFG");
            details::log_msg msg2(&logger_name, level::info,
                                  string_view_t(buf2.data(), buf2.size()));

            sink->log(msg);
            sink->log(msg);
            sink->log(msg);
            sink->log(msg);
            sink->log(msg);
            sink->log(msg2);
            std::string data = sink->data();
            REQUIRE_THAT(data, Contains("ABCDEFG\n"));
        }
    }
}