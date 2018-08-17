#Use this for OpenPLC console: http://eyalarubas.com/python-subproc-nonblock.html
import subprocess
import socket
import errno
import time
from threading import Thread
from Queue import Queue, Empty

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
            Collect lines from 'stream' and put them in 'quque'.
            '''

            while True:
                line = stream.readline()
                if line:
                    queue.put(line)
                else:
                    self.end_of_stream = True
                    #raise UnexpectedEndOfStream

        self._t = Thread(target = _populateQueue,
                args = (self._s, self._q))
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

class Runtime:

    project_file = ""
    project_name = ""
    project_description = ""
    runtime_status = RStatus.STOPPED

    
    def start_runtime(self):
        if self.status() == RStatus.STOPPED:
            a = subprocess.Popen(['./core/openplc'])
            self.runtime_status = RStatus.RUNNING
    
    def stop_runtime(self):
        if self.status() == RStatus.RUNNING:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('quit()\n')
                data = s.recv(1000)
                s.close()
                self.runtime_status = RStatus.STOPPED
            except socket.error as serr:
                print("Failed to stop the runtime. Error: " + str(serr))
    
    def compile_program(self, st_file):
        if self.status() == "Running":
            self.stop_runtime()
            
        self.is_compiling = True
        global compilation_status_str
        global compilation_object
        compilation_status_str = ""
        a = subprocess.Popen(['./scripts/compile_program.sh', str(st_file)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
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
        if 'compilation_object' in globals():
            if compilation_object.end_of_stream == False:
                return RStatus.COMPILING
        
        #If it is running, make sure that it really is running
        if self.runtime_status == RStatus.RUNNING:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('exec_time()\n')
                data = s.recv(10000)
                s.close()
                self.runtime_status = RStatus.RUNNING
            except socket.error as serr:
                print("OpenPLC Runtime is not running. Error: " + str(serr))
                self.runtime_status = RStatus.STOPPED
        
        return self.runtime_status

    def is_running(self):
        return self.runtime_status == RStatus.RUNNING
    
    def start_modbus(self, port_num):
        if self.status() == RStatus.RUNNING:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('start_modbus(' + str(port_num) + ')\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
                
    def stop_modbus(self):
        if self.status() == RStatus.RUNNING:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('stop_modbus()\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")

    def start_dnp3(self, port_num):
        if self.status() == RStatus.RUNNING:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('start_dnp3(' + str(port_num) + ')\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
        
    def stop_dnp3(self):
        if self.status() == RStatus.RUNNING:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('stop_dnp3()\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
    
    def logs(self):
        if self.status() == RStatus.RUNNING:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('runtime_logs()\n')
                data = s.recv(1000000)
                s.close()
                return data
            except:
                print("Error connecting to OpenPLC runtime")
            
            return "Error connecting to OpenPLC runtime"
        else:
            return "OpenPLC Runtime is not running"
        
    def exec_time(self):
        if self.status() == RStatus.RUNNING:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('exec_time()\n')
                data = s.recv(10000)
                s.close()
                return display_time(int(data), 4)
            except:
                print("Error connecting to OpenPLC runtime")
            
            return "Error connecting to OpenPLC runtime"
        else:
            return "N/A"