# DEL - Data Exchange Layer

**Bidirectional Zero-Copy IPC for OpenPLC v3 via IceOryx2**

## Overview

The **Data Exchange Layer (DEL)** extends OpenPLC v3 with two new IEC 61131-3
keywords: `PUBLISH` and `SUBSCRIBE`. These allow PLC variables to be shared with
external processes via IceOryx2 shared memory - no daemon, no broker, no network
overhead.

```
+-------------------+                      +-------------------+
|   OpenPLC v3      |   IceOryx2 SHM       |   POC-US Server   |
|                   |   (Zero-Copy)        |   / HMI / Logger  |
|  PUBLISH 'Motors' +--------------------->| subscribe(Motors) |
|  PUBLISH 'Status' +--------------------->| subscribe(Status) |
|                   |                      |                   |
|  SUBSCRIBE 'CAN'  |<--------------------+| publish(CAN)      |
+-------------------+                      +-------------------+
```

**Directions:**

| Keyword     | Data Flow            | Description                                |
|-------------|----------------------|--------------------------------------------|
| `PUBLISH`   | PLC -> External      | Send PLC outputs to external subscribers   |
| `SUBSCRIBE` | External -> PLC      | Receive external data as PLC inputs        |

## IEC 61131-3 Syntax

### PUBLISH - Send variables to external processes

```iec
VAR_GLOBAL PUBLISH '<GroupName>'
  <variable_declarations>
END_VAR
```

### SUBSCRIBE - Receive variables from external processes

```iec
VAR_GLOBAL SUBSCRIBE '<GroupName>'
  <variable_declarations>
END_VAR
```

### Combining with RETAIN

```iec
VAR_GLOBAL RETAIN PUBLISH '<GroupName>'
  <variable_declarations>
END_VAR
```

### Complete Example

```iec
CONFIGURATION config0
  RESOURCE resource0 ON PLC

    (* Outputs: visible to external subscribers *)
    VAR_GLOBAL PUBLISH 'Motors'
      motor1_speed   : INT  AT %QW0;
      motor2_speed   : INT  AT %QW1;
      motor1_running : BOOL AT %QX0.0;
      motor2_running : BOOL AT %QX0.1;
    END_VAR

    (* Inputs: fed by external CAN bus service *)
    VAR_GLOBAL SUBSCRIBE 'CANBus'
      temperature    : INT  AT %IW10;
      pressure       : INT  AT %IW11;
      rpm_feedback   : DINT AT %ID0;
    END_VAR

    (* Normal variables: unchanged, not shared *)
    VAR_GLOBAL
      local_counter  : DINT AT %MD0;
      error_code     : INT  AT %MW0;
    END_VAR

    (* Retentive + Published: survives power cycle AND is shared *)
    VAR_GLOBAL RETAIN PUBLISH 'Status'
      total_runtime  : DINT AT %MD10;
      fault_count    : INT  AT %MW10;
    END_VAR

    TASK task0(INTERVAL := T#50ms, PRIORITY := 0);
    PROGRAM instance0 WITH task0 : motor_control;
  END_RESOURCE
END_CONFIGURATION
```

## Architecture

### Data Flow Pipeline

```
ST source file (.st)
  |
  | (1) MatIEC Compiler: recognizes PUBLISH/SUBSCRIBE
  v
LOCATED_VARIABLES.h
  __LOCATED_VAR(INT,__QW0,Q,W,0,PUBLISH,Motors)
  __LOCATED_VAR(INT,__IW10,I,W,10,SUBSCRIBE,CANBus)
  __LOCATED_VAR(DINT,__MD0,M,D,0)          <-- normal, no tag
  |
  |-- glue_generator: unchanged (ignores extra params)
  |-- iceoryx2_generator: produces DEL bridge code
  |     |-> iceoryx2_structs.h   (POD structs per group)
  |     |-> iceoryx2_bridge.cpp  (init, publish, subscribe)
  v
Runtime (main.cpp):
  LOCK
    iceoryx2_subscribe_all()   <-- BEFORE PLC logic
    config_run__()             <-- PLC program
    iceoryx2_publish_all()     <-- AFTER PLC logic
  UNLOCK
```

### Generated Structs

For each group, the `iceoryx2_generator` produces a POD struct:

