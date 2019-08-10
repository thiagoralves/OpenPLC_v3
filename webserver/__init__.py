
import os

HERE_PATH = os.path.abspath(os.path.dirname(__file__))
ROOT_PATH = os.path.abspath(os.path.join(HERE_PATH, ".."))
ETC_PATH = os.path.abspath(os.path.join(ROOT_PATH, "etc"))
SCRIPTS_PATH = os.path.abspath(os.path.join(ROOT_PATH, "scripts"))

CURR_PLATFORM_FILE = os.path.abspath(os.path.join(ETC_PATH, 'openplc_platform'))
CURR_DRIVER_FILE = os.path.abspath(os.path.join(ETC_PATH, 'openplc_driver'))
CURR_PROGRAM_FILE = os.path.abspath(os.path.join(ETC_PATH, 'active_program'))