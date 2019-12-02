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

#include <spdlog/spdlog.h>
#include <algorithm>

#include "service_definition.h"
#include "glue.h"
#include "ladder.h"

using namespace std;


void null_binding_handler(const GlueVariablesBinding& binding) {}
void null_handler() {}

ServiceDefinition::ServiceDefinition(const char* name,
                                     service_start_fn start_fn) :
    name(name),
    init_fn(null_binding_handler),
    finalize_fn(null_binding_handler),
    start_fn(start_fn),
    before_cycle_fn(null_handler),
    after_cycle_fn(null_handler),
    running(false),
    thread(0),
    config_buffer()
{}

ServiceDefinition::ServiceDefinition(const char* name,
                                     service_start_fn start_fn,
                                     service_init_fn init_fn) :
    name(name),
    start_fn(start_fn),
    init_fn(init_fn),
    finalize_fn(null_binding_handler),
    before_cycle_fn(null_handler),
    after_cycle_fn(null_handler),
    running(false),
    thread(0),
    config_buffer()
{}

ServiceDefinition::ServiceDefinition(const char* name,
                                     service_start_fn start_fn,
                                     service_init_fn init_fn,
                                     service_finalize_fn finalize_fn) :
    name(name),
    start_fn(start_fn),
    init_fn(init_fn),
    finalize_fn(finalize_fn),
    before_cycle_fn(null_handler),
    after_cycle_fn(null_handler),
    running(false),
    thread(0),
    config_buffer()
{}

ServiceDefinition::ServiceDefinition(const char* name,
                                     service_start_fn start_fn,
                                     service_before_cycle_fn before_cycle_fn,
                                     service_after_cycle_fn fafter_cycle_fn) :
    name(name),
    start_fn(start_fn),
    init_fn(null_binding_handler),
    finalize_fn(null_binding_handler),
    before_cycle_fn(before_cycle_fn),
    after_cycle_fn(fafter_cycle_fn),
    running(false),
    thread(0),
    config_buffer()
{}

void ServiceDefinition::initialize()
{
    GlueVariablesBinding bindings(&bufferLock, OPLCGLUE_GLUE_SIZE,
                                  oplc_glue_vars, OPLCGLUE_MD5_DIGEST);
    this->init_fn(bindings);
}

void ServiceDefinition::finalize()
{
    GlueVariablesBinding bindings(&bufferLock, OPLCGLUE_GLUE_SIZE,
                                  oplc_glue_vars, OPLCGLUE_MD5_DIGEST);
    this->finalize_fn(bindings);
}

void ServiceDefinition::start(const char* config)
{
    // TODO there is a race condition here in creating the thread. This race
    // condition is old so I'm not trying to solve it now.
    if (this->running)
    {
        spdlog::debug("{} cannot start because it is running.", this->name);
        return;
    }

    size_t config_len = strlen(config);
    if (config_len > MAX_INTERACTIVE_CONFIG_SIZE - 1)
    {
        spdlog::warn("{} cannot be started because config is longer than {}.",
                     this->name, MAX_INTERACTIVE_CONFIG_SIZE);
        return;
    }

    // Copy the configuration information into our configuration buffer
    strncpy(this->config_buffer, config,
            min(config_len, MAX_INTERACTIVE_CONFIG_SIZE));

    spdlog::info("Starting service {}", this->name);

    this->running = true;
    pthread_create(&this->thread, NULL, &ServiceDefinition::run_service, this);
    pthread_setname_np(this->thread, this->name);
}

void ServiceDefinition::stop()
{
    // TODO there is a threading issue here with access to the thread
    // and detecting if the service is running.
    if (this->running)
    {
        spdlog::info("Stopping service {}", this->name);
        this->running = false;
        pthread_join(this->thread, nullptr);
    }
    else
    {
        spdlog::debug("Service {} was not running", this->name);
    }
}

void ServiceDefinition::before_cycle()
{
    if (this->running)
    {
        this->before_cycle_fn();
    }
}

void ServiceDefinition::after_cycle()
{
    if (this->running)
    {
        this->after_cycle_fn();
    }
}

void* ServiceDefinition::run_service(void* user_data)
{
    auto service = reinterpret_cast<ServiceDefinition*>(user_data);

    GlueVariablesBinding bindings(&bufferLock, OPLCGLUE_GLUE_SIZE,
                                  oplc_glue_vars, OPLCGLUE_MD5_DIGEST);
    // Start the service. This will not return until the service completes
    // which is normally because the running flag was set to false.
    service->start_fn(bindings, service->running, service->config_buffer);

    service->running = false;

    return nullptr;
}
