
(* Following taken directly from the IEC 61131.3 draft standard *)

(*
 * An IEC 61131-3 IL and ST compiler.
 *
 * Based on the
 * FINAL DRAFT - IEC 61131-3, 2nd Ed. (2001-12-10)
 *
 *)


 FUNCTION_BLOCK DERIVATIVE
   VAR_INPUT
     RUN : BOOL ;          (* 0 = reset                  *)
     XIN : REAL ;          (* Input to be differentiated *)
     CYCLE : TIME ;        (* Sampling period            *)
   END_VAR
   VAR_OUTPUT
     XOUT : REAL ;         (* Differentiated output      *)
   END_VAR
   VAR X1, X2, X3 : REAL ; END_VAR
   IF RUN THEN
      XOUT := (3.0 * (XIN - X3) + X1 - X2)
              / (10.0 * TIME_TO_REAL(CYCLE)) ;
      X3 := X2 ; X2 := X1 ; X1 := XIN ;
   ELSE XOUT := 0.0; X1 := XIN ; X2 := XIN ; X3 := XIN ;
   END_IF ;
 END_FUNCTION_BLOCK

