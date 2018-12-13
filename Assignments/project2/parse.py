#! /usr/bin/env python
#[  382.691180] LOOK_DISPATCH: 
import os
import random
import string

directory = "results"

mystr = os.getcwd()+"/data"

if not os.path.exists(directory):
    os.makedirs(directory)

os.chdir(directory)


#mystr = os.getcwd()+"/data" 
file_pointer = open(mystr,"r")

contents = file_pointer.read().replace('LOOK_DISPATCH:', ' ').replace(']', ', ').replace('[', ' ')


file_pointer2 = open('data_prc',"w+")
file_pointer2.write(contents)
file_pointer2.close()

#file_ptr3 = open('final_data', "w+");

filepath = os.getcwd() +"/data_prc"
print "------TIME------\n"
with open(filepath) as fp:  
   line = fp.readline()
   while line:
       	time,sector = line.split(',')
	#file_ptr3.write(time.replace(" ",""));
	print time.replace(" ","")
	line = fp.readline()
	line = line.replace('\n', ' ')


print "---SECTOR-----"
with open(filepath) as fp:
   line = fp.readline()
   line = line.replace('\n', ' ');
   while line:
        time,sector = line.split(',')
        #file_ptr3.write(sector.replace(" ",""));
	print sector.replace(" ","")
        line = fp.readline()
	line = line.replace('\n', ' ')

#file_ptr3.close()
file_pointer.close()

