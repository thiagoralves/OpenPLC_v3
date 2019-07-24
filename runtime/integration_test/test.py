# Copyright 2019 Smarter Grid Solutions

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http ://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissionsand
# limitations under the License.


import socket
import unittest

class IntegrationTest(unittest.TestCase):

    def test_connect_and_get_logs(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('127.0.0.1', 43628))
        s.sendall('runtime_logs()\n')
        data = s.recv(1)
        print(data)
        s.close()

if __name__ == '__main__':
    unittest.main()