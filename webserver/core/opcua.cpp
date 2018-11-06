#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ladder.h"

#define OPLC_CYCLE              50000000

using namespace std;

/* Booleans */
static IEC_BOOL last_bool_input[BUFFER_SIZE][8];
static IEC_BOOL last_bool_output[BUFFER_SIZE][8];
static PyObject *py_bool_input[BUFFER_SIZE][8];
static PyObject *py_bool_output[BUFFER_SIZE][8];

/* Bytes */
static IEC_BYTE last_byte_input[BUFFER_SIZE];
static IEC_BYTE last_byte_output[BUFFER_SIZE];

/* Analog I/O */
static IEC_UINT last_int_input[BUFFER_SIZE];
static IEC_UINT last_int_output[BUFFER_SIZE];
static PyObject *py_int_input[BUFFER_SIZE];
static PyObject *py_int_output[BUFFER_SIZE];

/* Memory */
static IEC_UINT last_int_memory[BUFFER_SIZE];
static IEC_DINT last_dint_memory[BUFFER_SIZE];
static IEC_LINT last_lint_memory[BUFFER_SIZE];

PyObject *
callPythonFunc(PyObject* pObj, const char *funcName, PyObject* pArgs, PyObject *pKw)
{
    PyObject *pValue;
    PyObject *pFunc = PyObject_GetAttrString(pObj, funcName);
    /* pFunc is a new reference */
    if (pFunc && PyCallable_Check(pFunc)) {
        if (pKw)
            pValue = PyObject_Call(pFunc, pArgs, pKw);
        else
            pValue = PyObject_CallObject(pFunc, pArgs);
        if (pValue != NULL) {
            Py_DECREF(pFunc);
            return pValue;
        }
        else {
            Py_DECREF(pFunc);
            PyErr_Print();
            fprintf(stderr, "Error calling \"%s\"\n", funcName);
            return NULL;
        }
    }
    else {
        if (PyErr_Occurred())
            PyErr_Print();
        fprintf(stderr, "Cannot find function \"%s\"\n", funcName);
    }
    Py_XDECREF(pFunc);
}

