import threading
import time
from pymodbus.server.sync import ModbusTcpServer
from pymodbus.datastore import ModbusSequentialDataBlock, ModbusSlaveContext, ModbusServerContext
from enum import Enum

class var_type(Enum):
    NONE    = 0
    DIG_INP = 1
    DIG_OUT = 2
    ANA_INP = 3
    ANA_OUT = 4
    
KILL_SIGNAL = 127

#Initialize points database
d_inputs = ModbusSequentialDataBlock(0, [0]*400)
d_outputs = ModbusSequentialDataBlock(0, [0]*400)
a_inputs = ModbusSequentialDataBlock(0, [0]*50)
a_outputs = ModbusSequentialDataBlock(0, [0]*51)

def extract_variable(variable_name):
    #init variables
    io_type = var_type.NONE
    location = ""
    address = 0

    #remove the optional '%' of the variable
    if ("%" in variable_name):
        location = variable_name.split("%")[1]
    else:
        location = variable_name

    #get io_type and address
    if ("IX" in location):
        io_type = var_type.DIG_INP
        location = location.split("IX")[1]
        if ("." in location):
            address = 8*(int(location.split(".")[0])) + int(location.split(".")[1])
        else:
            address = int(location)
            
    elif ("QX" in location):
        io_type = var_type.DIG_OUT
        location = location.split("QX")[1]
        if ("." in location):
            address = 8*(int(location.split(".")[0])) + int(location.split(".")[1])
        else:
            address = int(location)
            
    elif ("IW" in location):
        io_type = var_type.ANA_INP
        location = location.split("IW")[1]
        if ("." in location):
            #invalid location
            io_type = var_type.NONE
            location = ""
            address = 0
        else:
            address = int(location)
            
    elif ("QW" in location):
        io_type = var_type.ANA_OUT
        location = location.split("QW")[1]
        if ("." in location):
            #invalid location
            io_type = var_type.NONE
            location = ""
            address = 0
        else:
            address = int(location)
            
    else:
        io_type = var_type.NONE

    return io_type, address
    
def get_var(variable_name):
    var = extract_variable(variable_name)
    io_type = var[0]
    address = var[1]

    if (io_type == var_type.DIG_INP):
        return d_inputs.getValues(address)[0]

    elif (io_type == var_type.DIG_OUT):
        return d_outputs.getValues(address)[0]

    elif (io_type == var_type.ANA_INP):
        return a_inputs.getValues(address)[0]

    elif (io_type == var_type.ANA_OUT):
        return a_outputs.getValues(address)[0]

    else:
        return 0
    

def set_var(variable_name, value):
    var = extract_variable(variable_name)
    io_type = var[0]
    address = var[1]

    if (io_type == var_type.DIG_INP):
        return d_inputs.setValues(address, value)

    elif (io_type == var_type.DIG_OUT):
        return d_outputs.setValues(address, value)

    elif (io_type == var_type.ANA_INP):
        return a_inputs.setValues(address, value)

    elif (io_type == var_type.ANA_OUT):
        return a_outputs.setValues(address, value)

    else:
        return 0

def should_quit():
    if a_outputs.getValues(50)[0] == KILL_SIGNAL:
        return True
    return False

global mtcp_server

def run_server():
    global mtcp_server
    store = ModbusSlaveContext(di=d_inputs, co=d_outputs, ir=a_inputs, hr=a_outputs, zero_mode=True)
    context = ModbusServerContext(slaves=store, single=True)

    #run server
    mtcp_server = ModbusTcpServer(context, address=("localhost", 2605))
    mtcp_server.serve_forever()
    
#def monitor_server():
#    global server_running
#    while(server_running):
#        if a_outputs.getValues(50)[0] == KILL_SIGNAL:
#            self.stop()
#            server_running = False
#        time.sleep(1)
    
server_thread = threading.Thread(target=run_server)
#monitor_thread = threading.Thread(target=monitor_server)

def start():
    global server_running
    #server_running = True
    server_thread.start()
    #monitor_thread.start()

def stop():
    mtcp_server.server_close()
    mtcp_server.shutdown()
    
