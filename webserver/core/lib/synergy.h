/************************************************************************
 *                  DECLARATION OF Synergy LIB BLOCKS                   *
************************************************************************/
// SLM_INIT
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(BOOL,INIT)
  __DECLARE_VAR(BOOL,SUCCESS)

  // FB private variables - TEMP, private and located variables

} SLM_INIT;

// SLM_04THM_CONFIG
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(SINT,UNITS)
  __DECLARE_VAR(SINT,CH1_RANGE)
  __DECLARE_VAR(SINT,CH2_RANGE)
  __DECLARE_VAR(SINT,CH3_RANGE)
  __DECLARE_VAR(SINT,CH4_RANGE)
  __DECLARE_VAR(BOOL,SUCCESS)

  // FB private variables - TEMP, private and located variables

} SLM_04THM_CONFIG;

// SLM_DISCRETE_OUT_8
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(BOOL,O1)
  __DECLARE_VAR(BOOL,O2)
  __DECLARE_VAR(BOOL,O3)
  __DECLARE_VAR(BOOL,O4)
  __DECLARE_VAR(BOOL,O5)
  __DECLARE_VAR(BOOL,O6)
  __DECLARE_VAR(BOOL,O7)
  __DECLARE_VAR(BOOL,O8)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,DUMMY)

} SLM_DISCRETE_OUT_8;

// SLM_DISCRETE_OUT_15
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(BOOL,O1)
  __DECLARE_VAR(BOOL,O2)
  __DECLARE_VAR(BOOL,O3)
  __DECLARE_VAR(BOOL,O4)
  __DECLARE_VAR(BOOL,O5)
  __DECLARE_VAR(BOOL,O6)
  __DECLARE_VAR(BOOL,O7)
  __DECLARE_VAR(BOOL,O8)
  __DECLARE_VAR(BOOL,O9)
  __DECLARE_VAR(BOOL,O10)
  __DECLARE_VAR(BOOL,O11)
  __DECLARE_VAR(BOOL,O12)
  __DECLARE_VAR(BOOL,O13)
  __DECLARE_VAR(BOOL,O14)
  __DECLARE_VAR(BOOL,O15)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,DUMMY)

} SLM_DISCRETE_OUT_15;

// SLM_DISCRETE_OUT_16
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(BOOL,O1)
  __DECLARE_VAR(BOOL,O2)
  __DECLARE_VAR(BOOL,O3)
  __DECLARE_VAR(BOOL,O4)
  __DECLARE_VAR(BOOL,O5)
  __DECLARE_VAR(BOOL,O6)
  __DECLARE_VAR(BOOL,O7)
  __DECLARE_VAR(BOOL,O8)
  __DECLARE_VAR(BOOL,O9)
  __DECLARE_VAR(BOOL,O10)
  __DECLARE_VAR(BOOL,O11)
  __DECLARE_VAR(BOOL,O12)
  __DECLARE_VAR(BOOL,O13)
  __DECLARE_VAR(BOOL,O14)
  __DECLARE_VAR(BOOL,O15)
  __DECLARE_VAR(BOOL,O16)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,DUMMY)

} SLM_DISCRETE_OUT_16;

// SLM_DISCRETE_IN_8
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(BOOL,I1)
  __DECLARE_VAR(BOOL,I2)
  __DECLARE_VAR(BOOL,I3)
  __DECLARE_VAR(BOOL,I4)
  __DECLARE_VAR(BOOL,I5)
  __DECLARE_VAR(BOOL,I6)
  __DECLARE_VAR(BOOL,I7)
  __DECLARE_VAR(BOOL,I8)

  // FB private variables - TEMP, private and located variables

} SLM_DISCRETE_IN_8;

// SLM_DISCRETE_IN_16
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(BOOL,I1)
  __DECLARE_VAR(BOOL,I2)
  __DECLARE_VAR(BOOL,I3)
  __DECLARE_VAR(BOOL,I4)
  __DECLARE_VAR(BOOL,I5)
  __DECLARE_VAR(BOOL,I6)
  __DECLARE_VAR(BOOL,I7)
  __DECLARE_VAR(BOOL,I8)
  __DECLARE_VAR(BOOL,I9)
  __DECLARE_VAR(BOOL,I10)
  __DECLARE_VAR(BOOL,I11)
  __DECLARE_VAR(BOOL,I12)
  __DECLARE_VAR(BOOL,I13)
  __DECLARE_VAR(BOOL,I14)
  __DECLARE_VAR(BOOL,I15)
  __DECLARE_VAR(BOOL,I16)

  // FB private variables - TEMP, private and located variables

} SLM_DISCRETE_IN_16;

