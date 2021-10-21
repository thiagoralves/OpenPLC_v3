# -----------------------------------------------------------------------------
# Copyright 2015 Thiago Alves
# This file is part of the OpenPLC Software Stack.

# OpenPLC is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# OpenPLC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with OpenPLC.  If not, see <http://www.gnu.org/licenses/>.
# ----------------------------------------------------------------------------

#Use this for OpenPLC console: http://eyalarubas.com/python-subproc-nonblock.html
import subprocess
import socket
import errno
import time
import os
from threading import Thread
from queue import Queue, Empty

self_path = os.path.dirname(__file__)

intervals = (
    ('weeks', 604800),  # 60 * 60 * 24 * 7
    ('days', 86400),    # 60 * 60 * 24
    ('hours', 3600),    # 60 * 60
    ('minutes', 60),
    ('seconds', 1),
    )

def display_time(seconds, granularity=2):
    result = []

    for name, count in intervals:
        value = seconds // count
        if value:
            seconds -= value * count
            if value == 1:
                name = name.rstrip('s')
            result.append("{} {}".format(value, name))
    return ', '.join(result[:granularity])

class NonBlockingStreamReader:

    end_of_stream = False
    
    def __init__(self, stream):
        '''
        stream: the stream to read from.
                Usually a process' stdout or stderr.
        '''

        self._s = stream
        self._q = Queue()

        def _populateQueue(stream, queue):
            '''
            Collect lines from 'stream' and put them in 'queue'.
            '''

            #while True:
            while (self.end_of_stream == False):
                line = stream.readline()
                if line:
                    queue.put(line)
                    if (line.find("Compilation finished with errors!") >= 0 or line.find("Compilation finished successfully!") >= 0):
                        self.end_of_stream = True
                else:
                    self.end_of_stream = True
                    raise UnexpectedEndOfStream

        self._t = Thread(target = _populateQueue, args = (self._s, self._q))
        self._t.daemon = True
        self._t.start() #start collecting lines from the stream

    def readline(self, timeout = None):
        try:
            return self._q.get(block = timeout is not None,
                    timeout = timeout)
        except Empty:
            return None

class UnexpectedEndOfStream(Exception): pass

class runtime:
    project_file = ""
    project_name = ""
    project_description = ""
    runtime_status = "Stopped"
    
    def start_runtime(self):
        if (self.status() == "Stopped"):
            openplc_path = os.path.abspath(os.path.join(self_path, '..', 'bin', 'openplc'))
            self.theprocess = subprocess.Popen([openplc_path])  # XXX: iPAS
            self.runtime_status = "Running"
    
    def stop_runtime(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'quit()\n')
                data = s.recv(1000)
                s.close()
                self.runtime_status = "Stopped"

                while self.theprocess.poll() is None:  # XXX: iPAS, to prevent the defunct killed process.
                    time.sleep(1)  # https://www.reddit.com/r/learnpython/comments/776r96/defunct_python_process_when_using_subprocesspopen/

            except socket.error as serr:
                print(("Failed to stop the runtime. Error: " + str(serr)))
    
    def compile_program(self, st_file):
        if (self.status() == "Running"):
            self.stop_runtime()
            
        self.is_compiling = True
        global compilation_status_str
        global compilation_object
        compilation_status_str = ""
        compile_program_path = os.path.abspath(os.path.join(self_path, '..', 'scripts', 'compile_program.sh'))
        a = subprocess.Popen([compile_program_path, str(st_file)],
                             cwd=self_path,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT,
                             encoding='UTF-8')
        compilation_object = NonBlockingStreamReader(a.stdout)
    
    def compilation_status(self):
        global compilation_status_str
        global compilation_object
        while True:
            line = compilation_object.readline()
            if not line: break
            compilation_status_str += line
        return compilation_status_str
    
    def status(self):
        if ('compilation_object' in globals()):
            if (compilation_object.end_of_stream == False):
                return "Compiling"
        
        #If it is running, make sure that it really is running
        if (self.runtime_status == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'exec_time()\n')
                data = s.recv(10000)
                s.close()
                self.runtime_status = "Running"
            except socket.error as serr:
                print(("OpenPLC Runtime is not running. Error: " + str(serr)))
                self.runtime_status = "Stopped"
        
        return self.runtime_status
    
    def start_modbus(self, port_num):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'start_modbusslave(' + (b'%d' % port_num) + b')\n')
                data = s.recv(1000)
                s.close()
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")
                
    def stop_modbus(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'stop_modbusslave()\n')
                data = s.recv(1000)
                s.close()
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")

    def start_dnp3(self, port_num):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'start_dnp3s(' + (b'%d' % port_num) + ')\n')
                data = s.recv(1000)
                s.close()
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")
        
    def stop_dnp3(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'stop_dnp3s()\n')
                data = s.recv(1000)
                s.close()
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")
                
    def start_enip(self, port_num):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'start_enip(' + (b'%d' % port_num) + b')\n')
                data = s.recv(1000)
                s.close()
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")
                
    def stop_enip(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'stop_enip()\n')
                data = s.recv(1000)
                s.close()
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")
    
    def start_pstorage(self, poll_rate):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'start_pstorage(' + (b'%d' % poll_rate) + ')\n')
                data = s.recv(1000)
                s.close()
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")
                
    def stop_pstorage(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'stop_pstorage()\n')
                data = s.recv(1000)
                s.close()
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")
    
    def logs(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'runtime_logs()\n')
                data = s.recv(1000000)
                s.close()
                return data
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")
            
            return "Error connecting to OpenPLC runtime"
        else:
            return "OpenPLC Runtime is not running"
        
    def exec_time(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send(b'exec_time()\n')
                data = s.recv(10000)
                s.close()
                return display_time(int(data), 4)
            except ConnectionRefusedError:
                print("Error connecting to OpenPLC runtime")
            
            return "Error connecting to OpenPLC runtime"
        else:
            return "N/A"
