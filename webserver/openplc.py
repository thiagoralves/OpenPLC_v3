#Use this for OpenPLC console: http://eyalarubas.com/python-subproc-nonblock.html
import subprocess
import socket
import errno
import time
from threading import Thread
from queue import Queue, Empty
import os.path

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
                line = stream.readline().decode('utf-8')
                if line:
                    queue.put(line)
                    if "Compilation finished with errors!" in line or "Compilation finished successfully!" in line:
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

    def _rpc(self, msg, timeout=1000):
        data = ""
        if not self.runtime_status == "Running":
            return data
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect(('localhost', 43628))
            s.send(f'{msg}\n'.encode('utf-8'))
            data = s.recv(timeout).decode('utf-8')
            s.close()
            self.runtime_status = "Running"
        except socket.error as serr:
            print(f'Socket error during {msg}, is the runtime active?')
            self.runtime_status = "Stopped"
        return data

    def stop_runtime(self):
        if (self.status() == "Running"):
            self._rpc(f'quit()')
            self.runtime_status = "Stopped"

            while self.theprocess.poll() is None:  # XXX: iPAS, to prevent the defunct killed process.
                time.sleep(1)  # https://www.reddit.com/r/learnpython/comments/776r96/defunct_python_process_when_using_subprocesspopen/
    
    def compile_program(self, st_file):
        if (self.status() == "Running"):
            self.stop_runtime()
        
        self.is_compiling = True
        global compilation_status_str
        global compilation_object
        compilation_status_str = ""
        
        # Extract debug information from program
        f = open('./st_files/' + st_file, "r")
        combined_lines = f.read()
        f.close()
        combined_lines = combined_lines.split('\n')
        program_lines = []
        c_debug_lines = []

        for line in combined_lines:
            if line.startswith('(*DBG:') and line.endswith('*)'):
                c_debug_lines.append(line[6:-2])
            else:
                program_lines.append(line)

        if len(c_debug_lines) == 0:
            # Could not find debug info on program uploaded
            if os.path.isfile('./st_files/' + st_file + '.dbg'):
                # Debugger info exists on file - open it
                f = open('./st_files/' + st_file + '.dbg', "r")
                c_debug = f.read()
                f.close()

                # Write c_debug file
                f = open('./core/debug.cpp', "w")
                f.write(c_debug)
                f.close()

                # Start compilation
                a = subprocess.Popen(['./scripts/compile_program.sh', str(st_file)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                compilation_object = NonBlockingStreamReader(a.stdout)
            else:
                # No debug info... probably a program generated from the old editor
                compilation_status_str += 'Invalid program file format. Please update your OpenPLC Editor and try again\n'
                a = subprocess.Popen(['echo', 'Compilation finished with errors!'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                compilation_object = NonBlockingStreamReader(a.stdout)
        else:
            # Debug info was extracted from program
            program = '\n'.join(program_lines)
            c_debug = '\n'.join(c_debug_lines)

            # Write c_debug file
            f = open('./core/debug.cpp', "w")
            f.write(c_debug)
            f.close()

            #Write program and debug files
            f = open('./st_files/' + st_file, "w")
            f.write(program)
            f.close()
            f = open('./st_files/' + st_file + '.dbg', "w")
            f.write(c_debug)
            f.close()

            # Start compilation
            a = subprocess.Popen(['./scripts/compile_program.sh', str(st_file)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            compilation_object = NonBlockingStreamReader(a.stdout)
    
    def compilation_status(self):
        global compilation_status_str
        global compilation_object
        while compilation_object != None:
            line = compilation_object.readline()
            if not line: break
            compilation_status_str += line
        return compilation_status_str

    def status(self):
        if ('compilation_object' in globals()):
            if (compilation_object.end_of_stream == False):
                return "Compiling"

        if not self._rpc('exec_time()', 10000):
            self.runtime_status = "Stopped"

        return self.runtime_status

    def start_modbus(self, port_num):
        return self._rpc(f'start_modbus({port_num})')

    def stop_modbus(self):
        return self._rpc(f'stop_modbus()')

    def start_dnp3(self, port_num):
        return self._rpc(f'start_dnp3({port_num})')
        
    def stop_dnp3(self):
        return self._rpc(f'stop_dnp3()')
                
    def start_enip(self, port_num):
        return self._rpc(f'start_enip({port_num})')

    def stop_enip(self):
        return self._rpc(f'stop_enip()')
    
    def start_pstorage(self, poll_rate):
        return self._rpc(f'start_pstorage({poll_rate})')
                
    def stop_pstorage(self):
        return self._rpc(f'stop_pstorage()')
    
    def logs(self):
        return self._rpc(f'runtime_logs()',1000000)
        
    def exec_time(self):
        return self._rpc(f'exec_time()',10000) or "N/A"
