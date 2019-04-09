
import os


from fabric.api import local, sudo, lcd, sudo, warn_only, prompt, shell_env


ROOT_PATH =  os.path.abspath( os.path.dirname( __file__ ))

BUILD_DIR = os.path.join(ROOT_PATH, "build")


def matiec():
    """Compile Matiec"""
    with lcd("utils/matiec_src"):
        local("autoreconf - i")
        local("./configure")
        local("make")
        local("cp. /iec2c %s/iec2c" % BUILD_DIR)

def optim():
    """Compile ST Optmimizer"""
    with lcd("utils/st_optimizer_src"):
        local("g++ st_optimizer.cpp -o %s/st_optimizer" % BUILD_DIR)
