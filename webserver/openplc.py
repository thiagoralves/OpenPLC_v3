#Use this for OpenPLC console: http://eyalarubas.com/python-subproc-nonblock.html
import subprocess
import socket
import errno
import time
from threading import Thread, Lock
from queue import Queue, Empty
import os
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
    def __init__(self):
        self.project_file = ""
        self.project_name = ""
        self.project_description = ""
        self.compilation_status_str = ""
        self.compilation_error_str = ""
        self.compilation_object = None
        self.compilation_error = None
        self.runtime_status = "Stopped"

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
        self.compilation_status_str = ""
        
        # Extract debug information from program
        with open('./st_files/' + st_file, "r") as f:
            combined_lines = f.read()

        combined_lines = combined_lines.split('\n')
        program_lines = []
        c_debug_lines = []
        file_lines = {}

        for line in combined_lines:
            if line.startswith('(*DBG:') and line.endswith('*)'):
                c_debug_lines.append(line[6:-2])
            elif line.startswith('(*FILE:c_blocks_code.cpp') and 'extern "C" void' in line:
                # This is a hack to backport runtime v4 C/C++ functionality to v3. The v3 runtime needs to
                # exclude all extern "C" declarations from the c_blocks_code.cpp file. I know this is not
                # pretty, but v3 architecture is not pretty, so we are doing this here so that v4 code
                # can remain pretty.
                pass
            elif line.startswith('(*FILE:') and line.endswith('*)'):
                file_content = line[7:-2].strip()
                if ' ' in file_content:
                    file_path, file_line = file_content.split(' ', 1)
                    if file_path not in file_lines:
                        file_lines[file_path] = []
                    file_lines[file_path].append(file_line)
            else:
                program_lines.append(line)

        if len(c_debug_lines) == 0:
            c_debug = ''
            # Could not find debug info on program uploaded
            if os.path.isfile('./st_files/' + st_file + '.dbg'):
                # Debugger info exists on file - open it
                with open('./st_files/' + st_file + '.dbg', "r") as f:
                    c_debug = f.read()

            else:
                # No debug info... probably a program generated from the old editor. Use the blank debug info just to compile the program
                with open('./core/debug.blank', "r") as f:
                    c_debug = f.read()
                    f.close()

            # Write c_debug file
            with open('./core/debug.cpp', "w") as f:
                f.write(c_debug)

            for file_path, lines in file_lines.items():
                full_path = os.path.join('./core', file_path)
                os.makedirs(os.path.dirname(full_path), exist_ok=True)
                with open(full_path, "w") as f:
                    f.write('\n'.join(lines))

            # Start compilation
            try:
                a = subprocess.Popen(['./scripts/compile_program.sh', str(st_file)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                self.compilation_object = NonBlockingStreamReader(a.stdout)
                # self.compilation_error = NonBlockingStreamReader(a.stderr)
            except Exception as e:
                print(f"Error starting compilation: {e}")
        else:
            # Debug info was extracted from program
            program = '\n'.join(program_lines)
            c_debug = '\n'.join(c_debug_lines)

            # Write c_debug file
            with open('./core/debug.cpp', "w") as f:
                f.write(c_debug)

            for file_path, lines in file_lines.items():
                full_path = os.path.join('./core', file_path)
                os.makedirs(os.path.dirname(full_path), exist_ok=True)
                with open(full_path, "w") as f:
                    f.write('\n'.join(lines))

            #Write program and debug files
            with open('./st_files/' + st_file, "w") as f:
                f.write(program)

            with open('./st_files/' + st_file + '.dbg', "w") as f:
                f.write(c_debug)

            # Start compilation
            a = subprocess.Popen(['./scripts/compile_program.sh', str(st_file)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            self.compilation_object = NonBlockingStreamReader(a.stdout)
            # self.compilation_error = NonBlockingStreamReader(a.stderr)
    
    def compilation_status(self):
        while self.compilation_object != None:
            line = self.compilation_object.readline()
            if not line: break
            self.compilation_status_str += line
        return self.compilation_status_str

    def get_compilation_error(self):
        while self.compilation_error != None:
            line = self.compilation_error.readline()
            if not line: break
            self.compilation_error_str += line
        return self.compilation_error_str

    def status(self):
        try:
            if (self.compilation_object != None):
                if (self.compilation_object.end_of_stream == False):
                    return "Compiling"
        except Exception as e:
            print(f"Error checking compilation status: {e}")

        if not self._rpc('exec_time()', 10000):
            self.runtime_status = "Stopped"

        return self.runtime_status

    def start_modbus(self, port_num):
        return self._rpc(f'start_modbus({port_num})')

    def stop_modbus(self):
        return self._rpc(f'stop_modbus()')

    def start_snap7(self):
        return self._rpc(f'start_snap7()')

    def stop_snap7(self):
        return self._rpc(f'stop_snap7()')

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
