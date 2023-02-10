'''
##### File system rewinder  #####
0. Will be modified
- Change below functions to some object's method
- Get hash key when object constructed
- Create lower links when obeject constructed

1. Function description
1-1. checkpointing
- Simply copy current 'diff' directory to 'chk' directory for restore
- Using shutil.copy2 (shutil.copytree's copying method) to keep metadata of files
1-2. restoration
- First, check 'diff' and 'chk' only -> find the modification
- If not exist files of 'chk' in 'diff', traverse the lower link
- If find at lower link (lower image's diff), restore to the nearest lower link's file
- If not found, remove the file

2. PATH description
2-1. Merged
- Mount point of current container
- Can modify at host system
2-2. Diff
- Different point compare to lower images
- Cannot modify and automatically updated by storage driver (maybe)
2-3. CHK
- Checkpointed directory which created by file system rewinder
2-4. Lower
- Links of current container's baseline images
- Divided by ':' and sorted
'''

import os
import shutil
import time
from socket import *
from threading import Thread

ROOT_PATH = '/var/lib/docker/overlay2/'

def counts_of_path(path):
    cnt = 0
    if os.path.isdir(path):
        for root, dirs, files in os.walk(path):
            for f in files:
                cnt += 1
    else:
        cnt += 1

    return cnt

def size_of_path(path):
    size = 0
    if os.path.isdir(path):
        for root, dirs, files in os.walk(path):
            for f in files:
                size += os.stat(root+"/"+f).st_size
    else:
        size += os.stat(path).st_size

    return size


def remove(path):
    print(path)
    try:
        if os.path.isdir(path):
            shutil.rmtree(path)
        else:
            os.remove(path)
    except Exception as e:
        print(e)


class REWINDER:
    def __init__(self, key):
        tmp = os.popen("docker inspect --format={{.GraphDriver.Data.WorkDir}} " + key).read().split('/')
        self.hash_key = tmp[-2]
        self.merged_path = ROOT_PATH + tmp[-2] + '/merged/'
        self.diff_path = ROOT_PATH + tmp[-2] + '/diff/'
        self.chk_path = ROOT_PATH + tmp[-2] + '/chk/'
        self.lower_path = ROOT_PATH + tmp[-2] + '/lower'
        self.name = os.popen("docker inspect --format={{.Name}} " + key).read()

        self.removal = []
        self.removal_dir = []
        self.rewind = []
        self.lower_link = []

        self.log_file = open("/home/user01/rewind_workload/results/file_rewinder.txt", "a")
        self.rm_size = 0
        self.restore_size = 0
        self.rm_entries = 0
        self.restore_entries = 0
        self.log_cnt = 0
       
        with open(self.lower_path, 'r') as l:
            lower = l.readline()
            self.lower_link = lower.split(':')

    def log(self):
        if self.log_cnt == 0:
            self.log_file.write(self.name)
        else:
            msg = '{0}: {1} {2} {3} {4}\n'
            self.log_file.write(msg.format(self.log_cnt, self.rm_size, self.rm_entries, self.restore_size, self.restore_entries))
        self.log_cnt += 1

    def get_origin(self, path):
	# Traverse "lower" and find last modified version of "path"
        res = ''
        for links in self.lower_link:
            if os.path.exists(ROOT_PATH + links + '/' + self.change_path(path,'rm')):
                res = ROOT_PATH + links + '/' + self.change_path(path,'rm')
                break
        return res

    def change_path(self, path, cases):
        res = path
        if cases == 'd2c':
            res = path.replace(self.diff_path, self.chk_path)
        elif cases == 'd2m':
            res = path.replace(self.diff_path, self.merged_path)
        elif cases == 'rm':
            res = path.replace(self.diff_path, '')
        return res

    def is_modify(self, path):
        diff_stat = os.stat(path)
        chk_stat = os.stat(self.change_path(path, 'd2c'))
        if diff_stat.st_mtime == chk_stat.st_mtime:
            return False
        else:
            return True
   
    def file_from_lower(self, path):
        # Check if file is in lower links, and restore if exist
        file_origin = self.get_origin(path)
        if file_origin == '':
            self.removal.append(path)
        else:
            # Restore from lower links (Cannot process at below code)
            shutil.copy2(file_origin, change_path(path, 'd2m'))
    
    def cleaning(self):
        # Clean up each object which used in checkpointing() and restoration()
        shutil.rmtree(self.chk_path)

    def checkpointing(self):
        shutil.copytree(self.diff_path, self.chk_path)
        print("Checkpoint layer size = " + str(size_of_path(self.chk_path)) + "bytes")

    def restoration(self):
        self.removal = []
        self.removal_dir = []
        self.rewind = []

        for root, dirs, files in os.walk(self.diff_path):
            # Skip the removal directory
            if len(self.removal_dir) > 0 and root.find(self.removal_dir[-1]) == 0:
                continue

            # When directory exist on checkpoint
            if os.path.exists(self.change_path(root,'d2c')):
                for f in files:
                    if os.path.exists(self.change_path(root+'/'+f,'d2c')):
                        # modification check and add into REWIND
                        if self.is_modify(root+'/'+f):
                            self.rewind.append(root+'/'+f)
                    else:
                        self.file_from_lower(root+'/'+f)
            # If not, find from lower links
            else:
                origin_path = self.get_origin(root)
                if origin_path == '':
                    # This directory is created
                    self.removal_dir.append(root)
                else:
                    for f in files:
                        self.file_from_lower(root+'/'+f)
                        

        # Remove
        self.removal.extend(self.removal_dir)
        self.rm_size = 0
        self.rm_entries = len(self.removal)
        for path in self.removal:
            rm_path = self.change_path(path,'d2m')
            self.rm_size += size_of_path(rm_path)
            remove(rm_path)
        print("Remove: " + str(self.rm_entries) + " entries (Size= "+str(self.rm_size)+" bytes)")

        # Restore (only file)
        self.restore_size = 0
        self.restore_entries = len(self.rewind)
        for path in self.rewind:
            restore_path = self.change_path(path, 'd2m')
            shutil.copy2(self.change_path(path,'d2c'), restore_path)
            self.restore_size += size_of_path(restore_path)
        #print("Restore: " + str(restore_len) + " entries (Size= "+str(size)+" bytes)")

        #cleaning()


