/**
 * POC-US Subscriber Example
 *
 * This standalone program demonstrates how to:
 * 1. Subscribe to the OpenPLC Descriptor topic to discover available groups
 * 2. Subscribe to a PUBLISH group ("Motoren") and read motor speeds
 * 3. Publish test data to a SUBSCRIBE group ("CANBus") to feed the PLC
 *
 * Build:
 *   cmake -S . -B build && cmake --build build
 *
 * Run:
 *   ./build/poc_subscriber
 *
 * Prerequisites:
 *   - IceOryx2 library installed
 *   - OpenPLC runtime running with motor_control.st loaded
 */

#include "iox2/iceoryx2.hpp"

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <thread>
#include <chrono>

using namespace iox2;

/* ----------------------------------------------------------------
 * These structs match what iceoryx2_generator produces from the
 * PUBLISH/SUBSCRIBE declarations in motor_control.st.
 *
 * In a real generic gateway, you would NOT need these - you'd use
 * the Descriptor topic to discover the layout at runtime.
 * ---------------------------------------------------------------- */

// Matches: VAR_GLOBAL PUBLISH 'Motoren'
struct IoxGroup_Motoren {
    int16_t  QW0;    // motor1_speed
    int16_t  QW1;    // motor2_speed
    uint8_t  QX0_0;  // motor1_running
    uint8_t  QX0_1;  // motor2_running
} __attribute__((packed));

// Matches: VAR_GLOBAL SUBSCRIBE 'CANBus'
struct IoxGroup_CANBus {
    int16_t  IW10;   // temperature (0.1 deg C)
    int16_t  IW11;   // pressure (mbar)
    int32_t  ID0;    // rpm_feedback
} __attribute__((packed));

// Matches: VAR_GLOBAL RETAIN PUBLISH 'Status'
struct IoxGroup_Status {
    int32_t  MD10;   // total_runtime (seconds)
    int16_t  MW10;   // fault_count
} __attribute__((packed));


/* ----------------------------------------------------------------
 * Descriptor structs (auto-generated, fixed max sizes)
 * ---------------------------------------------------------------- */

#define IOX_MAX_GROUPS    16
#define IOX_MAX_FIELDS    32
#define IOX_MAX_NAME_LEN  64

struct IoxFieldDescriptor {
    char     name[IOX_MAX_NAME_LEN];
    char     iec_type[16];
    char     iec_address[16];
    uint16_t offset;
    uint16_t size;
};

struct IoxGroupDescriptor {
    char     name[IOX_MAX_NAME_LEN];
    char     service_name[IOX_MAX_NAME_LEN];
    char     direction[12];
    uint16_t field_count;
    uint16_t struct_size;
    IoxFieldDescriptor fields[IOX_MAX_FIELDS];
};

struct IoxDescriptor {
    uint32_t version;
    uint32_t group_count;
    uint32_t cycle_time_us;
    char     program_name[IOX_MAX_NAME_LEN];
    IoxGroupDescriptor groups[IOX_MAX_GROUPS];
};


/* ----------------------------------------------------------------
 * Main: Three modes of operation demonstrated
 * ---------------------------------------------------------------- */

