This version includes OPC UA support for windows only. 

## Installation:
```bash
git clone https://github.com/thiagoralves/OpenPLC_v3.git
cd OpenPLC_v3
./install.sh win
```

The code is built based on https://github.com/guqinchen/OpenPLC_v3 for OPC UA 
and https://github.com/thiagoralves/OpenPLC_v3 for OpenPLC Runtime on 3/30/2023

opcua.cpp is added to the core file and started on a thread in the interactive server.


