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

## Utils ------------------------------------d
def _read_file(path):
    with open(path, "r") as f:
        return f.read().strip()
    return None



















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

COMPILE_OPTS = [
    "maitec",
    "st_optimizer",
    "modbus",
    "Quit",
]


q_compile = [
    {
        'type': 'list',
        'name': 'compile',
        'message': 'Compile Menu',
        'choices': [
            "maitec",
            "st_optimizer",
            "modbus",
            "Quit",
        ]

    }
]


while True:
    answers = prompt(q_compile, style=custom_style)
    c = answers.get("compile")
    print(c)
    if c == "Quit":
        sys.exit(0)


