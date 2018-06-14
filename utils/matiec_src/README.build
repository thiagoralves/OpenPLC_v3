
Compile/Build 
=============

1) Compiling under Linux
------------------------
$ autoreconf -i
$ ./configure
$ make


2) Cross-Compiling under Linux, for Windows
-------------------------------------------
$ ./configure  --host=i586-pc-mingw32
(or, to use static linking, which does not require installing any mingw dll's on windows)
$ ./configure  --host=i586-pc-mingw32 LDFLAGS="-static"
$ make





Maintaining the Build Environment
=================================
-> Add new files to Makefile.am or add a new makefile
$ autoreconf


-> Prepare clean project
$ make distclean



-> Remember to add these files to your .hgignore
	Makefile
	config.*
	*.a
	.deps


-> Send me TODO list to complete build system.
	matteo.facchinetti@sirius-es.it

