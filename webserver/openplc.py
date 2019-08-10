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
from __future__ import absolute_import

# Use this for OpenPLC console: http://eyalarubas.com/python-subproc-nonblock.html
import subprocess
import socket
import errno
import time
import os
from threading import Thread
from Queue import Queue, Empty


from . import HERE_DIR, ROOT_DIR, SCRIPTS_DIR

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
    
    def __init__(self, stream, websock):
        '''
        stream: the stream to read from.
                Usually a process' stdout or stderr.
        '''
        self.websock = websock
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
                    self.websock.emit("xmessage", {"data": line})
                    if (line.find("Compilation finished with errors!") >= 0 or line.find("Compilation finished successfully!") >= 0):
                        self.websock.emit("xmessage", {"done": "1"})
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

class RStatus:
    STOPPED = "Stopped"
    RUNNING = "Running"
    COMPILING = "Compiling"
    UNKNOWN = "Unknown"

class Runtime:

    host = "localhost"
    port = 43628

    def __init__(self, websock):

        self.websock = websock

        self.program = None
        self.project_file = ""
        #self.project_name = ""
        #self.project_description = ""
        self.runtime_status = RStatus.STOPPED

        self.compileStreamReader = None


    def send_cmd(self, cmd, recv_bytes=1000):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.host, self.port))
            s.send('%s\n' % cmd)
            data = s.recv(recv_bytes)
            s.close()
            return data, None
        except socket.error as serr:
            e = "OpenPLC Runtime is not running. Error: " + str(serr)
            return None, e
        except Exception as e:
            e = "Error connecting to OpenPLC runtime"
            return None, e

    def is_running(self):
        return self.runtime_status == RStatus.RUNNING

    def is_compiling(self):
        #return True
        return self.status() == RStatus.COMPILING

    # -- Runtime --------------------------------------------------------
    def start_runtime(self):
        if self.status() == RStatus.STOPPED:
            openplc_path = os.path.abspath(os.path.join(ROOT_DIR, 'bin', 'openplc'))
            self.theprocess = subprocess.Popen([openplc_path])  # XXX: iPAS
            self.runtime_status = RStatus.RUNNING
    
    def stop_runtime(self):
        if self.is_running():
            data, err = self.send_cmd("quit()")
            if err:
                print(err)
                return
            self.runtime_status = RStatus.STOPPED

            while self.theprocess.poll() is None:  # XXX: iPAS, to prevent the defunct killed process.
                time.sleep(1) # https://www.reddit.com/r/learnpython/comments/776r96/defunct_python_process_when_using_subprocesspopen/

    # -- Compile --------------------------------------------------------
    def compile_program(self, st_file_name):
        if self.is_running():
            self.stop_runtime()
            
        self.is_compiling = True
        global compilation_status_str
        global compilation_object
        compilation_status_str = ""
        compile_program_script = os.path.abspath(os.path.join(SCRIPTS_DIR, 'compile_program.sh'))
        print("ST_FILE====", st_file_name)
        a = subprocess.Popen([compile_program_script, str(st_file_name)],
                             cwd=HERE_DIR,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        self.compileStreamReader = NonBlockingStreamReader(a.stdout, self.websock)
    
    def compilation_status(self):
        global compilation_status_str
        global compilation_object
        while True:
            line = self.compileStreamReader.readline()
            if not line: break
            compilation_status_str += line
        return compilation_status_str
    
    def status(self):
        if 'compilation_object' in globals():
            if compilation_object.end_of_stream == False:
                return RStatus.COMPILING
        
        # If it is running, make sure that it really is running
        if self.runtime_status == RStatus.RUNNING:
            data, err = self.send_cmd("exec_time()")
            if err:
                print(err)
                self.runtime_status = RStatus.STOPPED
                return
            self.runtime_status = RStatus.RUNNING

        return self.runtime_status

    # -- modbus --------------------------------------------------------
    def start_modbus(self, port_num):
        if self.is_running():
            data, err = self.send_cmd('start_modbus(%s)' % port_num)
            if err:
                print(err)

    def stop_modbus(self):
        if self.is_running():
            data, err = self.send_cmd('stop_modbus()')
            if err:
                print(err)

    # -- dnp3 --------------------------------------------------------
    def start_dnp3(self, port_num):
        if self.is_running():
            data, err = self.send_cmd('start_dnp3(%s)' % port_num)
            if err:
                print(err)
        
    def stop_dnp3(self):
        if self.is_running():
            data, err = self.send_cmd('stop_dnp3()')
            if err:
                print(err)

    # -- enip --------------------------------------------------------
    def start_enip(self, port_num):
        if self.is_running():
            data, err = self.send_cmd('start_enip(%s)' % port_num)
            if err:
                print(err)
                
    def stop_enip(self):
        if self.is_running():
            data, err = self.send_cmd('stop_enip()')
            if err:
                print(err)

    # -- storage --------------------------------------------------------
    def start_pstorage(self, poll_rate):
        if self.is_running():
            data, err = self.send_cmd('start_pstorage(%s)' % poll_rate)
            if err:
                print(err)
                
    def stop_pstorage(self):
        if self.is_running():
            data, err = self.send_cmd('stop_pstorage()')
            if err:
                print(err)

    # -- utils --------------------------------------------------------
    def logs(self):
        if self.is_running():
            data, err = self.send_cmd('runtime_logs()', 1000000)
            if err is not None:
                return data

            return err

        
    def exec_time(self):
        if self.is_running():
            data, err = self.send_cmd('exec_time()', 10000)
            if err is not None:
                return display_time(int(data), 4)
            print(err)
            return err
        return "N/A"
