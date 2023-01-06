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
    #debug("Into the init_func()...\n")
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

#fds = os.popen("ls -al /proc/"+str(os.getpid())+"/fd >> /root/ow_test/prev").read()
#syscall(551)
while True:
    param = sock.recv(1024).decode('utf-8')

    #rss = os.popen("cat /proc/"+str(os.getpid())+"/status | grep RSS").read()
    result = func.main(json.loads(param))
    #result["RSS-0"] = rss
    #result["RSS"] = os.popen("cat /proc/"+str(os.getpid())+"/status | grep RSS").read()
    result["RSS"] = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
    #dump = os.popen("ls -al /proc/"+str(os.getpid())+"/fd >> /root/ow_test/then").read()
    #time.sleep(2)
    sendData = json.dumps(result)
    sock.send(sendData.encode('utf-8'))
    #syscall(551)
