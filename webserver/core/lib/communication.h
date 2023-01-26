/************************************************************************
 *                DECLARATION OF COMMUNICATION BLOCKS                   *
************************************************************************/
// TCP_CONNECT
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(BOOL,CONNECT)
  __DECLARE_VAR(STRING,IP_ADDRESS)
  __DECLARE_VAR(INT,PORT)
  __DECLARE_VAR(BOOL,UDP)
  __DECLARE_VAR(INT,SOCKET_ID)

  // FB private variables - TEMP, private and located variables

} TCP_CONNECT;


// TCP_SEND
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(BOOL,SEND)
  __DECLARE_VAR(INT,SOCKET_ID)
  __DECLARE_VAR(STRING,MSG)
  __DECLARE_VAR(INT,BYTES_SENT)

  // FB private variables - TEMP, private and located variables

} TCP_SEND;


// TCP_RECEIVE
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(BOOL,RECEIVE)
  __DECLARE_VAR(INT,SOCKET_ID)
  __DECLARE_VAR(INT,BYTES_RECEIVED)
  __DECLARE_VAR(STRING,MSG)

  // FB private variables - TEMP, private and located variables

} TCP_RECEIVE;


// TCP_CLOSE
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(BOOL,CLOSE)
  __DECLARE_VAR(INT,SOCKET_ID)
  __DECLARE_VAR(INT,SUCCESS)

  // FB private variables - TEMP, private and located variables

} TCP_CLOSE;


/************************************************************************
 *                 END OF COMMUNICATION LIB BLOCKS                      *
************************************************************************/

/************************************************************************
 *              DECLARATION OF COMMUNICATION LIB BLOCKS                 *
************************************************************************/
//client.cpp
int connect_to_tcp_server(uint8_t *ip_address, uint16_t port, int method);
int send_tcp_message(uint8_t *msg, size_t msg_size, int socket_id);
int receive_tcp_message(uint8_t *msg_buffer, size_t buffer_size, int socket_id);
int close_tcp_connection(int socket_id);

static void TCP_CONNECT_init__(TCP_CONNECT *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->CONNECT,0,retain)
  __INIT_VAR(data__->IP_ADDRESS,__STRING_LITERAL(0,""),retain)
  __INIT_VAR(data__->PORT,0,retain)
  __INIT_VAR(data__->UDP,0,retain)
  __INIT_VAR(data__->SOCKET_ID,0,retain)
}

// Code part
static void TCP_CONNECT_body__(TCP_CONNECT *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Block code
  //(*data__).SOCKET_ID = connect_to_tcp_server((*data__).IP_ADDRESS.body, (*data__).PORT);
  
  if (__GET_VAR(data__->CONNECT))
  {
    #define GetFbVar(var,...) __GET_VAR(data__->var,__VA_ARGS__)
    #define SetFbVar(var,val,...) __SET_VAR(data__->,var,__VA_ARGS__,val)

    IEC_STRING tcp_ip = GetFbVar(IP_ADDRESS);
    int tcp_port = GetFbVar(PORT);
    int method = GetFbVar(UDP);

    int tcp_socket = connect_to_tcp_server(tcp_ip.body, tcp_port, method);
    SetFbVar(SOCKET_ID, tcp_socket);

    #undef GetFbVar
    #undef SetFbVar
  }

  goto __end;

__end:
  return;
} // TCP_CONNECT_body__()


static void TCP_SEND_init__(TCP_SEND *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->SEND,0,retain)
  __INIT_VAR(data__->SOCKET_ID,0,retain)
  __INIT_VAR(data__->MSG,__STRING_LITERAL(0,""),retain)
  __INIT_VAR(data__->BYTES_SENT,0,retain)
}

// Code part
static void TCP_SEND_body__(TCP_SEND *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Block code
  if (__GET_VAR(data__->SEND))
  {
    #define GetFbVar(var,...) __GET_VAR(data__->var,__VA_ARGS__)
    #define SetFbVar(var,val,...) __SET_VAR(data__->,var,__VA_ARGS__,val)

    IEC_STRING msg_buffer = GetFbVar(MSG);
    int tcp_socket = GetFbVar(SOCKET_ID);
    int sent_bytes = send_tcp_message(msg_buffer.body, msg_buffer.len, tcp_socket);
    SetFbVar(BYTES_SENT, sent_bytes);

    #undef GetFbVar
    #undef SetFbVar
  }

  goto __end;

__end:
  return;
} // TCP_SEND_body__()

static void TCP_RECEIVE_init__(TCP_RECEIVE *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->RECEIVE,0,retain)
  __INIT_VAR(data__->SOCKET_ID,0,retain)
  __INIT_VAR(data__->BYTES_RECEIVED,0,retain)
  __INIT_VAR(data__->MSG,__STRING_LITERAL(0,""),retain)
}

// Code part
static void TCP_RECEIVE_body__(TCP_RECEIVE *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Block code
  if (__GET_VAR(data__->RECEIVE))
  {
    #define GetFbVar(var,...) __GET_VAR(data__->var,__VA_ARGS__)
    #define SetFbVar(var,val,...) __SET_VAR(data__->,var,__VA_ARGS__,val)

    IEC_STRING msg_buffer = GetFbVar(MSG);
    int tcp_socket = GetFbVar(SOCKET_ID);
    int rcv_bytes = receive_tcp_message(msg_buffer.body, STR_MAX_LEN, tcp_socket);
    if (rcv_bytes < 0)
        msg_buffer.len = 0;
    else
        msg_buffer.len = (__strlen_t)rcv_bytes;
    SetFbVar(MSG, msg_buffer);
    SetFbVar(BYTES_RECEIVED, rcv_bytes);

    #undef GetFbVar
    #undef SetFbVar
  }

  goto __end;

__end:
  return;
} // TCP_RECEIVE_body__()


static void TCP_CLOSE_init__(TCP_CLOSE *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->CLOSE,0,retain)
  __INIT_VAR(data__->SOCKET_ID,0,retain)
  __INIT_VAR(data__->SUCCESS,0,retain)
}

// Code part
static void TCP_CLOSE_body__(TCP_CLOSE *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Dummy code - just for editor simulation. Real code is inside p1am_FB.h file on arduino folder
  __SET_VAR(data__->,SUCCESS,,0);

  goto __end;

__end:
  return;
} // TCP_CLOSE_body__()

/************************************************************************
 *                  END OF COMMUNICATION LIB BLOCK                      *
************************************************************************/
