(* ==========================================================================
 * Example: Motor Control with IceOryx2 PUBLISH/SUBSCRIBE
 *
 * This program demonstrates the new PUBLISH and SUBSCRIBE keywords.
 *
 * PUBLISH 'Motoren':  Publishes motor speed outputs via IceOryx2.
 *                     External HMI or logger can subscribe to see values.
 *
 * SUBSCRIBE 'CANBus': Receives sensor data from an external CAN bus manager
 *                     that publishes temperature and pressure via IceOryx2.
 *
 * Normal variables (without PUBLISH/SUBSCRIBE) work exactly as before.
 *
 * NOTE: In IEC 61131-3 CONFIGURATION syntax, the location AT xxxx
 *       comes BEFORE the colon:  var_name AT %QW0 : INT;
 * ========================================================================== *)

PROGRAM motor_control

VAR
  temp_limit      : INT  := 800;    (* 80.0 deg C shutdown threshold *)
  temp_warning    : INT  := 600;    (* 60.0 deg C warning threshold *)
  speed_setpoint  : INT  := 1500;   (* Default speed: 1500 RPM *)
  overheat        : BOOL := FALSE;
END_VAR

VAR_EXTERNAL
  motor1_speed   : INT;
  motor2_speed   : INT;
  motor1_running : BOOL;
  motor2_running : BOOL;
  temperature    : INT;
  pressure       : INT;
  rpm_feedback   : DINT;
  local_counter  : DINT;
  error_code     : INT;
  total_runtime  : DINT;
  fault_count    : INT;
END_VAR

(* --- Main Logic --- *)

(* Increment cycle counter *)
local_counter := local_counter + 1;

(* Check temperature from CAN bus subscriber *)
IF temperature > temp_limit THEN
  overheat := TRUE;
  error_code := 1;    (* Overheat fault *)
  fault_count := fault_count + 1;
ELSIF temperature < temp_warning THEN
  overheat := FALSE;
  error_code := 0;
END_IF;

(* Control motors based on temperature *)
IF NOT overheat THEN
  motor1_running := TRUE;
  motor2_running := TRUE;
  motor1_speed := speed_setpoint;
  motor2_speed := speed_setpoint;
ELSE
  motor1_running := FALSE;
  motor2_running := FALSE;
  motor1_speed := 0;
  motor2_speed := 0;
END_IF;

(* Track total runtime -- increments every 50ms cycle *)
IF motor1_running OR motor2_running THEN
  (* Every 20 cycles = 1 second at 50ms cycle time *)
  IF (local_counter MOD 20) = 0 THEN
    total_runtime := total_runtime + 1;
  END_IF;
END_IF;

END_PROGRAM


CONFIGURATION config0
  RESOURCE resource0 ON PLC

    (* --- Published outputs: visible to external subscribers --- *)
    VAR_GLOBAL PUBLISH 'Motoren'
      motor1_speed   AT %QW0   : INT;   (* Motor 1 speed setpoint *)
      motor2_speed   AT %QW1   : INT;   (* Motor 2 speed setpoint *)
      motor1_running AT %QX0.0 : BOOL;  (* Motor 1 running status *)
      motor2_running AT %QX0.1 : BOOL;  (* Motor 2 running status *)
    END_VAR

    (* --- Subscribed inputs: fed by external CAN bus service --- *)
    VAR_GLOBAL SUBSCRIBE 'CANBus'
      temperature    AT %IW10  : INT;   (* Sensor: Temperature in 0.1 deg C *)
      pressure       AT %IW11  : INT;   (* Sensor: Pressure in mbar *)
      rpm_feedback   AT %ID0   : DINT;  (* Sensor: Actual RPM from encoder *)
    END_VAR

    (* --- Normal variables: not shared via IceOryx2 --- *)
    VAR_GLOBAL
      local_counter  AT %MD0   : DINT;  (* Internal cycle counter *)
      error_code     AT %MW0   : INT;   (* Internal error register *)
    END_VAR

    (* --- Retentive + Published: survives power cycle AND is shared --- *)
    VAR_GLOBAL RETAIN PUBLISH 'Status'
      total_runtime  AT %MD10  : DINT;  (* Total runtime in seconds *)
      fault_count    AT %MW10  : INT;   (* Number of faults since install *)
    END_VAR

    TASK task0(INTERVAL := T#50ms, PRIORITY := 0);
    PROGRAM instance0 WITH task0 : motor_control;

  END_RESOURCE
END_CONFIGURATION
