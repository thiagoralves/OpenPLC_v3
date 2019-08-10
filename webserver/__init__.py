
import os

HERE_DIR = os.path.abspath(os.path.dirname(__file__))
ROOT_DIR = os.path.abspath(os.path.join(HERE_DIR, ".."))
ETC_DIR = os.path.abspath(os.path.join(ROOT_DIR, "etc"))
SCRIPTS_DIR = os.path.abspath(os.path.join(ROOT_DIR, "scripts"))

CURR_PLATFORM_FILE = os.path.abspath(os.path.join(ETC_DIR, 'openplc_platform'))
CURR_DRIVER_FILE = os.path.abspath(os.path.join(ETC_DIR, 'openplc_driver'))
CURR_PROGRAM_FILE = os.path.abspath(os.path.join(ETC_DIR, 'active_program'))