// SLM_ANALOG_IN_4
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(UINT,I1)
  __DECLARE_VAR(UINT,I2)
  __DECLARE_VAR(UINT,I3)
  __DECLARE_VAR(UINT,I4)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,DUMMY)

} SLM_ANALOG_IN_4;

// SLM_ANALOG_IN_8
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(UINT,I1)
  __DECLARE_VAR(UINT,I2)
  __DECLARE_VAR(UINT,I3)
  __DECLARE_VAR(UINT,I4)
  __DECLARE_VAR(UINT,I5)
  __DECLARE_VAR(UINT,I6)
  __DECLARE_VAR(UINT,I7)
  __DECLARE_VAR(UINT,I8)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,DUMMY)

} SLM_ANALOG_IN_8;

// SLM_ANALOG_OUT_4
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(UINT,O1)
  __DECLARE_VAR(UINT,O2)
  __DECLARE_VAR(UINT,O3)
  __DECLARE_VAR(UINT,O4)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,DUMMY)

} SLM_ANALOG_OUT_4;

// SLM_ANALOG_OUT_8
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(UINT,O1)
  __DECLARE_VAR(UINT,O2)
  __DECLARE_VAR(UINT,O3)
  __DECLARE_VAR(UINT,O4)
  __DECLARE_VAR(UINT,O5)
  __DECLARE_VAR(UINT,O6)
  __DECLARE_VAR(UINT,O7)
  __DECLARE_VAR(UINT,O8)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,DUMMY)

} SLM_ANALOG_OUT_8;

// SLM_TEMPERATURE_4
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,SLOT)
  __DECLARE_VAR(REAL,I1)
  __DECLARE_VAR(REAL,I2)
  __DECLARE_VAR(REAL,I3)
  __DECLARE_VAR(REAL,I4)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,DUMMY)

} SLM_TEMPERATURE_4;
/************************************************************************
 *                      END OF Synergy LIB BLOCKS                       *
************************************************************************/

/************************************************************************
 *                  DECLARATION LIBRARY FUNCTION CALLS                  *
************************************************************************/
uint8_t initHardware();
void writeDiscrete(uint32_t data,uint8_t slot, uint8_t channel = 0);
uint32_t readDiscrete(uint8_t slot, uint8_t channel = 0);
int readAnalog(uint8_t slot, uint8_t channel);
void writeAnalog(uint32_t data,uint8_t slot, uint8_t channel);
float readTemperature(uint8_t slot, uint8_t channel);
bool configureModule(char cfgData[], uint8_t slot);

/************************************************************************
 *                  DECLARATION OF Synergy LIB BLOCKS                   *
************************************************************************/
static void SLM_INIT_init__(SLM_INIT *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->INIT,0,retain)
  __INIT_VAR(data__->SUCCESS,0,retain)
}

// Code part
static void SLM_INIT_body__(SLM_INIT *data__) {
  static uint8_t init = 0;
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  if(init == 0) {
    initHardware();
    init = 1;
  }
  // __SET_VAR(data__->,SUCCESS,,initHardware());
  __SET_VAR(data__->,SUCCESS,,__BOOL_LITERAL(TRUE));

  goto __end;

__end:
  return;
} // SLM_INIT_body__()

static void SLM_04THM_CONFIG_init__(SLM_04THM_CONFIG *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->UNITS,0,retain)
  __INIT_VAR(data__->CH1_RANGE,0,retain)
  __INIT_VAR(data__->CH2_RANGE,0,retain)
  __INIT_VAR(data__->CH3_RANGE,0,retain)
  __INIT_VAR(data__->CH4_RANGE,0,retain)
  __INIT_VAR(data__->SUCCESS,0,retain)
}

