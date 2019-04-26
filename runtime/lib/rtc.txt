(****************************************************************

              RTC - Real-time clock

Q is a copy of IN.

When IN = FALSE, CDT is the current date and time as set by the
PLC driver.

When IN changes from FALSE to TRUE, PDT is stored. As long as IN is
TRUE, CDT is equal to PDT + the amount of time since PDT was loaded.

****************************************************************)

FUNCTION_BLOCK RTC
   VAR_INPUT
     IN : BOOL;                  (* 0 - current time, 1 - load time from 
PDT *)
     PDT : DT;                   (* Preset datetime *)
   END_VAR
   VAR_OUTPUT
     Q : BOOL := FALSE;          (* Copy of IN *)
     CDT : DT;                   (* Datetime, current or relative to PDT *)
   END_VAR

   VAR
     PREV_IN : BOOL := FALSE;
     OFFSET : TIME;
     CURRENT_TIME : DT;
   END_VAR

   {__SET_VAR(data__->,CURRENT_TIME,,__CURRENT_TIME)}

   IF IN
   THEN
     IF NOT PREV_IN
     THEN
         OFFSET := PDT - CURRENT_TIME;
     END_IF;

     (* PDT + time since PDT was loaded *)
     CDT := CURRENT_TIME + OFFSET;
   ELSE
     CDT := CURRENT_TIME;
   END_IF;

   Q := IN;
   PREV_IN := IN;

END_FUNCTION_BLOCK

