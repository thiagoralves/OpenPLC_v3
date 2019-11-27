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

#ifndef RUNTIME_CORE_SERVICE_SERVICE_DEFINITION_H_
#define RUNTIME_CORE_SERVICE_SERVICE_DEFINITION_H_

#include <pthread.h>
#include <cstdint>

/** \addtogroup openplc_runtime
 *  @{
 */

class GlueVariablesBinding;

const std::size_t MAX_INTERACTIVE_CONFIG_SIZE(1024);

typedef void (*service_init_fn) (const GlueVariablesBinding& binding);
typedef void (*service_finalize_fn) (const GlueVariablesBinding& binding);
typedef void (*service_start_fn) (const GlueVariablesBinding& binding, volatile bool& run, const char* config);
typedef void (*service_before_cycle_fn) ();
typedef void (*service_after_cycle_fn) ();

/// A service is the primary extension point for adding support for new
/// protocols or hardware specific capabilities. You should not implement
/// your service in this class. Rather, this is how the runtime learns
/// about your service and you provide to the service definition a set
/// of functions that that your service uses.
///
/// All services run in their own thread and this mechanism is responsible
/// for creating that thread. This further means that each service, must
/// keep only a very short lock on the glue variables so that it cannot
/// prevent the main PLC loop from accessing the variables.
///
/// Services can also participate with the runtime with the before and after
/// cycle functions. These functions do not receive the mutex because the
/// PLC cycle already has the mutex ownership.
///
/// @note There is presently no way to pass state from init to finalize
/// or from start to stop. That's only because we haven't had such a need
/// yet. If that comes up, then we'll add that.
class ServiceDefinition final
{
 public:
    /// Initialize a new instance of a service definition that can be started
    /// and stopped but does not participate in initialize or finalize.
    /// @param name The unique name of this service.
    /// @param start_fn A function to start the service.
    ServiceDefinition(const char* name, service_start_fn start_fn);

    /// Initialize a new instance of a service definition that can be started
    /// and stopped and participates in initialize.
    /// @param name The unique name of this service.
    /// @param start_fn A function to start the service.
    /// @param init_fn A function to run when the runtime initializes.
    ServiceDefinition(const char* name, service_start_fn start_fn,
                      service_init_fn init_fn);

    /// Initialize a new instance of a service that participates in all
    /// lifecycle events (initialize, finalize, start, stop).
    /// @param name The unique name of this service.
    /// @param start_fn A function to start the service.
    /// @param init_fn A function to run when the runtime initializes.
    /// @param finalize_fn A function to run when the runtime finalizes.
    ServiceDefinition(const char* name, service_start_fn start_fn,
                      service_init_fn init_fn,
                      service_finalize_fn finalize_fn);

    /// Initialize a new instance of a service that participates in the start
    /// lifecycle and in cycle events.
    /// @param name The unique name of this service.
    /// @param start_fn A function to start the service.
    /// @param before_cycle_fn A function to run before each PLC cycle.
    /// @param after_cycle_fn A function to run after each PLC cycle.
    ServiceDefinition(const char* name, service_start_fn start_fn,
                      service_before_cycle_fn before_cycle_fn,
                      service_after_cycle_fn fafter_cycle_fn);

    /// Lifecycle method for when the runtime starts. This is called for
    /// the service before the runtime loop beings. This does not mean
    /// that the service will or will not be started later.
    void initialize();
    /// Lifecycle method for when the runtime finalizes. This is called
    /// after the runtime loop stops.
    void finalize();

    /// Lifecycle method for when this service has been started on demand.
    void start(const char* config);
    /// Lifecycle method for when this service has been stopped on demand.
    void stop();

    void before_cycle();
    void after_cycle();

    /// Get the descriptive identifier for this service type.
    const char* id() const { return this->name; }

    /// Get the configuration information associated with starting this
    /// service.
    const char* config() const { return this->config_buffer; }

 private:
    // Hide the copy constructor
    ServiceDefinition(ServiceDefinition &);
    static void* run_service(void* user_data);

 private:
    /// The type name of the service.
    const char* name;
    /// The function to initialize the service.
    service_init_fn init_fn;
    /// The function to finalize the service.
    service_finalize_fn finalize_fn;
    /// The function to start the service.
    service_start_fn start_fn;
    /// The function to run before the PLC cycle.
    service_before_cycle_fn before_cycle_fn;
    /// The function to run after the PLC cycle.
    service_after_cycle_fn after_cycle_fn;
    /// Is the service running.
    volatile bool running;
    /// The thread the service is running on.
    pthread_t thread;
    /// A buffer for holding the configuration information that was part
    /// of the request to start the service.
    char config_buffer[MAX_INTERACTIVE_CONFIG_SIZE];
};

/** @}*/

#endif  // RUNTIME_CORE_SERVICE_SERVICE_DEFINITION_H_
