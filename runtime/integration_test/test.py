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