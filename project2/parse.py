#! /usr/bin/env python
#[  382.691180] LOOK_DISPATCH: 
import os
import random
import string

directory = "results"

if not os.path.exists(directory):
    os.makedirs(directory)

os.chdir(directory)

mystr = "/nfs/stak/users/ebrahimk/CS444/assign2/data"
file_pointer = open(mystr,"r")

contents = file_pointer.read().replace('LOOK_DISPATCH:', ' ').replace(']', ', ').replace('[', ' ')

file_pointer2 = open('data_prc',"w+")
file_pointer2.write(contents)
file_pointer2.close()

filepath = '/nfs/stak/users/ebrahimk/CS444/assign2/results/data_prc'
print "------TIME------\n"
with open(filepath) as fp:  
   line = fp.readline()
   while line:
       	time,sector = line.split(',')
	print time.replace(" ","")
	line = fp.readline()
	line = line.replace('\n', ' ')


print "---SECTOR-----"
with open(filepath) as fp:
   line = fp.readline()
   line = line.replace('\n', ' ');
   while line:
        time,sector = line.split(',')
        print sector.replace(" ","")
        line = fp.readline()
	line = line.replace('\n', ' ')

file_pointer.close()

