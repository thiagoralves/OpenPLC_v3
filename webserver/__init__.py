
import os
import sys

HERE_PATH = os.path.abspath(os.path.dirname(__file__))
ROOT_PATH = os.path.abspath(os.path.join(HERE_PATH, ".."))
ETC_PATH = os.path.abspath(os.path.join(ROOT_PATH, "etc"))

# if not ROOT_PATH in sys.path:
#     print("APPEND", ROOT_PATH)
#     sys.path.append(ROOT_PATH)
#
# if not HERE_PATH in sys.path:
#     print("APPEND", HERE_PATH)
#     sys.path.append(HERE_PATH)