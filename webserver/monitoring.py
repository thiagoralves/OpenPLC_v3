import os
import time, threading
from pymodbus.client.sync import ModbusTcpClient

from . import HERE_DIR, ROOT_DIR, ETC_DIR

class debug_var():
    name = ''
    location = ''
    type = ''
    forced = 'No'
    value = 0


def parse_st(st_file):
    global debug_vars
    filepath = os.path.join(ETC_DIR,  'st_files', st_file)
    
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





class Monitor:

    def __init__(self, websock):

        self.websock = websock
        self.debug_vars = []
        self.mb_client = None
        self.monitor_active = False
    
    def modbus_monitor(self):

        for debug_data in self.debug_vars:
            if (debug_data.location.find('IX')) > 0:
                #Reading Input Status
                mb_address = debug_data.location.split('%IX')[1].split('.')
                result = self.mb_client.read_discrete_inputs(int(mb_address[0])*8 + int(mb_address[1]), 1)
                debug_data.value = result.bits[0]

            elif (debug_data.location.find('QX')) > 0:
                #Reading Coils
                mb_address = debug_data.location.split('%QX')[1].split('.')
                result = self.mb_client.read_coils(int(mb_address[0])*8 + int(mb_address[1]), 1)
                debug_data.value = result.bits[0]

            elif (debug_data.location.find('IW')) > 0:
                #Reading Input Registers
                mb_address = debug_data.location.split('%IW')[1]
                result = self.mb_client.read_input_registers(int(mb_address), 1)
                debug_data.value = result.registers[0]

            elif (debug_data.location.find('QW')) > 0:
                #Reading Holding Registers
                mb_address = debug_data.location.split('%QW')[1]
                result = self.mb_client.read_holding_registers(int(mb_address), 1)
                debug_data.value = result.registers[0]

            elif (debug_data.location.find('MW')) > 0:
                #Reading Word Memory
                mb_address = debug_data.location.split('%MW')[1]
                result = self.mb_client.read_holding_registers(int(mb_address) + 1024, 1)
                debug_data.value = result.registers[0]

            elif (debug_data.location.find('MD')) > 0:
                #Reading Double Memory
                print('hi')

            elif (debug_data.location.find('ML')) > 0:
                #Reading Long Memory
                print('hi')


        if self.monitor_active == True:
            threading.Timer(0.5, self.modbus_monitor).start()
    
    def start_monitor(self):
        if self.monitor_active != True:
            self.monitor_active = True
            self.mb_client = ModbusTcpClient('127.0.0.1')
            self.modbus_monitor()

    def stop_monitor(self):
        if self.monitor_active != False:
            self.monitor_active = False
            self.mb_client.close()

    def cleanup(self):
        del self.debug_vars[:]