// Code part
static void SLM_04THM_CONFIG_body__(SLM_04THM_CONFIG *data__) {
  static uint8_t init = 0;
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  if(init == 0) {
    char config_data[20] = {0x40, 0x03, 0x60, 0x05, 0x21, 0x00, 0x22, 0x00, 0x23, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    char units = __GET_VAR(data__->UNITS);
    if (units >= 1 && units <= 4) {
        units = 2 * units - 1;
    }
    config_data[3] = units;
    config_data[5] = __GET_VAR(data__->CH1_RANGE);
    config_data[7] = __GET_VAR(data__->CH2_RANGE);
    config_data[9] = __GET_VAR(data__->CH3_RANGE);
    config_data[11] = __GET_VAR(data__->CH4_RANGE);
    configureModule(config_data, __GET_VAR(data__->SLOT));
    __SET_VAR(data__->,SUCCESS,,__BOOL_LITERAL(TRUE));

    init = 1;
  }

  goto __end;

__end:
  return;
} // SLM_CONFIG_TEMP_body__()

/************************************************************************
 *                  DISCRETE LIB BLOCKS                                 *
************************************************************************/
static void SLM_DISCRETE_OUT_8_init__(SLM_DISCRETE_OUT_8 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->DUMMY,0,retain)
  __INIT_VAR(data__->O1,0,retain)
  __INIT_VAR(data__->O2,0,retain)
  __INIT_VAR(data__->O3,0,retain)
  __INIT_VAR(data__->O4,0,retain)
  __INIT_VAR(data__->O5,0,retain)
  __INIT_VAR(data__->O6,0,retain)
  __INIT_VAR(data__->O7,0,retain)
  __INIT_VAR(data__->O8,0,retain)
}

// Code part
static void SLM_DISCRETE_OUT_8_body__(SLM_DISCRETE_OUT_8 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  uint8_t output_byte = __GET_VAR(data__->O8) << 7 | 
                        __GET_VAR(data__->O7) << 6 | 
                        __GET_VAR(data__->O6) << 5 | 
                        __GET_VAR(data__->O5) << 4 | 
                        __GET_VAR(data__->O4) << 3 | 
                        __GET_VAR(data__->O3) << 2 | 
                        __GET_VAR(data__->O2) << 1 | 
                        __GET_VAR(data__->O1);
  writeDiscrete(output_byte, __GET_VAR(data__->SLOT), 0);

  goto __end;

__end:
  return;
} // SLM_DISCRETE_OUT_8_body__()

static void SLM_DISCRETE_OUT_15_init__(SLM_DISCRETE_OUT_15 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->DUMMY,0,retain)
  __INIT_VAR(data__->O1,0,retain)
  __INIT_VAR(data__->O2,0,retain)
  __INIT_VAR(data__->O3,0,retain)
  __INIT_VAR(data__->O4,0,retain)
  __INIT_VAR(data__->O5,0,retain)
  __INIT_VAR(data__->O6,0,retain)
  __INIT_VAR(data__->O7,0,retain)
  __INIT_VAR(data__->O8,0,retain)
  __INIT_VAR(data__->O9,0,retain)
  __INIT_VAR(data__->O10,0,retain)
  __INIT_VAR(data__->O11,0,retain)
  __INIT_VAR(data__->O12,0,retain)
  __INIT_VAR(data__->O13,0,retain)
  __INIT_VAR(data__->O14,0,retain)
  __INIT_VAR(data__->O15,0,retain)
}

// Code part
static void SLM_DISCRETE_OUT_15_body__(SLM_DISCRETE_OUT_15 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  uint16_t output_byte = __GET_VAR(data__->O15) << 14 | 
                         __GET_VAR(data__->O14) << 13 | 
                         __GET_VAR(data__->O13) << 12 | 
                         __GET_VAR(data__->O12) << 11 | 
                         __GET_VAR(data__->O11) << 10 | 
                         __GET_VAR(data__->O10) << 9 |
                         __GET_VAR(data__->O9) << 8 |
                         __GET_VAR(data__->O8) << 7 | 
                         __GET_VAR(data__->O7) << 6 | 
                         __GET_VAR(data__->O6) << 5 | 
                         __GET_VAR(data__->O5) << 4 | 
                         __GET_VAR(data__->O4) << 3 | 
                         __GET_VAR(data__->O3) << 2 | 
                         __GET_VAR(data__->O2) << 1 | 
                         __GET_VAR(data__->O1);
  writeDiscrete(output_byte, __GET_VAR(data__->SLOT), 0);

  goto __end;

__end:
  return;
} // SLM_DISCRETE_OUT_15_body__()

