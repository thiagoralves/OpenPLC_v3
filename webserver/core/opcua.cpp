#include <open62541/server.h>
#include "ladder.h"

using namespace std;

static unsigned char log_msg[1000];

/* Booleans */
static IEC_BOOL last_bool_input[BUFFER_SIZE][8];
static IEC_BOOL last_bool_output[BUFFER_SIZE][8];
//static PyObject *py_bool_input[BUFFER_SIZE][8];
//static PyObject *py_bool_output[BUFFER_SIZE][8];

/* Bytes */
static IEC_BYTE last_byte_input[BUFFER_SIZE];
static IEC_BYTE last_byte_output[BUFFER_SIZE];

/* Analog I/O */
static IEC_UINT last_int_input[BUFFER_SIZE];
static IEC_UINT last_int_output[BUFFER_SIZE];
//static PyObject *py_int_input[BUFFER_SIZE];
//static PyObject *py_int_output[BUFFER_SIZE];

/* Memory */
static IEC_UINT last_int_memory[BUFFER_SIZE];
static IEC_DINT last_dint_memory[BUFFER_SIZE];
static IEC_LINT last_lint_memory[BUFFER_SIZE];

void opcuaStartServer(int port)
{
    UA_Server *server = UA_Server_new();

    // add a variable node to the adresspace
    // ToDo add address space
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_Int32 myInteger = 42;
    UA_Variant_setScalarCopy(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US","the answer");
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US","the answer");
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING_ALLOC(1, "the.answer");
    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME_ALLOC(1, "the answer");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
                              parentReferenceNodeId, myIntegerName,
                              UA_NODEID_NULL, attr, NULL, NULL);

    /* allocations on the heap need to be freed */
    UA_VariableAttributes_clear(&attr);
    UA_NodeId_clear(&myIntegerNodeId);
    UA_QualifiedName_clear(&myIntegerName);

    UA_StatusCode retval = UA_Server_runUntilInterrupt(server);
}