FUNCTION WEIGH : WORD     (* BCD encoded *)
  VAR_INPUT  (* "EN" input is used to indicate "scale ready" *)
    weigh_command : BOOL;
    gross_weight : WORD ; (* BCD encoded *)
    tare_weight : INT ;
  END_VAR
(* Function Body *)
IF weigh_command THEN
  WEIGH := INT_TO_BCD (BCD_TO_INT(gross_weight) - tare_weight);
END_IF ;

END_FUNCTION                     (* Implicit "ENO" *)