void
opcuaStartServer(int port)
{
    time_t timestamp;

    Py_Initialize();
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pKw, *pValue;
    PyObject *pServer, *pIdx, *pObjects, *pOpenPlcObj;

    /* from opcua import Server */
    pName = PyString_FromString("opcua");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (pModule != NULL) {
        /* server = Server() */
        pServer = callPythonFunc(pModule, "Server", NULL, NULL);
        if (pServer == NULL) {
            Py_DECREF(pModule);
            return;
        }

        /* server.set_endpoint("opc.tcp://localhost:{}/freeopcua/server/".format(port)) */
        pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, PyString_FromFormat("opc.tcp://localhost:%d/freeopcua/server", port));
        pValue = callPythonFunc(pServer, "set_endpoint", pArgs, NULL);
        Py_DECREF(pArgs);
        if (pValue == NULL) {
            Py_DECREF(pModule);
            return;
        }
        Py_DECREF(pValue);

        /* Setup our own namespace, not really necessary but should as spec */
        /* uri = "http://examples.freeopcua.github.io"
         * idx = server.register_namespace(uri)
         */
        pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, PyString_FromString("http://examples.freeopcua.github.io"));
        pIdx = callPythonFunc(pServer, "register_namespace", pArgs, NULL);
        if (pIdx == NULL) {
            Py_DECREF(pModule);
            return;
        }

        /* Get Objects node, this is where we should put our nodes */
        pObjects = callPythonFunc(pServer, "get_objects_node", NULL, NULL);
        if (pObjects == NULL) {
            Py_DECREF(pModule);
            return;
        }

        /* Populating our address space */
        /* openPlcObj = objects.add_object(idx, "OpenPLC") */
        pArgs = PyTuple_New(2);
        PyTuple_SetItem(pArgs, 0, pIdx);
        PyTuple_SetItem(pArgs, 1, PyString_FromString("OpenPLC"));
        pOpenPlcObj = callPythonFunc(pObjects, "add_object", pArgs, NULL);
        Py_DECREF(pArgs);

        /* Add each input/output */
        /* Address mapping can be found in http://www.openplcproject.com/scada
         *
         * Refer to
         * https://python-opcua.readthedocs.io/en/latest/_modules/opcua/ua/uatypes.html
         * for the definitions of variant types
         */
        /* opcua.ua.VariantType.UInt16 */
        PyObject *opcua_ua = PyObject_GetAttrString(pModule, "ua");
        PyObject *opcua_ua_VariantType = PyObject_GetAttrString(opcua_ua, "VariantType");
        PyObject *opcua_ua_VariantType_UInt16 = PyObject_GetAttrString(opcua_ua_VariantType, "UInt16");
        for(int i = 0; i < 100; ++i) {
            for (int j = 0; j < 8; ++j) {
                /* Digital inputs */
                if (bool_input[i][j]) {
                    pArgs = PyTuple_New(3);
                    PyTuple_SetItem(pArgs, 0, pIdx);
                    PyTuple_SetItem(pArgs, 1, PyString_FromFormat("%%IX%d.%d", i, j));
                    if (*bool_input[i][j]) {
                        Py_INCREF(Py_True);
                        PyTuple_SetItem(pArgs, 2, Py_True);
                    }
                    else {
                        Py_INCREF(Py_False);
                        PyTuple_SetItem(pArgs, 2, Py_False);
                    }
                    py_bool_input[i][j] = callPythonFunc(pOpenPlcObj, "add_variable", pArgs, NULL);
                    Py_DECREF(pArgs);
                    pValue = callPythonFunc(py_bool_input[i][j], "set_writable", NULL, NULL);
                    Py_DECREF(pValue);
                }
                /* Digital outputs */
                if (bool_output[i][j]) {
                    pArgs = PyTuple_New(3);
                    PyTuple_SetItem(pArgs, 0, pIdx);
                    PyTuple_SetItem(pArgs, 1, PyString_FromFormat("%%QX%d.%d", i, j));
                    if (*bool_output[i][j]) {
                        Py_INCREF(Py_True);
                        PyTuple_SetItem(pArgs, 2, Py_True);
                    }
                    else {
                        Py_INCREF(Py_False);
                        PyTuple_SetItem(pArgs, 2, Py_False);
                    }
                    py_bool_output[i][j] = callPythonFunc(pOpenPlcObj, "add_variable", pArgs, NULL);
                    Py_DECREF(pArgs);
                    pValue = callPythonFunc(py_bool_output[i][j], "set_read_only", NULL, NULL);
                    Py_DECREF(pValue);
                }
            }
            /* Analog inputs */
            if (int_input[i]) {
                    pArgs = PyTuple_New(3);
                    PyTuple_SetItem(pArgs, 0, pIdx);
                    PyTuple_SetItem(pArgs, 1, PyString_FromFormat("%%IW%d", i));
                    PyTuple_SetItem(pArgs, 2, PyInt_FromLong(*int_input[i]));
                    pKw = PyDict_New();
                    PyDict_SetItemString(pKw, "varianttype", opcua_ua_VariantType_UInt16);
                    py_int_input[i] = callPythonFunc(pOpenPlcObj, "add_variable", pArgs, pKw);
                    Py_DECREF(pKw);
                    Py_DECREF(pArgs);
                    pValue = callPythonFunc(py_int_input[i], "set_writable", NULL, NULL);
                    Py_DECREF(pValue);
            }
            /* Analog outputs */
            if (int_output[i]) {
                    pArgs = PyTuple_New(3);
                    PyTuple_SetItem(pArgs, 0, pIdx);
                    PyTuple_SetItem(pArgs, 1, PyString_FromFormat("%%QW%d", i));
                    PyTuple_SetItem(pArgs, 2, PyInt_FromLong(*int_output[i]));
                    pKw = PyDict_New();
                    PyDict_SetItemString(pKw, "varianttype", opcua_ua_VariantType_UInt16);
                    py_int_output[i] = callPythonFunc(pOpenPlcObj, "add_variable", pArgs, pKw);
                    Py_DECREF(pKw);
                    Py_DECREF(pArgs);
                    pValue = callPythonFunc(py_int_output[i], "set_read_only", NULL, NULL);
                    Py_DECREF(pValue);
            }
        }
        Py_DECREF(opcua_ua_VariantType_UInt16);
        Py_DECREF(opcua_ua_VariantType);
        Py_DECREF(opcua_ua);

        /* server.start() */
        pValue = callPythonFunc(pServer, "start", NULL, NULL);
        if (pValue == NULL) {
            Py_DECREF(pModule);
            return;
        }
        Py_DECREF(pValue);
        printf("OPC UA Server Enabled\n");

        /* Continuously update */
        struct timespec timer_start;
        clock_gettime(CLOCK_MONOTONIC, &timer_start);

        while(run_opcua) {
            for(int i = 0; i < 100; ++i) {
                for (int j = 0; j < 8; ++j) {
                    if (bool_input[i][j]) {
                        /* Copy the input value to OpenPLC */
                        pValue = callPythonFunc(py_bool_input[i][j], "get_value", NULL, NULL);
                        if (pValue && PyBool_Check(pValue)) {
                            if (PyObject_IsTrue(pValue))
                                *bool_input[i][j] = 1;
                            else
                                *bool_input[i][j] = 0;
                        }
                        Py_DECREF(pValue);
                        if (*bool_input[i][j] != last_bool_input[i][j]) {
                            timestamp = time(NULL);
                            printf("%s Bool Input [%d,%d] value changed to %s\n", ctime(&timestamp), i, j, *bool_input[i][j]?"true":"false");
                        }
                        last_bool_input[i][j] = *bool_input[i][j];
                    }
                    if (bool_output[i][j]) {
                        /* Copy the output value to OPC UA server */
                        pArgs = PyTuple_New(1);
                        if (*bool_output[i][j]) {
                            Py_INCREF(Py_True);
                            PyTuple_SetItem(pArgs, 0, Py_True);
                        }
                        else {
                            Py_INCREF(Py_False);
                            PyTuple_SetItem(pArgs, 0, Py_False);
                        }
                        pValue = callPythonFunc(py_bool_output[i][j], "set_value", pArgs, NULL);
                        Py_DECREF(pArgs);
                        Py_DECREF(pValue);
                        if (*bool_output[i][j] != last_bool_output[i][j]) {
                            timestamp = time(NULL);
                            printf("%s Bool Output [%d,%d] value changed to %s\n", ctime(&timestamp), i, j, *bool_output[i][j]?"true":"false");
                        }
                        last_bool_output[i][j] = *bool_output[i][j];
                    }
                }

                if (byte_input[i]) {
                    if (*byte_input[i] != last_byte_input[i]) {
                        timestamp = time(NULL);
                        printf("%s Byte Input [%d] value changed to %u\n", ctime(&timestamp), i, *byte_input[i]);
                    }
                    last_byte_input[i] = *byte_input[i];
                }
                if (byte_output[i]) {
                    if (*byte_output[i] != last_byte_output[i]) {
                        timestamp = time(NULL);
                        printf("%s Byte Output [%d] value changed to %u\n", ctime(&timestamp), i, *byte_output[i]);
                    }
                    last_byte_output[i] = *byte_output[i];
                }
                if (int_input[i]) {
                    /* Copy the input value to OpenPLC */
                    pValue = callPythonFunc(py_int_input[i], "get_value", NULL, NULL);
                    *int_input[i] = (IEC_UINT) PyInt_AsLong(pValue);
                    Py_DECREF(pValue);
                    if (*int_input[i] != last_int_input[i]) {
                        timestamp = time(NULL);
                        printf("%s Int Input [%d] value changed to %u\n", ctime(&timestamp), i, *int_input[i]);
                    }
                    last_int_input[i] = *int_input[i];
                }
                if (int_output[i]) {
                    /* Copy the out value to OPC UA server */
                    pArgs = PyTuple_New(1);
                    PyTuple_SetItem(pArgs, 0, PyInt_FromLong(*int_output[i]));
                    pValue = callPythonFunc(py_int_output[i], "set_value", pArgs, NULL);
                    Py_DECREF(pArgs);
                    Py_DECREF(pValue);
                    if (*int_output[i] != last_int_output[i]) {
                        timestamp = time(NULL);
                        printf("%s Int Output [%d] value changed to %u\n", ctime(&timestamp), i, *int_output[i]);
                    }
                    last_int_output[i] = *int_output[i];
                }
            }
            sleep_until(&timer_start, OPLC_CYCLE);
        }

        /* server.stop() */
        pValue = callPythonFunc(pServer, "stop", NULL, NULL);
        Py_DECREF(pValue);

        Py_DECREF(pOpenPlcObj);
        Py_DECREF(pObjects);
        Py_DECREF(pIdx);
        Py_DECREF(pServer);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", "opcua");
        return;
    }

    Py_Finalize();
}