static void SLM_DISCRETE_OUT_16_init__(SLM_DISCRETE_OUT_16 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->DUMMY,0,retain)
  __INIT_VAR(data__->O1,0,retain)
  __INIT_VAR(data__->O2,0,retain)
  __INIT_VAR(data__->O3,0,retain)
  __INIT_VAR(data__->O4,0,retain)
  __INIT_VAR(data__->O5,0,retain)
  __INIT_VAR(data__->O6,0,retain)
  __INIT_VAR(data__->O7,0,retain)
  __INIT_VAR(data__->O8,0,retain)
  __INIT_VAR(data__->O9,0,retain)
  __INIT_VAR(data__->O10,0,retain)
  __INIT_VAR(data__->O11,0,retain)
  __INIT_VAR(data__->O12,0,retain)
  __INIT_VAR(data__->O13,0,retain)
  __INIT_VAR(data__->O14,0,retain)
  __INIT_VAR(data__->O15,0,retain)
  __INIT_VAR(data__->O16,0,retain)
}

// Code part
static void SLM_DISCRETE_OUT_16_body__(SLM_DISCRETE_OUT_16 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  uint16_t output_byte = __GET_VAR(data__->O16) << 15 | 
                         __GET_VAR(data__->O15) << 14 | 
                         __GET_VAR(data__->O14) << 13 | 
                         __GET_VAR(data__->O13) << 12 | 
                         __GET_VAR(data__->O12) << 11 | 
                         __GET_VAR(data__->O11) << 10 | 
                         __GET_VAR(data__->O10) << 9 |
                         __GET_VAR(data__->O9) << 8 |
                         __GET_VAR(data__->O8) << 7 | 
                         __GET_VAR(data__->O7) << 6 | 
                         __GET_VAR(data__->O6) << 5 | 
                         __GET_VAR(data__->O5) << 4 | 
                         __GET_VAR(data__->O4) << 3 | 
                         __GET_VAR(data__->O3) << 2 | 
                         __GET_VAR(data__->O2) << 1 | 
                         __GET_VAR(data__->O1);
  writeDiscrete(output_byte, __GET_VAR(data__->SLOT), 0);

  goto __end;

__end:
  return;
} // SLM_DISCRETE_OUT_16_body__()

static void SLM_DISCRETE_IN_8_init__(SLM_DISCRETE_IN_8 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->I1,0,retain)
  __INIT_VAR(data__->I2,0,retain)
  __INIT_VAR(data__->I3,0,retain)
  __INIT_VAR(data__->I4,0,retain)
  __INIT_VAR(data__->I5,0,retain)
  __INIT_VAR(data__->I6,0,retain)
  __INIT_VAR(data__->I7,0,retain)
  __INIT_VAR(data__->I8,0,retain)
}

// Code part
static void SLM_DISCRETE_IN_8_body__(SLM_DISCRETE_IN_8 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  #define bitRead(value, bit) (((value) >> (bit)) & 0x01)
  uint32_t input_byte = readDiscrete(__GET_VAR(data__->SLOT), 0);
  __SET_VAR(data__->,I1,,bitRead(input_byte, 0));
  __SET_VAR(data__->,I2,,bitRead(input_byte, 1));
  __SET_VAR(data__->,I3,,bitRead(input_byte, 2));
  __SET_VAR(data__->,I4,,bitRead(input_byte, 3));
  __SET_VAR(data__->,I5,,bitRead(input_byte, 4));
  __SET_VAR(data__->,I6,,bitRead(input_byte, 5));
  __SET_VAR(data__->,I7,,bitRead(input_byte, 6));
  __SET_VAR(data__->,I8,,bitRead(input_byte, 7));

  goto __end;

__end:
  return;
} // SLM_DISCRETE_IN_8_body__()

