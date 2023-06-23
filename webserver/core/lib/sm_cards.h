


/************************************************************************
 *                  DECLARATION OF SM_CARDS LIB BLOCKS                  *
************************************************************************/

// SM_8RELAY
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
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

} SM_8RELAY;

// SM_16RELAY ARDUINO
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
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

} SM_16RELAY;

// SM_8DIN
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
  __DECLARE_VAR(BOOL,I1)
  __DECLARE_VAR(BOOL,I2)
  __DECLARE_VAR(BOOL,I3)
  __DECLARE_VAR(BOOL,I4)
  __DECLARE_VAR(BOOL,I5)
  __DECLARE_VAR(BOOL,I6)
  __DECLARE_VAR(BOOL,I7)
  __DECLARE_VAR(BOOL,I8)

  // FB private variables - TEMP, private and located variables

} SM_8DIN;


// SM_16DIN
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
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

} SM_16DIN;

// SM_4REL_4IN
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
  __DECLARE_VAR(BOOL,RELAY1)
  __DECLARE_VAR(BOOL,RELAY2)
  __DECLARE_VAR(BOOL,RELAY3)
  __DECLARE_VAR(BOOL,RELAY4)
  __DECLARE_VAR(BOOL,OPTO1)
  __DECLARE_VAR(BOOL,OPTO2)
  __DECLARE_VAR(BOOL,OPTO3)
  __DECLARE_VAR(BOOL,OPTO4)
  __DECLARE_VAR(BOOL,AC_OPTO1)
  __DECLARE_VAR(BOOL,AC_OPTO2)
  __DECLARE_VAR(BOOL,AC_OPTO3)
  __DECLARE_VAR(BOOL,AC_OPTO4)
  __DECLARE_VAR(REAL,PWM1)
  __DECLARE_VAR(REAL,PWM2)
  __DECLARE_VAR(REAL,PWM3)
  __DECLARE_VAR(REAL,PWM4)
  __DECLARE_VAR(UINT,FREQ1)
  __DECLARE_VAR(UINT,FREQ2)
  __DECLARE_VAR(UINT,FREQ3)
  __DECLARE_VAR(UINT,FREQ4)
  __DECLARE_VAR(BOOL,BUTTON)
  // FB private variables - TEMP, private and located variables

} SM_4REL4IN;


// SM_RTD
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
  __DECLARE_VAR(REAL,TEMP1)
  __DECLARE_VAR(REAL,TEMP2)
  __DECLARE_VAR(REAL,TEMP3)
  __DECLARE_VAR(REAL,TEMP4)
  __DECLARE_VAR(REAL,TEMP5)
  __DECLARE_VAR(REAL,TEMP6)
  __DECLARE_VAR(REAL,TEMP7)
  __DECLARE_VAR(REAL,TEMP8)

} SM_RTD;

// SM_INDUSTRIAL
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
  __DECLARE_VAR(BOOL,LED1)
  __DECLARE_VAR(BOOL,LED2)
  __DECLARE_VAR(BOOL,LED3)
  __DECLARE_VAR(BOOL,LED4)
  __DECLARE_VAR(REAL,Q0_10V1)
  __DECLARE_VAR(REAL,Q0_10V2)
  __DECLARE_VAR(REAL,Q0_10V3)
  __DECLARE_VAR(REAL,Q0_10V4)
  __DECLARE_VAR(REAL,Q4_20MA1)
  __DECLARE_VAR(REAL,Q4_20MA2)
  __DECLARE_VAR(REAL,Q4_20MA3)
  __DECLARE_VAR(REAL,Q4_20MA4)
  __DECLARE_VAR(REAL,QOD1)
  __DECLARE_VAR(REAL,QOD2)
  __DECLARE_VAR(REAL,QOD3)
  __DECLARE_VAR(REAL,QOD4)
  __DECLARE_VAR(BOOL,OPTO1)
  __DECLARE_VAR(BOOL,OPTO2)
  __DECLARE_VAR(BOOL,OPTO3)
  __DECLARE_VAR(BOOL,OPTO4)
  __DECLARE_VAR(REAL,I0_10V1)
  __DECLARE_VAR(REAL,I0_10V2)
  __DECLARE_VAR(REAL,I0_10V3)
  __DECLARE_VAR(REAL,I0_10V4)
  __DECLARE_VAR(REAL,I4_20MA1)
  __DECLARE_VAR(REAL,I4_20MA2)
  __DECLARE_VAR(REAL,I4_20MA3)
  __DECLARE_VAR(REAL,I4_20MA4)
  __DECLARE_VAR(REAL,OWB_T1)
  __DECLARE_VAR(REAL,OWB_T2)
  __DECLARE_VAR(REAL,OWB_T3)
  __DECLARE_VAR(REAL,OWB_T4)
} SM_INDUSTRIAL;


