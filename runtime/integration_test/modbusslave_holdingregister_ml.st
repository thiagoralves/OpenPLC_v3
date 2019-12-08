PROGRAM program0
  VAR
    HoldingRegisterInt AT %ML0 : LINT;
  END_VAR

  HoldingRegisterInt := HoldingRegisterInt;
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : program0;
  END_RESOURCE
END_CONFIGURATION
