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

#include <algorithm>
#include <string>
#include <spdlog/spdlog.h>
#include "ini_util.h"

using namespace std;

namespace oplc
{

const size_t CONFIG_BUFFER_SIZE = 2048;
char ini_path[CONFIG_BUFFER_SIZE] = "../etc/config.ini";

void set_config_path(const char* new_path, size_t count)
{
    spdlog::info("Configuration path set to {}", new_path);
    strncpy(ini_path, new_path, std::min(count, CONFIG_BUFFER_SIZE));
}

const char* get_config_path()
{
    return ini_path;
}

config_stream open_config()
{
    return config_stream(
            new std::ifstream(oplc::get_config_path()),
            [] (std::istream* s)
            {
                reinterpret_cast<std::ifstream*>(s)->close();
                delete s;
            }
        );
}

}  //  namespace oplc