// SM_BUILDING
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
  __DECLARE_VAR(BOOL,TRIAC1)
  __DECLARE_VAR(BOOL,TRIAC2)
  __DECLARE_VAR(BOOL,TRIAC3)
  __DECLARE_VAR(BOOL,TRIAC4)
  __DECLARE_VAR(BOOL,LED1)
  __DECLARE_VAR(BOOL,LED2)
  __DECLARE_VAR(BOOL,LED3)
  __DECLARE_VAR(BOOL,LED4)
  __DECLARE_VAR(UINT,IN1_T)
  __DECLARE_VAR(UINT,IN2_T)
  __DECLARE_VAR(UINT,IN3_T)
  __DECLARE_VAR(UINT,IN4_T)
  __DECLARE_VAR(UINT,IN5_T)
  __DECLARE_VAR(UINT,IN6_T)
  __DECLARE_VAR(UINT,IN7_T)
  __DECLARE_VAR(UINT,IN8_T)
  __DECLARE_VAR(REAL,Q0_10V1)
  __DECLARE_VAR(REAL,Q0_10V2)
  __DECLARE_VAR(REAL,Q0_10V3)
  __DECLARE_VAR(REAL,Q0_10V4)
  __DECLARE_VAR(REAL,UNIV1)
  __DECLARE_VAR(REAL,UNIV2)
  __DECLARE_VAR(REAL,UNIV3)
  __DECLARE_VAR(REAL,UNIV4)
  __DECLARE_VAR(REAL,UNIV5)
  __DECLARE_VAR(REAL,UNIV6)
  __DECLARE_VAR(REAL,UNIV7)
  __DECLARE_VAR(REAL,UNIV8)  
  __DECLARE_VAR(BOOL,DRY_C1)
  __DECLARE_VAR(BOOL,DRY_C2)
  __DECLARE_VAR(BOOL,DRY_C3)
  __DECLARE_VAR(BOOL,DRY_C4)
  __DECLARE_VAR(BOOL,DRY_C5)
  __DECLARE_VAR(BOOL,DRY_C6)
  __DECLARE_VAR(BOOL,DRY_C7)
  __DECLARE_VAR(BOOL,DRY_C8) 
  __DECLARE_VAR(REAL,OWB_T1)
  __DECLARE_VAR(REAL,OWB_T2)
  __DECLARE_VAR(REAL,OWB_T3)
  __DECLARE_VAR(REAL,OWB_T4)
} SM_BAS;


// SM_HOME 
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
  __DECLARE_VAR(BOOL,RELAY1)
  __DECLARE_VAR(BOOL,RELAY2)
  __DECLARE_VAR(BOOL,RELAY3)
  __DECLARE_VAR(BOOL,RELAY4)
  __DECLARE_VAR(BOOL,RELAY5)
  __DECLARE_VAR(BOOL,RELAY6)
  __DECLARE_VAR(BOOL,RELAY7)
  __DECLARE_VAR(BOOL,RELAY8)
  __DECLARE_VAR(REAL,Q0_10V1)
  __DECLARE_VAR(REAL,Q0_10V2)
  __DECLARE_VAR(REAL,Q0_10V3)
  __DECLARE_VAR(REAL,Q0_10V4)
  __DECLARE_VAR(REAL,QOD1)
  __DECLARE_VAR(REAL,QOD2)
  __DECLARE_VAR(REAL,QOD3)
  __DECLARE_VAR(REAL,QOD4)
  __DECLARE_VAR(BOOL,OPTO1)
  __DECLARE_VAR(BOOL,OPTO2)
  __DECLARE_VAR(BOOL,OPTO3)
  __DECLARE_VAR(BOOL,OPTO4)
  __DECLARE_VAR(BOOL,OPTO5)
  __DECLARE_VAR(BOOL,OPTO6)
  __DECLARE_VAR(BOOL,OPTO7)
  __DECLARE_VAR(BOOL,OPTO8)
  __DECLARE_VAR(REAL,ADC1)
  __DECLARE_VAR(REAL,ADC2)
  __DECLARE_VAR(REAL,ADC3)
  __DECLARE_VAR(REAL,ADC4)
  __DECLARE_VAR(REAL,ADC5)
  __DECLARE_VAR(REAL,ADC6)
  __DECLARE_VAR(REAL,ADC7)
  __DECLARE_VAR(REAL,ADC8)
  __DECLARE_VAR(REAL,OWB_T1)
  __DECLARE_VAR(REAL,OWB_T2)
  __DECLARE_VAR(REAL,OWB_T3)
  __DECLARE_VAR(REAL,OWB_T4)
} SM_HOME;



// SM_8MOSFET 
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(SINT,STACK)
  __DECLARE_VAR(BOOL,MOS1)
  __DECLARE_VAR(BOOL,MOS2)
  __DECLARE_VAR(BOOL,MOS3)
  __DECLARE_VAR(BOOL,MOS4)
  __DECLARE_VAR(BOOL,MOS5)
  __DECLARE_VAR(BOOL,MOS6)
  __DECLARE_VAR(BOOL,MOS7)
  __DECLARE_VAR(BOOL,MOS8)
  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,DUMMY)
} SM_8MOSFET;


/************************************************************************
 *                      END OF SM_CARDS LIB BLOCKS                      *
************************************************************************/

/************************************************************************
 *                  DECLARATION OF SM_CARDS LIB BLOCKS                  *
************************************************************************/
int relay8Init(int);
int relays8Set(uint8_t, uint8_t);


