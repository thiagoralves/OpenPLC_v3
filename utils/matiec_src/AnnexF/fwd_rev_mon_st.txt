FUNCTION_BLOCK FWD_REV_MON
VAR_INPUT AUTO : BOOL ;(* Enable automated commands *)
  ACK : BOOL ;         (* Acknowledge/cancel all alarms *)
  AUTO_FWD : BOOL ;    (* Automated forward command *)
  MAN_FWD : BOOL ;     (* Manual forward command *)
  MAN_FWD_CHK : BOOL ; (* Negated MAN_FWD for debouncing *)
  T_FWD_MAX : TIME ;  (* Maximum time from FWD_CMD to FWD_FDBK *)
  FWD_FDBK : BOOL ;    (* Confirmation of FWD_CMD completion *)
                       (*   by operative unit *)
  AUTO_REV : BOOL ;    (* Automated reverse command *)
  MAN_REV : BOOL ;     (* Manual reverse command *)
  MAN_REV_CHK : BOOL ; (* Negated MAN_REV for debouncing *)
  T_REV_MAX : TIME ;  (* Maximum time from REV_CMD to REV_FDBK *)  
  REV_FDBK : BOOL ;    (* Confirmation of REV_CMD completion *)
END_VAR                (*    by operative unit *)
VAR_OUTPUT KLAXON : BOOL ;      (* Any alarm active *)
  FWD_REV_ALRM : BOOL; (* Forward/reverse command conflict *)
  FWD_CMD : BOOL ;     (* "Forward" command to operative unit *)
  FWD_ALRM : BOOL ;    (* T_FWD_MAX expired without FWD_FDBK *)
  REV_CMD : BOOL ;     (* "Reverse" command to operative unit *)
  REV_ALRM : BOOL ;    (* T_REV_MAX expired without REV_FDBK *)
END_VAR
VAR FWD_MON : CMD_MONITOR; (* "Forward" command monitor *)
  REV_MON : CMD_MONITOR;   (* "Reverse" command monitor *)
  FWD_REV_FF : SR ;      (* Forward/Reverse contention latch *)
END_VAR
(* Function Block body *)
(* Evaluate internal function blocks *)
  FWD_MON    (AUTO_MODE   := AUTO,
              ACK         := ACK,
              AUTO_CMD    := AUTO_FWD,
              MAN_CMD     := MAN_FWD,
              MAN_CMD_CHK := MAN_FWD_CHK,
              T_CMD_MAX   := T_FWD_MAX,
              FDBK        := FWD_FDBK);
  REV_MON    (AUTO_MODE   := AUTO,
              ACK         := ACK,
              AUTO_CMD    := AUTO_REV,
              MAN_CMD     := MAN_REV,
              MAN_CMD_CHK := MAN_REV_CHK,
              T_CMD_MAX   := T_REV_MAX,
              FDBK        := REV_FDBK);
  FWD_REV_FF (S1 := FWD_MON.CMD & REV_MON.CMD, R := ACK); 
(* Transfer data to outputs *)
  FWD_REV_ALRM := FWD_REV_FF.Q1;
  FWD_CMD := FWD_MON.CMD & NOT FWD_REV_ALRM;
  FWD_ALRM := FWD_MON.ALRM;
  REV_CMD := REV_MON.CMD & NOT FWD_REV_ALRM;
  REV_ALRM := REV_MON.ALRM;
  KLAXON := FWD_ALRM OR REV_ALRM OR FWD_REV_ALRM;

END_FUNCTION_BLOCK