```cpp
// Auto-generated from VAR_GLOBAL PUBLISH 'Motors'
struct IoxGroup_Motors {
    int16_t  QW0;    // motor1_speed
    int16_t  QW1;    // motor2_speed
    uint8_t  QX0_0;  // motor1_running
    uint8_t  QX0_1;  // motor2_running
} __attribute__((packed));

// Auto-generated from VAR_GLOBAL SUBSCRIBE 'CANBus'
struct IoxGroup_CANBus {
    int16_t  IW10;   // temperature
    int16_t  IW11;   // pressure
    int32_t  ID0;    // rpm_feedback
} __attribute__((packed));
```

### IceOryx2 Service Naming

```
OpenPLC/Descriptor  -> Metadata (self-description)
OpenPLC/Motors      -> IoxGroup_Motors (PUBLISH)
OpenPLC/Status      -> IoxGroup_Status (PUBLISH)
OpenPLC/CANBus      -> IoxGroup_CANBus (SUBSCRIBE)
```

## Descriptor Topic (Self-Description)

The DEL automatically publishes a **descriptor topic** on `OpenPLC/Descriptor`.
This allows generic gateways to discover struct layouts at runtime **without**
needing the generated headers.

```
OpenPLC/Descriptor
  |
  +-- version: 1
  +-- program_name: "motor_control.st"
  +-- cycle_time_us: 50000
  +-- groups[]:
       +-- [0] name: "Motors"
       |       direction: "PUBLISH"
       |       service_name: "OpenPLC/Motors"
       |       struct_size: 6
       |       fields[]:
       |         - QW0 : INT (%QW0) offset=0 size=2
       |         - QW1 : INT (%QW1) offset=2 size=2
       |         - QX0_0 : BOOL (%QX0.0) offset=4 size=1
       |         - QX0_1 : BOOL (%QX0.1) offset=5 size=1
       |
       +-- [1] name: "CANBus"
               direction: "SUBSCRIBE"
               service_name: "OpenPLC/CANBus"
               struct_size: 8
               fields[]:
                 - IW10 : INT (%IW10) offset=0 size=2
                 - IW11 : INT (%IW11) offset=2 size=2
                 - ID0 : DINT (%ID0) offset=4 size=4
```

### Generic Gateway (no compile-time headers needed)

```cpp
// 1. Subscribe to descriptor
auto desc = subscribe("OpenPLC/Descriptor");

// 2. Iterate groups
for (auto& group : desc.groups) {
    if (strcmp(group.direction, "PUBLISH") == 0) {
        // 3. Dynamically subscribe to data topic
        auto data = subscribe_raw(group.service_name, group.struct_size);

        // 4. Interpret fields via offset/size
        for (auto& field : group.fields) {
            void* ptr = (uint8_t*)data + field.offset;
            // Read value according to field.size
        }
    }
}
```

### Re-Publish on Program Change

When a new ST program is compiled and started, `iceoryx2_init()` publishes a new
descriptor with an incremented `version`. Subscribers detect the change and can
reconfigure themselves.

## IEC Data Type Mapping

| IEC Type | C Type     | Size    |
|----------|------------|---------|
| BOOL     | uint8_t    | 1 byte  |
| BYTE     | uint8_t    | 1 byte  |
| SINT     | int8_t     | 1 byte  |
| USINT    | uint8_t    | 1 byte  |
| INT      | int16_t    | 2 bytes |
| UINT     | uint16_t   | 2 bytes |
| WORD     | uint16_t   | 2 bytes |
| DINT     | int32_t    | 4 bytes |
| UDINT    | uint32_t   | 4 bytes |
| DWORD    | uint32_t   | 4 bytes |
| LINT     | int64_t    | 8 bytes |
| ULINT    | uint64_t   | 8 bytes |
| LWORD    | uint64_t   | 8 bytes |
| REAL     | float      | 4 bytes |
| LREAL    | double     | 8 bytes |

## Build Requirements

- **Rust toolchain** (for building the IceOryx2 core library)
- **CMake >= 3.22**
- **C++17 compiler** (g++ or clang++)
- IceOryx2 as git submodule in `utils/iceoryx2_src/`

## Files

```
utils/
  iceoryx2_src/                     Git submodule (IceOryx2)
  iceoryx2_generator_src/
    iceoryx2_generator.cpp          DEL bridge generator
  matiec_src/                       Extended IEC compiler

webserver/core/
  main.cpp                          Runtime with DEL hooks
  iceoryx2_structs.h                (auto-generated)
  iceoryx2_bridge.h                 (auto-generated)
  iceoryx2_bridge.cpp               (auto-generated)

documentation/DEL-Data-Exchange-Layer/
  README.md                         This file
  examples/
    motor_control.st                Example ST program
    poc_subscriber.cpp              Example external subscriber
```