static void SLM_DISCRETE_IN_16_init__(SLM_DISCRETE_IN_16 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->I1,0,retain)
  __INIT_VAR(data__->I2,0,retain)
  __INIT_VAR(data__->I3,0,retain)
  __INIT_VAR(data__->I4,0,retain)
  __INIT_VAR(data__->I5,0,retain)
  __INIT_VAR(data__->I6,0,retain)
  __INIT_VAR(data__->I7,0,retain)
  __INIT_VAR(data__->I8,0,retain)
  __INIT_VAR(data__->I9,0,retain)
  __INIT_VAR(data__->I10,0,retain)
  __INIT_VAR(data__->I11,0,retain)
  __INIT_VAR(data__->I12,0,retain)
  __INIT_VAR(data__->I13,0,retain)
  __INIT_VAR(data__->I14,0,retain)
  __INIT_VAR(data__->I15,0,retain)
  __INIT_VAR(data__->I16,0,retain)
}

// Code part
static void SLM_DISCRETE_IN_16_body__(SLM_DISCRETE_IN_16 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  #define bitRead(value, bit) (((value) >> (bit)) & 0x01)
  uint32_t input_byte = readDiscrete(__GET_VAR(data__->SLOT), 0);
  __SET_VAR(data__->,I1,,bitRead(input_byte, 0));
  __SET_VAR(data__->,I2,,bitRead(input_byte, 1));
  __SET_VAR(data__->,I3,,bitRead(input_byte, 2));
  __SET_VAR(data__->,I4,,bitRead(input_byte, 3));
  __SET_VAR(data__->,I5,,bitRead(input_byte, 4));
  __SET_VAR(data__->,I6,,bitRead(input_byte, 5));
  __SET_VAR(data__->,I7,,bitRead(input_byte, 6));
  __SET_VAR(data__->,I8,,bitRead(input_byte, 7));
  __SET_VAR(data__->,I9,,bitRead(input_byte, 8));
  __SET_VAR(data__->,I10,,bitRead(input_byte, 9));
  __SET_VAR(data__->,I11,,bitRead(input_byte, 10));
  __SET_VAR(data__->,I12,,bitRead(input_byte, 11));
  __SET_VAR(data__->,I13,,bitRead(input_byte, 12));
  __SET_VAR(data__->,I14,,bitRead(input_byte, 13));
  __SET_VAR(data__->,I15,,bitRead(input_byte, 14));
  __SET_VAR(data__->,I16,,bitRead(input_byte, 15));

  goto __end;

__end:
  return;
} // SLM_DISCRETE_IN_16_body__()

/************************************************************************
 *                  ANALOG LIB BLOCKS                                   *
************************************************************************/

static void SLM_ANALOG_IN_4_init__(SLM_ANALOG_IN_4 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->DUMMY,0,retain)
  __INIT_VAR(data__->I1,0,retain)
  __INIT_VAR(data__->I2,0,retain)
  __INIT_VAR(data__->I3,0,retain)
  __INIT_VAR(data__->I4,0,retain)
}

// Code part
static void SLM_ANALOG_IN_4_body__(SLM_ANALOG_IN_4 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  char msg[100];
  uint16_t input_byte = readAnalog(__GET_VAR(data__->SLOT), 1);
  __SET_VAR(data__->,I1,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 2);
  __SET_VAR(data__->,I2,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 3);
  __SET_VAR(data__->,I3,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 4);
  __SET_VAR(data__->,I4,,input_byte);

  goto __end;

__end:
  return;
} // SLM_ANALOG_IN_4_body__()

static void SLM_ANALOG_IN_8_init__(SLM_ANALOG_IN_8 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->DUMMY,0,retain)
  __INIT_VAR(data__->I1,0,retain)
  __INIT_VAR(data__->I2,0,retain)
  __INIT_VAR(data__->I3,0,retain)
  __INIT_VAR(data__->I4,0,retain)
  __INIT_VAR(data__->I5,0,retain)
  __INIT_VAR(data__->I6,0,retain)
  __INIT_VAR(data__->I7,0,retain)
  __INIT_VAR(data__->I8,0,retain)
}

// Code part
static void SLM_ANALOG_IN_8_body__(SLM_ANALOG_IN_8 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  char msg[100];
  uint16_t input_byte = readAnalog(__GET_VAR(data__->SLOT), 1);
  __SET_VAR(data__->,I1,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 2);
  __SET_VAR(data__->,I2,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 3);
  __SET_VAR(data__->,I3,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 4);
  __SET_VAR(data__->,I4,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 5);
  __SET_VAR(data__->,I5,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 6);
  __SET_VAR(data__->,I6,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 7);
  __SET_VAR(data__->,I7,,input_byte);
  input_byte = readAnalog(__GET_VAR(data__->SLOT), 8);
  __SET_VAR(data__->,I8,,input_byte);

  goto __end;

__end:
  return;
} // SLM_ANALOG_IN_8_body__()