static void SM_8RELAY_init__(SM_8RELAY *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
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
static void SM_8RELAY_body__(SM_8RELAY *data__) {
	static uint8_t init = 0;
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  __SET_VAR(data__->,DUMMY,,0);
  if(init == 0)
  {
	  if(0 == relay8Init(__GET_VAR(data__->STACK)))
	  {
		  init = 1;
	  }
  }
  else // already init
  {
	uint8_t output_byte = __GET_VAR(data__->O8) << 7 | 
                        __GET_VAR(data__->O7) << 6 | 
                        __GET_VAR(data__->O6) << 5 | 
                        __GET_VAR(data__->O5) << 4 | 
                        __GET_VAR(data__->O4) << 3 | 
                        __GET_VAR(data__->O3) << 2 | 
                        __GET_VAR(data__->O2) << 1 | 
                        __GET_VAR(data__->O1);
	relays8Set(__GET_VAR(data__->STACK), output_byte);
  }
  goto __end;

__end:
  return;
} // SM_8RELAY_body__()

int relay16Init(int);
int relay16Set(uint8_t, uint16_t);

static void SM_16RELAY_init__(SM_16RELAY *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
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
static void SM_16RELAY_body__(SM_16RELAY *data__) {
	static uint8_t init = 0;
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Dummy code - just for editor simulation. Real code is inside iec_std_FB.h file on arduino folder
  __SET_VAR(data__->,DUMMY,,0);
  if(init == 0)
  {
	  if(0 == relay16Init(__GET_VAR(data__->STACK)))
	  {
		  init = 1;
	  }
  }
  else // already init
  {
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
	relay16Set(__GET_VAR(data__->STACK), output_byte);
  }
  goto __end;

__end:
  return;
} // SM_16RELAY_body__()

int digIn8Get(uint8_t, uint8_t*);
int digIn8Init(int );

static void SM_8DIN_init__(SM_8DIN *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
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
static void SM_8DIN_body__(SM_8DIN *data__) {
	static uint8_t init = 0;
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  #define bitRead(value, bit) (((value) >> (bit)) & 0x01)
  
  if(init == 0)
  {
  	  if(0 == digIn8Init(__GET_VAR(data__->STACK)))
  	  {
  		  init = 1;
  	  }
  }
  else // already init
  {
	  uint8_t input_byte = 0;
	  if(0 == digIn8Get(__GET_VAR(data__->STACK), &input_byte)){
		  __SET_VAR(data__->,I1,,bitRead(input_byte, 0));
		  __SET_VAR(data__->,I2,,bitRead(input_byte, 1));
		  __SET_VAR(data__->,I3,,bitRead(input_byte, 2));
		  __SET_VAR(data__->,I4,,bitRead(input_byte, 3));
		  __SET_VAR(data__->,I5,,bitRead(input_byte, 4));
		  __SET_VAR(data__->,I6,,bitRead(input_byte, 5));
		  __SET_VAR(data__->,I7,,bitRead(input_byte, 6));
		  __SET_VAR(data__->,I8,,bitRead(input_byte, 7));
	  }
  }

   goto __end;

__end:
  return;
} // SM_8DIN_body__()



int digIn16Get(uint8_t, uint16_t*);
int digIn16Init(int );

static void SM_16DIN_init__(SM_16DIN *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
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
static void SM_16DIN_body__(SM_16DIN *data__) {
	static uint8_t init = 0;
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  #define bitRead(value, bit) (((value) >> (bit)) & 0x01)
  
  if(init == 0)
  {
  	  if(0 == digIn16Init(__GET_VAR(data__->STACK)))
  	  {
  		  init = 1;
  	  }
  }
  else // already init
  {
	  uint16_t input_byte = 0;
	  if(0 == digIn16Get(__GET_VAR(data__->STACK), &input_byte)){
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
	  }
  }
  goto __end;

__end:
  return;
} // SM_16DIN_body__()

int r4i4SetRelays(uint8_t, uint8_t);
int r4i4GetOptoInputs(uint8_t, uint8_t*);
int r4i4GetACInputs(uint8_t, uint8_t*);
int r4i4GetButton(uint8_t, uint8_t*);
int r4i4GetPWMInFill(uint8_t, uint8_t, uint16_t*);
int r4i4GetPWMInFreq(uint8_t, uint8_t, uint16_t*);

static void SM_4REL4IN_init__(SM_4REL4IN *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
  __INIT_VAR(data__->RELAY1,0,retain)
  __INIT_VAR(data__->RELAY2,0,retain)
  __INIT_VAR(data__->RELAY3,0,retain)
  __INIT_VAR(data__->RELAY4,0,retain)
  __INIT_VAR(data__->OPTO1,0,retain)
  __INIT_VAR(data__->OPTO2,0,retain)
  __INIT_VAR(data__->OPTO3,0,retain)
  __INIT_VAR(data__->OPTO4,0,retain)
  __INIT_VAR(data__->AC_OPTO1,0,retain)
  __INIT_VAR(data__->AC_OPTO2,0,retain)
  __INIT_VAR(data__->AC_OPTO3,0,retain)
  __INIT_VAR(data__->AC_OPTO4,0,retain)
  __INIT_VAR(data__->PWM1,0,retain)
  __INIT_VAR(data__->PWM2,0,retain)
  __INIT_VAR(data__->PWM3,0,retain)
  __INIT_VAR(data__->PWM4,0,retain)
  __INIT_VAR(data__->FREQ1,0,retain)
  __INIT_VAR(data__->FREQ2,0,retain)
  __INIT_VAR(data__->FREQ3,0,retain)
  __INIT_VAR(data__->FREQ4,0,retain)
  __INIT_VAR(data__->BUTTON,0,retain)
}

// Code part
static void SM_4REL4IN_body__(SM_4REL4IN *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  uint8_t output_byte = __GET_VAR(data__->RELAY4) << 3 | 
                        __GET_VAR(data__->RELAY3) << 2 | 
                        __GET_VAR(data__->RELAY2) << 1 | 
                        __GET_VAR(data__->RELAY1);
  r4i4SetRelays(__GET_VAR(data__->STACK), output_byte);
  #define bitRead(value, bit) (((value) >> (bit)) & 0x01)
  uint8_t input_byte = 0;
	if(0 == r4i4GetOptoInputs(__GET_VAR(data__->STACK), &input_byte)){
		__SET_VAR(data__->,OPTO1,,bitRead(input_byte, 0));
		__SET_VAR(data__->,OPTO2,,bitRead(input_byte, 1));
		__SET_VAR(data__->,OPTO3,,bitRead(input_byte, 2));
		__SET_VAR(data__->,OPTO4,,bitRead(input_byte, 3));
	}
	if(0 == r4i4GetACInputs(__GET_VAR(data__->STACK), &input_byte)){
		__SET_VAR(data__->,AC_OPTO1,,bitRead(input_byte, 0));
		__SET_VAR(data__->,AC_OPTO2,,bitRead(input_byte, 1));
		__SET_VAR(data__->,AC_OPTO3,,bitRead(input_byte, 2));
		__SET_VAR(data__->,AC_OPTO4,,bitRead(input_byte, 3));
	}
	if(0 == r4i4GetButton(__GET_VAR(data__->STACK), &input_byte)){
		__SET_VAR(data__->,BUTTON,,bitRead(input_byte, 0));
	}
	uint16_t sint_val = 0;
	
    if( 0 == r4i4GetPWMInFreq(__GET_VAR(data__->STACK), 0, &sint_val) )
    {
		__SET_VAR(data__->,FREQ1,,sint_val);
    }
	if( 0 == r4i4GetPWMInFreq(__GET_VAR(data__->STACK), 1, &sint_val) )
    {
		__SET_VAR(data__->,FREQ2,,sint_val);
    }
	if( 0 == r4i4GetPWMInFreq(__GET_VAR(data__->STACK), 2, &sint_val) )
    {
		__SET_VAR(data__->,FREQ3,,sint_val);
    }
	if( 0 == r4i4GetPWMInFreq(__GET_VAR(data__->STACK), 3, &sint_val) )
    {
		__SET_VAR(data__->,FREQ4,,sint_val);
    }

	 if( 0 == r4i4GetPWMInFill(__GET_VAR(data__->STACK), 0, &sint_val) )
    {
		__SET_VAR(data__->,PWM1,,(float)sint_val/100);
    }
	if( 0 == r4i4GetPWMInFill(__GET_VAR(data__->STACK), 1, &sint_val) )
    {
		__SET_VAR(data__->,PWM2,,(float)sint_val/100);
    }
	if( 0 == r4i4GetPWMInFill(__GET_VAR(data__->STACK), 2, &sint_val) )
    {
		__SET_VAR(data__->,PWM3,,(float)sint_val/100);
    }
	if( 0 == r4i4GetPWMInFill(__GET_VAR(data__->STACK), 3, &sint_val) )
    {
		__SET_VAR(data__->,PWM4,,(float)sint_val/100);
    }

   goto __end;

__end:
  return;
} // SM_4REL4IN_body__()



int rtdGetTemp(uint8_t, uint8_t, float*);

static void SM_RTD_init__(SM_RTD *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
  __INIT_VAR(data__->TEMP1,0,retain)
  __INIT_VAR(data__->TEMP2,0,retain)
  __INIT_VAR(data__->TEMP3,0,retain)
  __INIT_VAR(data__->TEMP4,0,retain)
  __INIT_VAR(data__->TEMP5,0,retain)
  __INIT_VAR(data__->TEMP6,0,retain)
  __INIT_VAR(data__->TEMP7,0,retain)
  __INIT_VAR(data__->TEMP8,0,retain)
}

// Code part
static void SM_RTD_body__(SM_RTD *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  float val = 0;
	
  if( 0 == rtdGetTemp(__GET_VAR(data__->STACK), 0, &val))
  {
	__SET_VAR(data__->,TEMP1,,val);
  }
  if( 0 == rtdGetTemp(__GET_VAR(data__->STACK), 1, &val))
  {
	__SET_VAR(data__->,TEMP2,,val);
  }
  if( 0 == rtdGetTemp(__GET_VAR(data__->STACK), 2, &val))
  {
	__SET_VAR(data__->,TEMP3,,val);
  }
  if( 0 == rtdGetTemp(__GET_VAR(data__->STACK), 3, &val))
  {
	__SET_VAR(data__->,TEMP4,,val);
  }
  if( 0 == rtdGetTemp(__GET_VAR(data__->STACK), 4, &val))
  {
	__SET_VAR(data__->,TEMP5,,val);
  }
  if( 0 == rtdGetTemp(__GET_VAR(data__->STACK), 5, &val))
  {
	__SET_VAR(data__->,TEMP6,,val);
  }
  if( 0 == rtdGetTemp(__GET_VAR(data__->STACK), 6, &val))
  {
	__SET_VAR(data__->,TEMP7,,val);
  }
  if( 0 == rtdGetTemp(__GET_VAR(data__->STACK), 7, &val))
  {
	__SET_VAR(data__->,TEMP8,,val);
  }
	
   goto __end;

__end:
  return;
} // SM_RTD_body__()

int indSetLeds(uint8_t, uint8_t);
int indGetOptoInputs(uint8_t, uint8_t*);
int indGet0_10Vin(uint8_t, uint8_t, float*);
int indGet4_20mAin(uint8_t, uint8_t, float*);
int indGet1WbTemp(uint8_t, uint8_t, float*);
int indSet0_10Vout(uint8_t, uint8_t, float);
int indSet4_20mAout(uint8_t, uint8_t, float);
int indSetPWMout(uint8_t, uint8_t, float);

static void SM_INDUSTRIAL_init__(SM_INDUSTRIAL *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
  __INIT_VAR(data__->LED1,0,retain)
  __INIT_VAR(data__->LED2,0,retain)
  __INIT_VAR(data__->LED3,0,retain)
  __INIT_VAR(data__->LED4,0,retain)
  __INIT_VAR(data__->Q0_10V1,0,retain)
  __INIT_VAR(data__->Q0_10V2,0,retain)
  __INIT_VAR(data__->Q0_10V3,0,retain)
  __INIT_VAR(data__->Q0_10V4,0,retain)
  __INIT_VAR(data__->Q4_20MA1,0,retain)
  __INIT_VAR(data__->Q4_20MA2,0,retain)
  __INIT_VAR(data__->Q4_20MA3,0,retain)
  __INIT_VAR(data__->Q4_20MA4,0,retain)
  __INIT_VAR(data__->QOD1,0,retain)
  __INIT_VAR(data__->QOD2,0,retain)
  __INIT_VAR(data__->QOD3,0,retain)
  __INIT_VAR(data__->QOD4,0,retain)
  __INIT_VAR(data__->OPTO1,0,retain)
  __INIT_VAR(data__->OPTO2,0,retain)
  __INIT_VAR(data__->OPTO3,0,retain)
  __INIT_VAR(data__->OPTO4,0,retain)
  __INIT_VAR(data__->I0_10V1,0,retain)
  __INIT_VAR(data__->I0_10V2,0,retain)
  __INIT_VAR(data__->I0_10V3,0,retain)
  __INIT_VAR(data__->I0_10V4,0,retain)
  __INIT_VAR(data__->I4_20MA1,0,retain)
  __INIT_VAR(data__->I4_20MA2,0,retain)
  __INIT_VAR(data__->I4_20MA3,0,retain)
  __INIT_VAR(data__->I4_20MA4,0,retain)
  __INIT_VAR(data__->OWB_T1,0,retain)
  __INIT_VAR(data__->OWB_T2,0,retain)
  __INIT_VAR(data__->OWB_T3,0,retain)
  __INIT_VAR(data__->OWB_T4,0,retain)
}

// Code part
static void SM_INDUSTRIAL_body__(SM_INDUSTRIAL *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  uint8_t output_byte = __GET_VAR(data__->LED4) << 3 |
                        __GET_VAR(data__->LED3) << 2 |
                        __GET_VAR(data__->LED2) << 1 |
                        __GET_VAR(data__->LED1);
  indSetLeds(__GET_VAR(data__->STACK), output_byte);
  #define bitRead(value, bit) (((value) >> (bit)) & 0x01)
  uint8_t input_byte = 0;
  if(0 == indGetOptoInputs(__GET_VAR(data__->STACK), &input_byte)){
	__SET_VAR(data__->,OPTO1,,bitRead(input_byte, 0));
	__SET_VAR(data__->,OPTO2,,bitRead(input_byte, 1));
	__SET_VAR(data__->,OPTO3,,bitRead(input_byte, 2));
	__SET_VAR(data__->,OPTO4,,bitRead(input_byte, 3));
  }
  float val = 0;

// ---------  0-10V inputs	------------------
  if( 0 == indGet0_10Vin(__GET_VAR(data__->STACK), 0, &val))
  {
	__SET_VAR(data__->,I0_10V1,,val);
  }
  if( 0 == indGet0_10Vin(__GET_VAR(data__->STACK), 1, &val))
  {
	__SET_VAR(data__->,I0_10V2,,val);
  }
  if( 0 == indGet0_10Vin(__GET_VAR(data__->STACK), 2, &val))
  {
	__SET_VAR(data__->,I0_10V3,,val);
  }
  if( 0 == indGet0_10Vin(__GET_VAR(data__->STACK), 3, &val))
  {
	__SET_VAR(data__->,I0_10V4,,val);
  }

// -------------- 4-20mA inputs -----------------------
  if( 0 == indGet4_20mAin(__GET_VAR(data__->STACK), 0, &val))
  {
	__SET_VAR(data__->,I4_20MA1,,val);
  }
  if( 0 == indGet4_20mAin(__GET_VAR(data__->STACK), 1, &val))
  {
	__SET_VAR(data__->,I4_20MA2,,val);
  }
  if( 0 == indGet4_20mAin(__GET_VAR(data__->STACK), 2, &val))
  {
	__SET_VAR(data__->,I4_20MA3,,val);
  }
  if( 0 == indGet4_20mAin(__GET_VAR(data__->STACK), 3, &val))
  {
	__SET_VAR(data__->,I4_20MA4,,val);
  }

// --------------- One wire bus temperature inputs (dsb20 sensors)
  if( 0 == indGet1WbTemp(__GET_VAR(data__->STACK), 0, &val))
  {
	__SET_VAR(data__->,OWB_T1,,val);
  }
  if( 0 == indGet1WbTemp(__GET_VAR(data__->STACK), 1, &val))
  {
	__SET_VAR(data__->,OWB_T2,,val);
  }
  if( 0 == indGet1WbTemp(__GET_VAR(data__->STACK), 2, &val))
  {
	__SET_VAR(data__->,OWB_T3,,val);
  }
  if( 0 == indGet1WbTemp(__GET_VAR(data__->STACK), 3, &val))
  {
	__SET_VAR(data__->,OWB_T4,,val);
  }

// ----------------- 0-10V outputs ---------------------------------------
  indSet0_10Vout(__GET_VAR(data__->STACK), 0, __GET_VAR(data__->Q0_10V1));
  indSet0_10Vout(__GET_VAR(data__->STACK), 1, __GET_VAR(data__->Q0_10V2));
  indSet0_10Vout(__GET_VAR(data__->STACK), 2, __GET_VAR(data__->Q0_10V3));
  indSet0_10Vout(__GET_VAR(data__->STACK), 3, __GET_VAR(data__->Q0_10V4));

// ----------------- 4-20 mA outputs --------------------------------------
  indSet4_20mAout(__GET_VAR(data__->STACK), 0, __GET_VAR(data__->Q4_20MA1));
  indSet4_20mAout(__GET_VAR(data__->STACK), 1, __GET_VAR(data__->Q4_20MA2));
  indSet4_20mAout(__GET_VAR(data__->STACK), 2, __GET_VAR(data__->Q4_20MA3));
  indSet4_20mAout(__GET_VAR(data__->STACK), 3, __GET_VAR(data__->Q4_20MA4));

// ---------------- open dranin with PWM output fill factor 0-100% ---------
  indSetPWMout(__GET_VAR(data__->STACK), 0, __GET_VAR(data__->QOD1));
  indSetPWMout(__GET_VAR(data__->STACK), 1, __GET_VAR(data__->QOD2));
  indSetPWMout(__GET_VAR(data__->STACK), 2, __GET_VAR(data__->QOD3));
  indSetPWMout(__GET_VAR(data__->STACK), 3, __GET_VAR(data__->QOD4));


goto __end;

__end:
  return;
} // SM_INDUSTRIAL_body__()




int basSetTriacs(uint8_t, uint8_t);
int basSet0_10Vout(uint8_t, uint8_t, float);
int basGetDryContacts(uint8_t, uint8_t*);
int basGetUniversalIn(uint8_t, uint8_t, uint8_t, float*);
int basGet1WbTemp(uint8_t, uint8_t, float*);


static void SM_BAS_init__(SM_BAS *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
  __INIT_VAR(data__->TRIAC1,0,retain)
  __INIT_VAR(data__->TRIAC2,0,retain)
  __INIT_VAR(data__->TRIAC3,0,retain)
  __INIT_VAR(data__->TRIAC4,0,retain)
  __INIT_VAR(data__->LED1,0,retain)
  __INIT_VAR(data__->LED2,0,retain)
  __INIT_VAR(data__->LED3,0,retain)
  __INIT_VAR(data__->LED4,0,retain)
  __INIT_VAR(data__->IN1_T,0,retain) 
  __INIT_VAR(data__->IN2_T,0,retain) 
  __INIT_VAR(data__->IN3_T,0,retain) 
  __INIT_VAR(data__->IN4_T,0,retain) 
  __INIT_VAR(data__->IN5_T,0,retain) 
  __INIT_VAR(data__->IN6_T,0,retain) 
  __INIT_VAR(data__->IN7_T,0,retain) 
  __INIT_VAR(data__->IN8_T,0,retain)   
  __INIT_VAR(data__->Q0_10V1,0,retain)
  __INIT_VAR(data__->Q0_10V2,0,retain)
  __INIT_VAR(data__->Q0_10V3,0,retain)
  __INIT_VAR(data__->Q0_10V4,0,retain)
  __INIT_VAR(data__->UNIV1,0,retain)
  __INIT_VAR(data__->UNIV2,0,retain)
  __INIT_VAR(data__->UNIV3,0,retain)
  __INIT_VAR(data__->UNIV4,0,retain)
  __INIT_VAR(data__->UNIV5,0,retain)
  __INIT_VAR(data__->UNIV6,0,retain)
  __INIT_VAR(data__->UNIV7,0,retain)
  __INIT_VAR(data__->UNIV8,0,retain)
  __INIT_VAR(data__->DRY_C1,0,retain)
  __INIT_VAR(data__->DRY_C2,0,retain)
  __INIT_VAR(data__->DRY_C3,0,retain)
  __INIT_VAR(data__->DRY_C4,0,retain)
  __INIT_VAR(data__->DRY_C5,0,retain)
  __INIT_VAR(data__->DRY_C6,0,retain)
  __INIT_VAR(data__->DRY_C7,0,retain)
  __INIT_VAR(data__->DRY_C8,0,retain)
  __INIT_VAR(data__->OWB_T1,0,retain)
  __INIT_VAR(data__->OWB_T2,0,retain)
  __INIT_VAR(data__->OWB_T3,0,retain)
  __INIT_VAR(data__->OWB_T4,0,retain)
}



static void SM_BAS_body__(SM_BAS *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  uint8_t output_byte = __GET_VAR(data__->LED4) << 7 |
						__GET_VAR(data__->LED3) << 6 |
						__GET_VAR(data__->LED2) << 5 |
						__GET_VAR(data__->LED1) << 4 |
						__GET_VAR(data__->TRIAC4) << 3 | 
                        __GET_VAR(data__->TRIAC3) << 2 | 
                        __GET_VAR(data__->TRIAC2) << 1 | 
                        __GET_VAR(data__->TRIAC1);
  
  basSetTriacs(__GET_VAR(data__->STACK), output_byte);
  #define bitRead(value, bit) (((value) >> (bit)) & 0x01)
  uint8_t input_byte = 0;
  if(0 == basGetDryContacts(__GET_VAR(data__->STACK), &input_byte)){
	__SET_VAR(data__->,DRY_C1,,bitRead(input_byte, 0));
	__SET_VAR(data__->,DRY_C2,,bitRead(input_byte, 1));
	__SET_VAR(data__->,DRY_C3,,bitRead(input_byte, 2));
	__SET_VAR(data__->,DRY_C4,,bitRead(input_byte, 3));
	__SET_VAR(data__->,DRY_C5,,bitRead(input_byte, 4));
	__SET_VAR(data__->,DRY_C6,,bitRead(input_byte, 5));
	__SET_VAR(data__->,DRY_C7,,bitRead(input_byte, 6));
	__SET_VAR(data__->,DRY_C8,,bitRead(input_byte, 7));	
  }
  float val = 0;

// ---------  Universal  inputs	------------------
  if( 0 == basGetUniversalIn(__GET_VAR(data__->STACK), 0, __GET_VAR(data__->IN1_T), &val))
  {
	__SET_VAR(data__->,UNIV1,,val);
  }
  if( 0 == basGetUniversalIn(__GET_VAR(data__->STACK), 1, __GET_VAR(data__->IN2_T), &val))
  {
	__SET_VAR(data__->,UNIV2,,val);
  }
  if( 0 == basGetUniversalIn(__GET_VAR(data__->STACK), 2, __GET_VAR(data__->IN3_T), &val))
  {
	__SET_VAR(data__->,UNIV3,,val);
  }
  if( 0 == basGetUniversalIn(__GET_VAR(data__->STACK), 3, __GET_VAR(data__->IN4_T), &val))
  {
	__SET_VAR(data__->,UNIV4,,val);
  }
  if( 0 == basGetUniversalIn(__GET_VAR(data__->STACK), 4, __GET_VAR(data__->IN5_T), &val))
  {
	__SET_VAR(data__->,UNIV5,,val);
  }
  if( 0 == basGetUniversalIn(__GET_VAR(data__->STACK), 5, __GET_VAR(data__->IN6_T), &val))
  {
	__SET_VAR(data__->,UNIV6,,val);
  }
  if( 0 == basGetUniversalIn(__GET_VAR(data__->STACK), 6, __GET_VAR(data__->IN7_T), &val))
  {
	__SET_VAR(data__->,UNIV7,,val);
  }
  if( 0 == basGetUniversalIn(__GET_VAR(data__->STACK), 7, __GET_VAR(data__->IN8_T), &val))
  {
	__SET_VAR(data__->,UNIV8,,val);
  }


// --------------- One wire bus temperature inputs (dsb20 sensors)
  if( 0 == basGet1WbTemp(__GET_VAR(data__->STACK), 0, &val))
  {
	__SET_VAR(data__->,OWB_T1,,val);
  }
  if( 0 == basGet1WbTemp(__GET_VAR(data__->STACK), 1, &val))
  {
	__SET_VAR(data__->,OWB_T2,,val);
  }
  if( 0 == basGet1WbTemp(__GET_VAR(data__->STACK), 2, &val))
  {
	__SET_VAR(data__->,OWB_T3,,val);
  }
  if( 0 == basGet1WbTemp(__GET_VAR(data__->STACK), 3, &val))
  {
	__SET_VAR(data__->,OWB_T4,,val);
  }

// ----------------- 0-10V outputs ---------------------------------------
  basSet0_10Vout(__GET_VAR(data__->STACK), 0, __GET_VAR(data__->Q0_10V1));
  basSet0_10Vout(__GET_VAR(data__->STACK), 1, __GET_VAR(data__->Q0_10V2));
  basSet0_10Vout(__GET_VAR(data__->STACK), 2, __GET_VAR(data__->Q0_10V3));
  basSet0_10Vout(__GET_VAR(data__->STACK), 3, __GET_VAR(data__->Q0_10V4));



goto __end;

__end:
  return;
} // SM_BAS_body__()



static void SM_HOME_init__(SM_HOME *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
  __INIT_VAR(data__->RELAY1,0,retain)
  __INIT_VAR(data__->RELAY2,0,retain)
  __INIT_VAR(data__->RELAY3,0,retain)
  __INIT_VAR(data__->RELAY4,0,retain)
  __INIT_VAR(data__->RELAY5,0,retain)
  __INIT_VAR(data__->RELAY6,0,retain)
  __INIT_VAR(data__->RELAY7,0,retain)
  __INIT_VAR(data__->RELAY8,0,retain)
  __INIT_VAR(data__->Q0_10V1,0,retain)
  __INIT_VAR(data__->Q0_10V2,0,retain)
  __INIT_VAR(data__->Q0_10V3,0,retain)
  __INIT_VAR(data__->Q0_10V4,0,retain)
  __INIT_VAR(data__->QOD1,0,retain)
  __INIT_VAR(data__->QOD2,0,retain)
  __INIT_VAR(data__->QOD3,0,retain)
  __INIT_VAR(data__->QOD4,0,retain)
  __INIT_VAR(data__->OPTO1,0,retain)
  __INIT_VAR(data__->OPTO2,0,retain)
  __INIT_VAR(data__->OPTO3,0,retain)
  __INIT_VAR(data__->OPTO4,0,retain)
  __INIT_VAR(data__->OPTO5,0,retain)
  __INIT_VAR(data__->OPTO6,0,retain)
  __INIT_VAR(data__->OPTO7,0,retain)
  __INIT_VAR(data__->OPTO8,0,retain)
  __INIT_VAR(data__->ADC1,0,retain)
  __INIT_VAR(data__->ADC2,0,retain)
  __INIT_VAR(data__->ADC3,0,retain)
  __INIT_VAR(data__->ADC4,0,retain)
  __INIT_VAR(data__->ADC5,0,retain)
  __INIT_VAR(data__->ADC6,0,retain)
  __INIT_VAR(data__->ADC7,0,retain)
  __INIT_VAR(data__->ADC8,0,retain)
  __INIT_VAR(data__->OWB_T1,0,retain)
  __INIT_VAR(data__->OWB_T2,0,retain)
  __INIT_VAR(data__->OWB_T3,0,retain)
  __INIT_VAR(data__->OWB_T4,0,retain)
}

// Code part

int homeSetRelays(uint8_t, uint8_t);
int homeSet0_10Vout(uint8_t, uint8_t, float);
int homeSetOD(uint8_t, uint8_t, float);
int homeGetOpto(uint8_t, uint8_t*);
int homeGetADC(uint8_t, uint8_t, float*);
int homeGet1WbTemp(uint8_t, uint8_t, float*);

static void SM_HOME_body__(SM_HOME *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  uint8_t output_byte = __GET_VAR(data__->RELAY8) << 7 |
						__GET_VAR(data__->RELAY7) << 6 |
						__GET_VAR(data__->RELAY6) << 5 |
						__GET_VAR(data__->RELAY5) << 4 |
						__GET_VAR(data__->RELAY4) << 3 | 
                        __GET_VAR(data__->RELAY3) << 2 | 
                        __GET_VAR(data__->RELAY2) << 1 | 
                        __GET_VAR(data__->RELAY1);
  
  homeSetRelays(__GET_VAR(data__->STACK), output_byte);
  #define bitRead(value, bit) (((value) >> (bit)) & 0x01)
  uint8_t input_byte = 0;
  if(0 == homeGetOpto(__GET_VAR(data__->STACK), &input_byte)){
	__SET_VAR(data__->,OPTO1,,bitRead(input_byte, 0));
	__SET_VAR(data__->,OPTO2,,bitRead(input_byte, 1));
	__SET_VAR(data__->,OPTO3,,bitRead(input_byte, 2));
	__SET_VAR(data__->,OPTO4,,bitRead(input_byte, 3));
	__SET_VAR(data__->,OPTO5,,bitRead(input_byte, 4));
	__SET_VAR(data__->,OPTO6,,bitRead(input_byte, 5));
	__SET_VAR(data__->,OPTO7,,bitRead(input_byte, 6));
	__SET_VAR(data__->,OPTO8,,bitRead(input_byte, 7));	
  }
  float val = 0;

// ---------  ADC  inputs	------------------
  if( 0 == homeGetADC(__GET_VAR(data__->STACK), 0, &val))
  {
	__SET_VAR(data__->,ADC1,,val);
  }
  if( 0 == homeGetADC(__GET_VAR(data__->STACK), 1, &val))
  {
	__SET_VAR(data__->,ADC2,,val);
  }
  if( 0 == homeGetADC(__GET_VAR(data__->STACK), 2, &val))
  {
	__SET_VAR(data__->,ADC3,,val);
  }
  if( 0 == homeGetADC(__GET_VAR(data__->STACK), 3, &val))
  {
	__SET_VAR(data__->,ADC4,,val);
  }
  if( 0 == homeGetADC(__GET_VAR(data__->STACK), 4, &val))
  {
	__SET_VAR(data__->,ADC5,,val);
  }
  if( 0 == homeGetADC(__GET_VAR(data__->STACK), 5, &val))
  {
	__SET_VAR(data__->,ADC6,,val);
  }
  if( 0 == homeGetADC(__GET_VAR(data__->STACK), 6, &val))
  {
	__SET_VAR(data__->,ADC7,,val);
  }
  if( 0 == homeGetADC(__GET_VAR(data__->STACK), 7, &val))
  {
	__SET_VAR(data__->,ADC8,,val);
  }
 

// --------------- One wire bus temperature inputs (dsb20 sensors)
  if( 0 == homeGet1WbTemp(__GET_VAR(data__->STACK), 0, &val))
  {
	__SET_VAR(data__->,OWB_T1,,val);
  }
  if( 0 == homeGet1WbTemp(__GET_VAR(data__->STACK), 1, &val))
  {
	__SET_VAR(data__->,OWB_T2,,val);
  }
  if( 0 == homeGet1WbTemp(__GET_VAR(data__->STACK), 2, &val))
  {
	__SET_VAR(data__->,OWB_T3,,val);
  }
  if( 0 == homeGet1WbTemp(__GET_VAR(data__->STACK), 3, &val))
  {
	__SET_VAR(data__->,OWB_T4,,val);
  }

// ----------------- 0-10V outputs ---------------------------------------
  homeSet0_10Vout(__GET_VAR(data__->STACK), 0, __GET_VAR(data__->Q0_10V1));
  homeSet0_10Vout(__GET_VAR(data__->STACK), 1, __GET_VAR(data__->Q0_10V2));
  homeSet0_10Vout(__GET_VAR(data__->STACK), 2, __GET_VAR(data__->Q0_10V3));
  homeSet0_10Vout(__GET_VAR(data__->STACK), 3, __GET_VAR(data__->Q0_10V4));

  // -------------- Open-Drain Outputs -------------------------------------
  homeSetOD(__GET_VAR(data__->STACK), 0, __GET_VAR(data__->QOD1));
  homeSetOD(__GET_VAR(data__->STACK), 1, __GET_VAR(data__->QOD2));
  homeSetOD(__GET_VAR(data__->STACK), 2, __GET_VAR(data__->QOD3));
  homeSetOD(__GET_VAR(data__->STACK), 3, __GET_VAR(data__->QOD4));
  


goto __end;

__end:
  return;
} // SM_HOME_body__()


static void SM_8MOSFET_init__(SM_8MOSFET *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STACK,0,retain)
  __INIT_VAR(data__->DUMMY,0,retain)
  __INIT_VAR(data__->MOS1,0,retain)
  __INIT_VAR(data__->MOS2,0,retain)
  __INIT_VAR(data__->MOS3,0,retain)
  __INIT_VAR(data__->MOS4,0,retain)
  __INIT_VAR(data__->MOS5,0,retain)
  __INIT_VAR(data__->MOS6,0,retain)
  __INIT_VAR(data__->MOS7,0,retain)
  __INIT_VAR(data__->MOS8,0,retain)

}

// Code part

int mosfet8Init(int);
int mosfets8Set(uint8_t, uint8_t);

static void SM_8MOSFET_body__(SM_8MOSFET *data__) {
	static uint8_t init = 0;
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Dummy code - just for editor simulation. Real code is inside sm_cards.h file on arduino folder
  __SET_VAR(data__->,DUMMY,,0);
   if(init == 0)
  {
	  if(0 == mosfet8Init(__GET_VAR(data__->STACK)))
	  {
		  init = 1;
	  }
  }
  else // already init
  {
	uint8_t output_byte = __GET_VAR(data__->MOS8) << 7 | 
                        __GET_VAR(data__->MOS7) << 6 | 
                        __GET_VAR(data__->MOS6) << 5 | 
                        __GET_VAR(data__->MOS5) << 4 | 
                        __GET_VAR(data__->MOS4) << 3 | 
                        __GET_VAR(data__->MOS3) << 2 | 
                        __GET_VAR(data__->MOS2) << 1 | 
                        __GET_VAR(data__->MOS1);
	mosfets8Set(__GET_VAR(data__->STACK), output_byte);
  }
  goto __end;

__end:
  return;
} // SM_8MOSFET_body__()

