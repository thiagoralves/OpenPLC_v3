
import os
import sys

PY3 = sys.version_info[0] == 3

HERE_DIR = os.path.abspath(os.path.dirname(__file__))
ROOT_DIR = os.path.abspath(os.path.join(HERE_DIR, ".."))

BIN_DIR = os.path.abspath(os.path.join(ROOT_DIR, "bin"))
ETC_DIR = os.path.abspath(os.path.join(ROOT_DIR, "etc"))
SCRIPTS_DIR = os.path.abspath(os.path.join(ROOT_DIR, "scripts"))

CURR_PLATFORM_FILE = os.path.abspath(os.path.join(ETC_DIR, 'openplc_platform'))
CURR_DRIVER_FILE = os.path.abspath(os.path.join(ETC_DIR, 'openplc_driver'))
CURR_PROGRAM_FILE = os.path.abspath(os.path.join(ETC_DIR, 'active_program'))

