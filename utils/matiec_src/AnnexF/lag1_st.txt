 FUNCTION_BLOCK LAG1
   VAR_INPUT
     RUN : BOOL ;     (* 1 = run, 0 = reset *)
     XIN : REAL ;     (* Input variable *)
     TAU : TIME ;     (* Filter time constant *)
     CYCLE : TIME ;   (* Sampling time interval *)
   END_VAR
   VAR_OUTPUT XOUT : REAL ; END_VAR  (* Filtered output *)
   VAR K : REAL ;     (* Smoothing constant, 0.0<=K<1.0 *)
   END_VAR
   IF RUN THEN XOUT := XOUT + K * (XIN - XOUT) ;
   ELSE XOUT := XIN ;
        K := TIME_TO_REAL(CYCLE) / TIME_TO_REAL(CYCLE + TAU) ;
   END_IF ;
 END_FUNCTION_BLOCK