def rewinder_thread(conn):
    cnt = 0
    tlist = []
    elist = []
    key = conn.recv(1024).decode('utf-8')
    file_rewinder = REWINDER(key)
    while True:
        msg = conn.recv(10).decode('utf-8')
        if msg == 'checkpoint':
            tmp = time.time()
            file_rewinder.checkpointing()
            print("Checkpoint!" + str(time.time()-tmp) + "sec")
        elif msg == 'rewind':
            tmp = time.time()
            file_rewinder.restoration()
            #print("Start at " + str(tmp) + ", Takes " + str(time.time()-tmp))
            tmp2 = time.time()
            tlist.append(str(tmp2-tmp))
            elist.append(str(tmp))
            file_rewinder.log()
            cnt += 1
        
        if cnt == 11:
            file_rewinder.log_file.write("lat: "+tlist[1]+" "+tlist[2]+" "+tlist[3]+" "+tlist[4]+" "+tlist[5]+" "+tlist[6]+" "+tlist[7]+" "+tlist[8]+" "+tlist[9]+" "+tlist[10]+"\n")
            file_rewinder.log_file.write("time: "+elist[1]+" "+elist[2]+" "+elist[3]+" "+elist[4]+" "+elist[5]+" "+elist[6]+" "+elist[7]+" "+elist[8]+" "+elist[9]+" "+elist[10]+"\n")
            file_rewinder.log_file.close()
            conn.close()
            break
        


port = 40510
serverSock = socket(AF_INET, SOCK_STREAM)
serverSock.bind(('172.17.0.1', port))

while True:
    serverSock.listen(1)    
    conn, addr = serverSock.accept()
    t = Thread(target=rewinder_thread, args=(conn,))
    t.start()

serverSock.close()

