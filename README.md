# OpenPLC Runtime version 3

[![Build Status](https://travis-ci.org/thiagoralves/OpenPLC_v3.svg?branch=master)](https://travis-ci.org/thiagoralves/OpenPLC_v3)
[![Build status](https://ci.appveyor.com/api/projects/status/ut3466ixwtyf68qg?svg=true)](https://ci.appveyor.com/project/shrmrf/openplc-v3)

OpenPLC is an open-source [Programmable Logic Controller](https://en.wikipedia.org/wiki/Programmable_logic_controller) that is based on easy to use software. Our focus is to provide a low cost industrial solution for automation and research. OpenPLC has been used in [many research papers](https://scholar.google.com/scholar?as_ylo=2014&q=openplc&hl=en&as_sdt=0,1) as a framework for industrial cyber security research, given that it is the only controller to provide the entire source code.
The OpenPLC Project consists of three sub-projects:
1. [Runtime](https://github.com/thiagoralves/OpenPLC_v3)
2. [Programming editor](http://www.openplcproject.com/plcopen-editor)
3. [HMI builder](http://www.openplcproject.com/reference-installing-scadabr)

## Installation:
```bash
git clone https://github.com/thiagoralves/OpenPLC_v3.git
cd OpenPLC_v3
./install.sh [platform]
```

Where `[platform]` can be:

`win` - Install OpenPLC on Windows over Cygwin

`linux` - Install OpenPLC on a Debian-based Linux distribution

`docker` - Used by the `Dockerfile` (i.e. doesn't invoke `sudo`)

`rpi` - Install OpenPLC on a Raspberry Pi

`custom` - Skip all specific package installation and tries to install OpenPLC assuming your system already has all dependencies met. This option can be useful if you're trying to install OpenPLC on an unsuported Linux platform or had manually installed all the dependency packages before.

### Building, Installing and Running inside Docker
Make sure [`docker` is installed](https://docs.docker.com/install/linux/docker-ce/ubuntu/)

#### Build
```
# instead of running install.sh as stated above, run:
docker build -t openplc:v3 .
```

#### RUN
_Devices can be passed to the `docker` daemon using the `-v` flag (e.g. `-v /dev/ttyACM0:/dev/ttyACM0`)_

```bash
docker run -it --rm --privileged -p 8080:8080 openplc:v3
```

