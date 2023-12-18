int16_t initI2C();
void closeI2C();
extern "C" int16_t getSwitchId();

// FUNCTION_BLOCK ROTARY_SWITCH
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(BOOL,READ)
  __DECLARE_VAR(BOOL,ERROR)
  __DECLARE_VAR(INT,OUT)

  // FB private variables - TEMP, private and located variables

} ROTARY_SWITCH;

static void ROTARY_SWITCH_init__(ROTARY_SWITCH *data__, BOOL retain);
// Code part
static void ROTARY_SWITCH_body__(ROTARY_SWITCH *data__);
static void ROTARY_SWITCH_init__(ROTARY_SWITCH *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->READ,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->ERROR,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUT,0,retain)
}

// Code part
static void ROTARY_SWITCH_body__(ROTARY_SWITCH *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  if ((__GET_VAR(data__->READ,) == __BOOL_LITERAL(TRUE))) {
    int16_t rotary_read = getSwitchId();
    if (rotary_read < 0)
    {
      __SET_VAR(data__->,OUT,,0);
      __SET_VAR(data__->,ERROR,,1);
    }
    else
    {
      __SET_VAR(data__->,OUT,,rotary_read);
      __SET_VAR(data__->,ERROR,,0);
    }
  }

  goto __end;

__end:
  return;
} // ROTARY_SWITCH_body__() 
