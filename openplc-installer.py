#!/usr/bin/python
# -*- coding: utf-8 -*-


from __future__ import print_function

from pprint import pprint

import os
import sys
import platform


from PyInquirer import style_from_dict, Token, prompt, Separator

from fabric.api import local, lcd, sudo, warn_only


ROOT_PATH =  os.path.abspath( os.path.dirname( __file__ ))

BUILD_DIR = os.path.join(ROOT_PATH, "build")

## Utils ------------------------------------
def outta_here():
    sys.exit(0)


def _read_file(path):
    with open(path, "r") as f:
        return f.read().strip()
    return None


def c_maitec():
    """Compile Matiec"""
    with lcd("utils/matiec_src"):
        local("./autoreconf - i")
        local("./configure")
        local("make")
        local("cp. /iec2c %s/iec2c" % BUILD_DIR)

def c_st_optimizer():
    """Compile ST Optmimizer"""
    with lcd("utils/st_optimizer_src"):
        local("g++ st_optimizer.cpp -o %s/st_optimizer" % BUILD_DIR)

def c_modbus():
    """Compile ST Optmimizer"""
    with lcd("utils/libmodbus_src"):
        local("./autogen.sh")
        local("./configure")
        local("sudo make install")
        local("sudo ldconfig")

def c_dnp3():
    """Compile ST Optmimizer"""
    local("mkdir %s/dnp3" % BUILD_DIR)
    with lcd("%s/dnp3" % BUILD_DIR):
        local("./autogen.sh")
        local("$1 dd if=/dev/zero of=swapfile bs=1M count=1000")
        local("$1 mkswap swapfile")
        local("$1 swapon swapfile")
        local("cmake ../../utils/dnp3_src")
        local("make")
        local("sudo make insall")
        local("sudo ldconfig")
        local("sudo swapoff swapfile")
        local("sudo rm -f ./swapfile")














custom_style = style_from_dict({
    Token.Separator: '#cc5454',
    Token.QuestionMark: '#673ab7 bold',
    Token.Selected: '#cc5454',  # default
    Token.Pointer: '#673ab7 bold',
    Token.Instruction: '',  # default
    Token.Answer: '#f44336 bold',
    Token.Question: '',
})


#=======================================================================

PLATFORMS = [
    "win", "linux", "rpi", "custom"
]
PLATFORM_FILE = "%s/openplc_platform.txt" % BUILD_DIR

def get_platform():
    if not os.path.exists(PLATFORM_FILE):
        return None
    plat = _read_file(PLATFORM_FILE)
    if not plat in PLATFORMS:  # validate
        return None
    return plat

curr_platform = get_platform()

def q_platform():
    qu = [
        {
            'type': 'list',
            'name': 'platform',
            'message': 'Select Platform',
            'choices': PLATFORMS,
            'default': get_platform()

        }
    ]

    answers = prompt(qu, style=custom_style)
    return answers.get("platform")


#=======================================================================

print('=== OpenPLC Installer ====')

print("PLATROM=", curr_platform, PLATFORM_FILE, platform.platform())

if curr_platform == None:
    q_platform()

print('Platform: %s' %  curr_platform)



COMPILE_PACKS = [
    {"package": "all",          "label": "All packages", "runner": c_maitec},
    {"package": "maitec",       "label": "maitec - compile and install", "runner": c_maitec},
    {"package": "st_optimizer", "label": "ST Optimizer - compile", "runner": c_st_optimizer},
    {"package": "dnp3",         "label": "dnp3 - compile and install", "runner": c_dnp3},
    {"package": "modbus",       "label": "modbus - compile and install", "runner": c_modbus},

    {"package": "quit",         "label": "Quit", "runner": outta_here},
]
q_compile = [
    {
        'type': 'list',
        'name': 'package',
        'message': 'Compile Packages',
        'choices': [p['label'] for p in COMPILE_PACKS]

    }
]


while True:
    answers = prompt(q_compile, style=custom_style)
    pck = answers.get("package")
    print("pck=", pck)

    if pck.lower() == "quit":
        sys.exit(0)

    for p in COMPILE_PACKS:
        if p['label'] == pck:
            p['runner']()