static void SLM_ANALOG_OUT_4_init__(SLM_ANALOG_OUT_4 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->DUMMY,0,retain)
  __INIT_VAR(data__->O1,0,retain)
  __INIT_VAR(data__->O2,0,retain)
  __INIT_VAR(data__->O3,0,retain)
  __INIT_VAR(data__->O4,0,retain)
}

// Code part
static void SLM_ANALOG_OUT_4_body__(SLM_ANALOG_OUT_4 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  char msg[100];
  uint32_t data = __GET_VAR(data__->O1);
  writeAnalog(data, __GET_VAR(data__->SLOT), 1);
  data = __GET_VAR(data__->O2);
  writeAnalog(data, __GET_VAR(data__->SLOT), 2);
  data = __GET_VAR(data__->O3);
  writeAnalog(data, __GET_VAR(data__->SLOT), 3);
  data = __GET_VAR(data__->O4);
  writeAnalog(data, __GET_VAR(data__->SLOT), 4);

  goto __end;

__end:
  return;
} // SLM_ANALOG_OUT_4_body__()

static void SLM_ANALOG_OUT_8_init__(SLM_ANALOG_OUT_8 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->DUMMY,0,retain)
  __INIT_VAR(data__->O1,0,retain)
  __INIT_VAR(data__->O2,0,retain)
  __INIT_VAR(data__->O3,0,retain)
  __INIT_VAR(data__->O4,0,retain)
  __INIT_VAR(data__->O5,0,retain)
  __INIT_VAR(data__->O6,0,retain)
  __INIT_VAR(data__->O7,0,retain)
  __INIT_VAR(data__->O8,0,retain)
}

// Code part
static void SLM_ANALOG_OUT_8_body__(SLM_ANALOG_OUT_8 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  char msg[100];
  uint32_t data = __GET_VAR(data__->O1);
  writeAnalog(data, __GET_VAR(data__->SLOT), 1);
  data = __GET_VAR(data__->O2);
  writeAnalog(data, __GET_VAR(data__->SLOT), 2);
  data = __GET_VAR(data__->O3);
  writeAnalog(data, __GET_VAR(data__->SLOT), 3);
  data = __GET_VAR(data__->O4);
  writeAnalog(data, __GET_VAR(data__->SLOT), 4);
  data = __GET_VAR(data__->O5);
  writeAnalog(data, __GET_VAR(data__->SLOT), 5);
  data = __GET_VAR(data__->O6);
  writeAnalog(data, __GET_VAR(data__->SLOT), 6);
  data = __GET_VAR(data__->O7);
  writeAnalog(data, __GET_VAR(data__->SLOT), 7);
  data = __GET_VAR(data__->O8);
  writeAnalog(data, __GET_VAR(data__->SLOT), 8);

  goto __end;

__end:
  return;
} // SLM_ANALOG_OUT_8_body__()

static void SLM_TEMPERATURE_4_init__(SLM_TEMPERATURE_4 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SLOT,0,retain)
  __INIT_VAR(data__->DUMMY,0,retain)
  __INIT_VAR(data__->I1,0,retain)
  __INIT_VAR(data__->I2,0,retain)
  __INIT_VAR(data__->I3,0,retain)
  __INIT_VAR(data__->I4,0,retain)
}

// Code part
static void SLM_TEMPERATURE_4_body__(SLM_TEMPERATURE_4 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  char msg[100];
  float input_byte = readTemperature(__GET_VAR(data__->SLOT), 1);
  __SET_VAR(data__->,I1,,input_byte);
  input_byte = readTemperature(__GET_VAR(data__->SLOT), 2);
  __SET_VAR(data__->,I2,,input_byte);
  input_byte = readTemperature(__GET_VAR(data__->SLOT), 3);
  __SET_VAR(data__->,I3,,input_byte);
  input_byte = readTemperature(__GET_VAR(data__->SLOT), 4);
  __SET_VAR(data__->,I4,,input_byte);

  goto __end;

__end:
  return;
} // SLM_TEMPERATURE_4_body__()
/************************************************************************
 *                      END OF Synergy LIB BLOCKS                       *
************************************************************************/