int main() {
    printf("=== OpenPLC IceOryx2 POC Subscriber ===\n\n");

    // Create an IceOryx2 node (no daemon needed!)
    auto node_result = NodeBuilder().create<ServiceType::Ipc>();
    if (!node_result.has_value()) {
        printf("ERROR: Failed to create IceOryx2 node\n");
        return 1;
    }
    auto node = std::move(node_result.value());


    /* --- 1. Read the Descriptor topic --- */
    printf("[1] Subscribing to OpenPLC/Descriptor...\n");
    {
        auto svc = node.service_builder(
                ServiceName::create("OpenPLC/Descriptor").value())
            .publish_subscribe<IoxDescriptor>()
            .open_or_create();

        if (svc.has_value()) {
            auto sub = svc.value().subscriber_builder().create();
            if (sub.has_value()) {
                // Wait briefly for the PLC to publish the descriptor
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                auto sample = sub.value().receive();
                if (sample.has_value() && sample->has_value()) {
                    const auto& desc = sample->value().payload();
                    printf("    Program: %s (version %u)\n",
                           desc.program_name, desc.version);
                    printf("    Cycle time: %u us\n", desc.cycle_time_us);
                    printf("    Groups: %u\n", desc.group_count);
                    for (uint32_t g = 0; g < desc.group_count; g++) {
                        const auto& grp = desc.groups[g];
                        printf("      [%s] %s -> %s (%u fields, %u bytes)\n",
                               grp.direction, grp.name, grp.service_name,
                               grp.field_count, grp.struct_size);
                        for (uint16_t f = 0; f < grp.field_count; f++) {
                            const auto& fld = grp.fields[f];
                            printf("        - %s : %s (%s, offset=%u, size=%u)\n",
                                   fld.name, fld.iec_type, fld.iec_address,
                                   fld.offset, fld.size);
                        }
                    }
                } else {
                    printf("    (No descriptor received yet - is OpenPLC running?)\n");
                }
            }
        }
    }
    printf("\n");


    /* --- 2. Subscribe to Motoren (typed access) --- */
    printf("[2] Subscribing to OpenPLC/Motoren...\n");
    auto motoren_svc = node.service_builder(
            ServiceName::create("OpenPLC/Motoren").value())
        .publish_subscribe<IoxGroup_Motoren>()
        .open_or_create();

    /* --- 3. Subscribe to Status (typed access) --- */
    printf("[3] Subscribing to OpenPLC/Status...\n");
    auto status_svc = node.service_builder(
            ServiceName::create("OpenPLC/Status").value())
        .publish_subscribe<IoxGroup_Status>()
        .open_or_create();

    /* --- 4. Publish to CANBus (feed PLC inputs) --- */
    printf("[4] Publishing to OpenPLC/CANBus...\n");
    auto canbus_svc = node.service_builder(
            ServiceName::create("OpenPLC/CANBus").value())
        .publish_subscribe<IoxGroup_CANBus>()
        .open_or_create();

    if (!motoren_svc.has_value() || !canbus_svc.has_value() ||
        !status_svc.has_value()) {
        printf("ERROR: Failed to create services\n");
        return 1;
    }

    auto motoren_sub = motoren_svc.value().subscriber_builder().create().value();
    auto status_sub  = status_svc.value().subscriber_builder().create().value();
    auto canbus_pub  = canbus_svc.value().publisher_builder().create().value();


    /* --- Main loop: read PLC outputs, send simulated sensor data --- */
    printf("\n=== Running (Ctrl+C to stop) ===\n\n");

    int16_t sim_temperature = 250;  // Start at 25.0 deg C
    int16_t sim_pressure = 1013;    // 1013 mbar
    int32_t sim_rpm = 0;
    int cycle = 0;

    while (true) {
        // Simulate slowly rising temperature
        if (cycle % 100 == 0 && sim_temperature < 900) {
            sim_temperature += 5;  // +0.5 deg C every 10 seconds
        }

        // Publish simulated CAN bus sensor data to PLC
        auto loan = canbus_pub.loan_uninit();
        if (loan.has_value()) {
            IoxGroup_CANBus data;
            data.IW10 = sim_temperature;
            data.IW11 = sim_pressure;
            data.ID0  = sim_rpm;
            auto initialized = loan.value().write_payload(data);
            send(std::move(initialized)).expect("send failed");
        }

        // Read motor outputs from PLC
        auto m_sample = motoren_sub.receive();
        if (m_sample.has_value() && m_sample->has_value()) {
            const auto& m = m_sample->value().payload();
            sim_rpm = m.QW0 > 0 ? m.QW0 : 0;  // Use setpoint as feedback
            printf("Motors: speed1=%d speed2=%d running1=%d running2=%d | ",
                   m.QW0, m.QW1, m.QX0_0, m.QX0_1);
        }

        // Read status from PLC
        auto s_sample = status_sub.receive();
        if (s_sample.has_value() && s_sample->has_value()) {
            const auto& s = s_sample->value().payload();
            printf("Runtime=%ds Faults=%d | ", s.MD10, s.MW10);
        }

        printf("Sim: temp=%.1f C pressure=%d mbar rpm=%d\n",
               sim_temperature / 10.0, sim_pressure, sim_rpm);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        cycle++;
    }

    return 0;
}
