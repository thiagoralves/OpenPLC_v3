# OpenPLC Runtime version 3
OpenPLC is an open-source [Programmable Logic Controller](https://en.wikipedia.org/wiki/Programmable_logic_controller) that is based on easy to use software. Our focus is to provide a low cost industrial solution for automation and research. OpenPLC has been used in [many research papers](https://scholar.google.com/scholar?as_ylo=2014&q=openplc&hl=en&as_sdt=0,1) as a framework for industrial cyber security research, given that it is the only controller to provide the entire source code.
The OpenPLC Project consists of three sub-projects:
1. [Runtime](https://github.com/thiagoralves/OpenPLC_v3)
2. [Programming editor](http://www.openplcproject.com/downloads)
3. [HMI builder](http://www.openplcproject.com/reference-installing-scadabr)
<br/>
To install:
<br/>
git clone https://github.com/thiagoralves/OpenPLC_v3.git
<br/>
cd OpenPLC_v3
<br/>
./install.sh [platform]
<br/>
where [platform] can be
<br/>
**win** - Install OpenPLC on Windows over Cygwin
<br/>
**linux** - Install OpenPLC on a Debian-based Linux distribution
<br/>
**rpi** - Install OpenPLC on a Raspberry Pi
<br/>
**custom** - Skip all specific package installation and tries to install OpenPLC assuming your system already has all dependencies met. This option can be useful if you're trying to install OpenPLC on an unsuported Linux platform or had manually installed all the dependency packages before.
