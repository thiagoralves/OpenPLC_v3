# Copyright 2019 Garret Fick

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http ://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissionsand
# limitations under the License.
"""
This test module defines a series of integration tests.
These tests compile the runtime with a specified ST input
and then validate some aspect of the runtime. In generate, these
are focused on the protocol implementation.
"""

import os
import socket
import subprocess
import time
import unittest
from pymodbus.client.sync import ModbusTcpClient as ModbusClient

here = os.path.abspath(os.path.dirname(__file__))
build_dir = os.path.abspath(os.path.join("..", "..", "build"))
bin_dir = os.path.abspath(os.path.join("..", "..", "bin"))

class TestProtocols(unittest.TestCase):

    def setUp(self):
        self.process = None
        self.socket = None
        self.client = None

    def tearDown(self):
        if self.process is not None:
            self.process.kill()
            self.process.wait()
        if self.socket is not None:
            self.socket.close()
        if self.client is not None:
            self.client.close()

    def compile_and_run(self, st_filename, config_filename):
        """
        Compile the runtime with the specified ST input, then start.
        
        This gives a simple way to start the runtime for testing. We expect
        that the files are in the same directory as this test.
        """

        st_filepath = os.path.join(here, st_filename)
        config_filepath = os.path.join(here, config_filename)

        if not os.path.exists(build_dir):
            os.mkdir(build_dir)

        # Generate the environment
        cmake_cmd = [
            "cmake",
            "..",
            "-DUSER_STSOURCE_DIR=" + os.path.dirname(st_filepath),
            "-Dprogram_name=" + os.path.basename(st_filepath)
        ]
        subprocess.run(cmake_cmd, cwd=build_dir, check=True)

        # Build the files
        subprocess.run(["make"], cwd=build_dir)

        # Start the runtime
        self.process = subprocess.Popen(
            [os.path.join(bin_dir, "openplc"), "--config", config_filepath],
            # We run with this as our current working directory because
            # We expect that the runtime should work correctly irrespective
            # of the current directory we had at start
            cwd=here
        )

        return self.process

    def compile_and_run_modbus(self, st_filename):
        self.compile_and_run(st_filename, "modbusslave.ini")
        time.sleep(2)

        # Create a master that will connect to the modbus slave. We use
        # port 2502 so that we don't need to run as root
        self.client = ModbusClient('localhost', port=2502)
        return self.client

    def test_modbus_slave_singlecoil(self):
        client = self.compile_and_run_modbus("modbusslave_singlecoil.st")
        unit=0x01

        # Set the coil to true
        resp = client.write_coil(0, True, unit=unit)
        self.assertEqual(1, resp.value)
        time.sleep(1)

        # Validate that the coil now reports true
        rr = client.read_coils(0, 1, unit=unit)
        bit_value = rr.getBit(0)
        self.assertTrue(bit_value)

        # Write the value false to the coil
        client.write_coil(0, False, unit=unit)
        #self.assertEqual(0, resp.value)
        time.sleep(1)

        # Validate that the coil now reports false
        rr = client.read_coils(0, 1, unit=unit)
        bit_value = rr.getBit(0)
        self.assertFalse(bit_value)

    def disabled_test_modbus_slave_multiplecoils(self):
        client = self.compile_and_run_modbus("modbusslave_multiplecoils.st")
        unit=0x01

        # Set the coil to true
        rq = client.write_coils(0, [False]*8, unit=unit)
        self.assertTrue(rq.function_code < 0x80)
        time.sleep(2)

        # Validate that the coil now reports true
        rr = client.read_coils(0, 8, unit=unit)
        print(rr)
        self.assertEqual(rr.bits, [False]*8)

    def test_modbus_slave_single_discrete_input_true(self):
        client = self.compile_and_run_modbus("modbusslave_discreteinput_true.st")
        unit=0x01

        # Set the coil to true
        resp = client.read_discrete_inputs(0, 1, unit=unit)
        bit_value = resp.getBit(0)
        self.assertTrue(bit_value)
    
    def test_modbus_slave_single_discrete_input_false(self):
        client = self.compile_and_run_modbus("modbusslave_discreteinput_false.st")
        unit=0x01

        # Set the coil to true
        resp = client.read_discrete_inputs(0, 1, unit=unit)
        bit_value = resp.getBit(0)
        self.assertFalse(bit_value)

    def test_modbus_slave_holding_registers_qw(self):
        client = self.compile_and_run_modbus("modbusslave_holdingregister_qw.st")
        unit=0x01

        # Set the holding register value - this is function code 6
        resp = client.write_register(0, 10, unit=unit)
        time.sleep(1)

        # Validate that the coil now reports true
        rr = client.read_holding_registers(0, 1, unit=unit)
        reg_value = rr.registers[0]
        self.assertEqual(10, reg_value)

    def test_modbus_slave_holding_registers_mw(self):
        client = self.compile_and_run_modbus("modbusslave_holdingregister_mw.st")
        unit=0x01

        # Set the holding register value - this is function code 6
        resp = client.write_register(1024, 10, unit=unit)
        time.sleep(1)

        # Validate that the coil now reports true
        rr = client.read_holding_registers(1024, 1, unit=unit)
        reg_value = rr.registers[0]
        self.assertEqual(10, reg_value)

    def test_modbus_slave_holding_registers_md_highbytes(self):
        client = self.compile_and_run_modbus("modbusslave_holdingregister_md.st")
        unit=0x01

        # Set the holding register value - this is function code 6
        resp = client.write_register(2048, 10, unit=unit)
        time.sleep(1)

        # Validate that the coil now reports true
        rr = client.read_holding_registers(2048, 1, unit=unit)
        reg_value = rr.registers[0]
        self.assertEqual(10, reg_value)

    def test_modbus_slave_holding_registers_md_lowbytes(self):
        client = self.compile_and_run_modbus("modbusslave_holdingregister_md.st")
        unit=0x01

        # Set the holding register value - this is function code 6
        resp = client.write_register(2049, 10, unit=unit)
        time.sleep(1)

        # Validate that the coil now reports true
        rr = client.read_holding_registers(2049, 1, unit=unit)
        reg_value = rr.registers[0]
        self.assertEqual(10, reg_value)

    def test_modbus_slave_holding_registers_ml_byte1(self):
        client = self.compile_and_run_modbus("modbusslave_holdingregister_ml.st")
        unit=0x01

        # Set the holding register value - this is function code 6
        resp = client.write_register(4096, 10, unit=unit)
        time.sleep(1)

        # Validate that the coil now reports true
        rr = client.read_holding_registers(4096, 1, unit=unit)
        reg_value = rr.registers[0]
        self.assertEqual(10, reg_value)

    def test_modbus_slave_holding_registers_ml_byte2(self):
        client = self.compile_and_run_modbus("modbusslave_holdingregister_ml.st")
        unit=0x01

        # Set the holding register value - this is function code 6
        resp = client.write_register(4097, 10, unit=unit)
        time.sleep(1)

        # Validate that the coil now reports true
        rr = client.read_holding_registers(4097, 1, unit=unit)
        reg_value = rr.registers[0]
        self.assertEqual(10, reg_value)

    def test_modbus_slave_holding_registers_ml_byte3(self):
        client = self.compile_and_run_modbus("modbusslave_holdingregister_ml.st")
        unit=0x01

        # Set the holding register value - this is function code 6
        resp = client.write_register(4098, 10, unit=unit)
        time.sleep(1)

        # Validate that the coil now reports true
        rr = client.read_holding_registers(4098, 1, unit=unit)
        reg_value = rr.registers[0]
        self.assertEqual(10, reg_value)

    def test_modbus_slave_holding_registers_ml_byte4(self):
        client = self.compile_and_run_modbus("modbusslave_holdingregister_ml.st")
        unit=0x01

        # Set the holding register value - this is function code 6
        resp = client.write_register(4099, 10, unit=unit)
        time.sleep(1)

        # Validate that the coil now reports true
        rr = client.read_holding_registers(4099, 1, unit=unit)
        reg_value = rr.registers[0]
        self.assertEqual(10, reg_value)

    def socket_connect_and_run(self, command):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("127.0.0.1", 43628))
        self.socket.sendall(str.encode(command))
        data = self.socket.recv(1000)
        return data.decode()

    def atest_interactiveserver_runtimelogs(self):
        proc = self.compile_and_run("socketserver.st", "socketserver.ini")

        data = self.socket_connect_and_run("runtime_logs()\n")
        # We should be able to get the logs and the logs should
        # have this message in it
        self.assertTrue("OpenPLC Runtime starting..." in data)

    def atest_interactiveserver_exectime(self):
        proc = self.compile_and_run("socketserver.st", "socketserver.ini")

        data = self.socket_connect_and_run("exec_time()\n")

        # The execution time should be greater than 0, but by how much
        # we cannot tell
        val = int(data)
        self.assertTrue(val > 0)

    def atest_interactiveserver_quit(self):
        proc = self.compile_and_run("socketserver.st", "socketserver.ini")

        data = self.socket_connect_and_run("quit()\n")

        self.assertTrue("OK" in data)


if __name__ == '__main__':
    unittest.main()
