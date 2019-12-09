PROGRAM prog0
  VAR
    var_in : BOOL;
    var_out : BOOL;
  END_VAR

  var_out := var_in;
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK Main(INTERVAL := T#50ms,PRIORITY := 0);
    PROGRAM Inst0 WITH Main : prog0;
  END_RESOURCE
END_CONFIGURATION
