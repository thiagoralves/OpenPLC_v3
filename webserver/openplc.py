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
            self.theprocess = subprocess.Popen(['./core/openplc'])  # XXX: iPAS
            self.runtime_status = "Running"
    
    def stop_runtime(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('quit()\n')
                data = s.recv(1000)
                s.close()
                self.runtime_status = "Stopped"

                while self.theprocess.poll() is None:  # XXX: iPAS, to prevent the defunct killed process.
                    time.sleep(1)  # https://www.reddit.com/r/learnpython/comments/776r96/defunct_python_process_when_using_subprocesspopen/
                    
            except socket.error as serr:
                print("Failed to stop the runtime. Error: " + str(serr))
    
    def compile_program(self, st_file):
        if (self.status() == "Running"):
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
        if ('compilation_object' in globals()):
            if (compilation_object.end_of_stream == False):
                return "Compiling"
        
        #If it is running, make sure that it really is running
        if (self.runtime_status == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('exec_time()\n')
                data = s.recv(10000)
                s.close()
                self.runtime_status = "Running"
            except socket.error as serr:
                print("OpenPLC Runtime is not running. Error: " + str(serr))
                self.runtime_status = "Stopped"
        
        return self.runtime_status
    
    def start_modbus(self, port_num):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('start_modbus(' + str(port_num) + ')\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
                
    def stop_modbus(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('stop_modbus()\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")

    def start_dnp3(self, port_num):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('start_dnp3(' + str(port_num) + ')\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
        
    def stop_dnp3(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('stop_dnp3()\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
                
    def start_enip(self, port_num):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('start_enip(' + str(port_num) + ')\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
                
    def stop_enip(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('stop_enip()\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
    
    def start_pstorage(self, poll_rate):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('start_pstorage(' + str(poll_rate) + ')\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
                
    def stop_pstorage(self):
        if (self.status() == "Running"):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(('localhost', 43628))
                s.send('stop_pstorage()\n')
                data = s.recv(1000)
                s.close()
            except:
                print("Error connecting to OpenPLC runtime")
    
    def logs(self):
        if (self.status() == "Running"):
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
        if (self.status() == "Running"):
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
