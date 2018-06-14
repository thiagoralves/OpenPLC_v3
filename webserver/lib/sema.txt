(*
 * (c) 2011 Edouard Tisserant
 *
 * Fake semaphore support, for non preemptive implementation.
 *
 *)


FUNCTION_BLOCK SEMA
  VAR_INPUT
    CLAIM : BOOL;
    RELEASE : BOOL;
  END_VAR
  VAR_OUTPUT
    BUSY : BOOL;
  END_VAR
  VAR
    Q_INTERNAL : BOOL;
  END_VAR

  Q_INTERNAL := CLAIM OR ( Q_INTERNAL AND (NOT RELEASE));
  BUSY := Q_INTERNAL;

END_FUNCTION_BLOCK

