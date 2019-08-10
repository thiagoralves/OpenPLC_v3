import os
import time, threading
from pymodbus.client.sync import ModbusTcpClient

from . import HERE_DIR, ROOT_DIR

class debug_var():
    name = ''
    location = ''
    type = ''
    forced = 'No'
    value = 0

debug_vars = []
monitor_active = False
mb_client = None

def parse_st(st_file):
    global debug_vars
    filepath = os.path.join(ROOT_PATH, 'etc', 'st_files', st_file)
    
    st_program = open(filepath, 'r')
    
    for line in st_program.readlines():
        if line.find(' AT ') > 0:
            debug_data = debug_var()
            tmp = line.strip().split(' ')
            debug_data.name = tmp[0]
            debug_data.location = tmp[2]
            debug_data.type = tmp[4].split(';')[0]
            debug_vars.append(debug_data)
    
    for debugs in debug_vars:
        print('Name: ' + debugs.name)
        print('Location: ' + debugs.location)
        print('Type: ' + debugs.type)
        print('')


def cleanup():
    del debug_vars[:]
    
def modbus_monitor():
    global mb_client
    for debug_data in debug_vars:
        if (debug_data.location.find('IX')) > 0:
            #Reading Input Status
            mb_address = debug_data.location.split('%IX')[1].split('.')
            result = mb_client.read_discrete_inputs(int(mb_address[0])*8 + int(mb_address[1]), 1)
            debug_data.value = result.bits[0]
            
        elif (debug_data.location.find('QX')) > 0:
            #Reading Coils
            mb_address = debug_data.location.split('%QX')[1].split('.')
            result = mb_client.read_coils(int(mb_address[0])*8 + int(mb_address[1]), 1)
            debug_data.value = result.bits[0]
            
        elif (debug_data.location.find('IW')) > 0:
            #Reading Input Registers
            mb_address = debug_data.location.split('%IW')[1]
            result = mb_client.read_input_registers(int(mb_address), 1)
            debug_data.value = result.registers[0]
            
        elif (debug_data.location.find('QW')) > 0:
            #Reading Holding Registers
            mb_address = debug_data.location.split('%QW')[1]
            result = mb_client.read_holding_registers(int(mb_address), 1)
            debug_data.value = result.registers[0]
            
        elif (debug_data.location.find('MW')) > 0:
            #Reading Word Memory
            mb_address = debug_data.location.split('%MW')[1]
            result = mb_client.read_holding_registers(int(mb_address) + 1024, 1)
            debug_data.value = result.registers[0]
            
        elif (debug_data.location.find('MD')) > 0:
            #Reading Double Memory
            print('hi')
            
        elif (debug_data.location.find('ML')) > 0:
            #Reading Long Memory
            print('hi')
            
    
    if monitor_active == True:
        threading.Timer(0.5, modbus_monitor).start()
    
def start_monitor():
    global monitor_active
    global mb_client
    
    if monitor_active != True:
        monitor_active = True
        mb_client = ModbusTcpClient('127.0.0.1')
        
        modbus_monitor()

def stop_monitor():
    global monitor_active
    global mb_client
    
    if monitor_active != False:
        monitor_active = False
        mb_client.close()
