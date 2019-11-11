// Copyright 2018 Thiago Alves
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

#include <string>
#include <vector>
#ifdef __linux__
#include <pthread.h>
#include <sys/mman.h>
#endif  // __linux__

#include <spdlog/spdlog.h>
#include <ini.h>

#include "ini_util.h"
#include "ladder.h"
#include "service/service_definition.h"
#include "service/service_registry.h"

// Bootstrap is what we need to do in order to start the runtime. This
// handles initializing glue and reading configuration to start up the
// runtime according to the config.

struct PlcConfig {
    /// A list of services that we will enable once the runtime has
    /// started.
    std::vector<std::string> services;
};

/// Handle reading values from the configuration file
int config_handler(void* user_data, const char* section,
                   const char* name, const char* value) {

    auto config = reinterpret_cast<PlcConfig*>(user_data);

    if (ini_matches("logging", "level", section, name)) {
        if (strcmp(value, "trace") == 0) {
            spdlog::set_level(spdlog::level::trace);
        } else if (strcmp(value, "debug") == 0) {
            spdlog::set_level(spdlog::level::debug);
        } else if (strcmp(value, "info") == 0) {
            spdlog::set_level(spdlog::level::info);
        } else if (strcmp(value, "warn") == 0) {
            spdlog::set_level(spdlog::level::warn);
        } else if (strcmp(value, "error") == 0) {
            spdlog::set_level(spdlog::level::err);
        }
    } else if (strcmp("enabled", name) == 0 && ini_atob(value) && services_find(section)) {
        // This is the name of service that we can enable, so add it
        // to the list of services that we will enable.
        config->services.push_back(section);
    }
}

/// Initialize the PLC runtime
void bootstrap() {
    //======================================================
    //                 BOOSTRAP CONFIGURATION
    //======================================================

    // Read the configuration file to initialize which features
    // we will have available on the PLC. This should be the
    // first thing we do because it may change the logging level
    // and we want that to happen early on.
    PlcConfig config;

    // We just assume that the file we are reading with the
    // configuration information in in the etc subfolder and use
    // a relative path to find it.
    const char* config_path = "../etc/config.ini";
    if (ini_parse(config_path, config_handler, &config) < 0) {
        spdlog::info("Config file {} could not be read", config_path);
    }

    //======================================================
    //                 PLC INITIALIZATION
    //======================================================

    // Defined by the MATIEC output to initialize itself
    config_init__();
    // Initialize the binding between located variables and
    // our internal buffers. Required for items that depend on
    // the sized-array representations.
    glueVars();

    //======================================================
    //                 HARDWARE INITIALIZATION
    //======================================================

    // Perform any hardware specific initialization that is required. This is
    // a standard part the platform where we have implemented capabilities
    // for specific hardware targes.
    initializeHardware();
    initializeMB();
    // User provided logic that runs on initialization.
    initCustomLayer();
    updateBuffersIn();
    updateCustomIn();
    updateBuffersOut();
    updateCustomOut();
    glueVars();
    mapUnusedIO();

    //======================================================
    //                 SERVICE INITIALIZATION
    //======================================================

    // Initializes any services that is known and wants to participate
    // in bootstrapping.
    services_init();

#ifdef __linux__
    //======================================================
    //              REAL-TIME INITIALIZATION
    //======================================================
    // Set our thread to real time priority
    struct sched_param sp;
    sp.sched_priority = 30;
    spdlog::info("Setting main thread priority to RT");
    if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp))
    {
        spdlog::warn("WARNING: Failed to set main thread to real-time priority");
    }

    // Lock memory to ensure no swapping is done.
    spdlog::info("Locking main thread memory");
    if(mlockall(MCL_FUTURE|MCL_CURRENT))
    {
        spdlog::warn("WARNING: Failed to lock memory");
    }
#endif

    //======================================================
    //              SERVICE START
    //======================================================

    // Our next step here is to start the main loop, so start any
    // services that we want now.

    for (auto it = config.services.begin(); it != config.services.end(); ++it)
    {
        const char* service_config = "";
        ServiceDefinition* def = services_find(it->c_str());
        def->start(service_config);
    }
}
