import ctypes
#from numba import cuda

from socket import *
import threading
import time
import os
import json

import resource

lib_name = "/action/handler.py"
f = 'main'
func = None
syscall = ctypes.CDLL(None).syscall
myname = os.popen("cat /etc/hostname").read().split('\n')[0]
CHK="checkpoint"
REW="rewind"

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

file_sock = socket(AF_INET, SOCK_STREAM)
file_sock.connect(('172.17.0.1', 40510))
file_sock.send(myname.encode('utf-8'))

recvData = sock.recv(1024).decode('utf-8')
if recvData == 'init':
    err = init_func(f)
    sock.send(err.encode('utf-8'))
else:
    quit()

file_sock.send(CHK.encode('utf-8'))

#fds = os.popen("ls -al /proc/"+str(os.getpid())+"/fd >> /root/ow_test/prev").read()

while True:
    syscall(548, 1)
    #syscall(551)

    param = sock.recv(1024).decode('utf-8')

    #rss = os.popen("cat /proc/"+str(os.getpid())+"/status | grep RSS").read()
    result = func.main(json.loads(param))
    #result["RSS-1"] = rss
    #result["RSS-2"] = os.popen("cat /proc/"+str(os.getpid())+"/status | grep RSS").read()
    result["RSS"] = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
    #cnt = cnt+1

    #result["fd"] = fds
    #os.close(5)
    #os.close(6)
    #dump = os.popen("ls -al /proc/"+str(os.getpid())+"/fd >> /root/ow_test/then").read()
    #time.sleep(2)
    sendData = json.dumps(result)
    sock.send(sendData.encode('utf-8'))
    
    file_sock.send(REW.encode('utf-8'))
    syscall(549, 2)
