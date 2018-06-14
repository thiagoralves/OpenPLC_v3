 FUNCTION_BLOCK AVERAGE
   VAR_INPUT
     RUN : BOOL ;      (* 1 = run, 0 = reset *)
     XIN : REAL ;     (* Input variable *)
     N   : INT ;      (* 0 <= N < 128 or manufacturer- *)
   END_VAR            (*      specified maximum value  *)
   VAR_OUTPUT XOUT : REAL ; END_VAR (* Averaged output *)
   VAR SUM  : REAL := 0.0; (* Running sum *)
       FIFO : DELAY ;      (* N-Element FIFO *)
   END_VAR
   SUM := SUM - FIFO.XOUT ;
   FIFO (RUN := RUN , XIN := XIN, N := N) ;
   SUM := SUM + FIFO.XOUT ;
   IF RUN THEN XOUT := SUM/N ;
   ELSE SUM := N*XIN ; XOUT := XIN ;
   END_IF ;
 END_FUNCTION_BLOCK

