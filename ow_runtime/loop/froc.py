from socket import *
import threading
import time
import os
import json

import ctypes

import resource

lib_name = "/action/handler.py"
f = 'main'
func = None

syscall = ctypes.CDLL(None).syscall

def init_func(fname):
    global func
    if (os.path.isfile(lib_name) and os.access(lib_name, os.R_OK)):
        # Function lib init
        import handler as func
        return "success"
    else:
        return "err"

port = 40509

sock = socket(AF_INET, SOCK_STREAM)
sock.connect(('127.0.0.1', port))

recvData = sock.recv(1024).decode('utf-8')
if recvData == 'init':
    err = init_func(f)
    sock.send(err.encode('utf-8'))
else:
    quit()

while True:
    param = sock.recv(1024).decode('utf-8')
    result = func.main(json.loads(param))
    result["RSS"] = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
    sendData = json.dumps(result)
    sock.send(sendData.encode('utf-8'))
