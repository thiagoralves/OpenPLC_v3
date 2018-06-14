FUNCTION_BLOCK STACK_INT
  VAR_INPUT PUSH, POP: BOOL R_EDGE; (* Basic stack operations *)
            R1 : BOOL ;          (* Over-riding reset *)
            IN : INT ;           (* Input to be pushed *)
            N  : INT ;           (* Maximum depth after reset *)
  END_VAR
  VAR_OUTPUT EMPTY : BOOL := 1 ;     (* Stack empty *)
             OFLO  : BOOL := 0 ;     (* Stack overflow *)
             OUT   : INT  := 0 ;     (* Top of stack data *)
  END_VAR
  VAR STK : ARRAY[0..127] OF INT; (* Internal stack *)
      NI : INT :=128  ;           (* Storage for N upon reset *)
      PTR : INT := -1 ;           (* Stack pointer *)
  END_VAR
    (* Function Block body *)
    IF R1 THEN
       OFLO := 0; EMPTY := 1; PTR := -1;
       NI := LIMIT (MN:=1,IN:=N,MX:=128); OUT := 0;
    ELSIF POP & NOT EMPTY THEN
       OFLO := 0; PTR := PTR-1; EMPTY := PTR < 0;
       IF EMPTY THEN OUT := 0;
       ELSE OUT := STK[PTR];
       END_IF ;
    ELSIF PUSH & NOT OFLO THEN
       EMPTY := 0; PTR := PTR+1; OFLO := (PTR = NI);
       IF NOT OFLO THEN OUT := IN ; STK[PTR] := IN;
       ELSE OUT := 0;
       END_IF ;
    END_IF ;

  END_FUNCTION_BLOCK
