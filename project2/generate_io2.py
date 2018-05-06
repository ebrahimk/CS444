#! /usr/bin/env python

import os
import random
import string
import shutil

starting_dir = os.getcwd()
directory = "test_dir_second"
directory2 ="test_dir_second1"
mystr = "file"


#if we only generate I/O in one directory all of the requests occur in roughly the same sector
#we thus want to skip around the directory structure
for z in range(50):	
	if not os.path.exists(directory):
   		 os.makedirs(directory)
	os.chdir(directory)
	
	for x in range(20):
		file_pointer = open(mystr+str(x)+".txt","w+")
		for y in range(10):
			newChar = random.choice(string.ascii_lowercase)
			file_pointer.write(newChar)
			#sys.stdout.write(newChar) 
		file_pointer.write("\n")
		file_pointer.close()

	for x in range(20):
        	file_pointer = open(mystr+str(x)+".txt","r")
       		file_pointer.read()  
		file_pointer.close()	

	os.chdir('..') #move to folder in which script exists 
	shutil.rmtree(directory) #delete the folder for clean up

	os.chdir('..')	#move one folder above that

        if not os.path.exists(directory):
                 os.makedirs(directory)
        os.chdir(directory)

        for x in range(20):
                file_pointer = open(mystr+str(x)+".txt","w+")
                for y in range(10):
                        newChar = random.choice(string.ascii_lowercase)
                        file_pointer.write(newChar)
                        #sys.stdout.write(newChar) 
                file_pointer.write("\n")
                file_pointer.close()

        for x in range(20):
                file_pointer = open(mystr+str(x)+".txt","r")
                file_pointer.read()
                file_pointer.close()	

	os.chdir('..') #move to the parent directory 
	shutil.rmtree(directory) # delete it
	
	os.chdir(starting_dir) #move back to home dir
	if not os.path.exists(directory):
                 os.makedirs(directory)
	os.chdir(directory)

	#move into a sub directory
        if not os.path.exists(directory2):
                 os.makedirs(directory2)
        os.chdir(directory2)

        for x in range(20):
                file_pointer = open(mystr+str(x)+".txt","w+")
                for y in range(10):
                        newChar = random.choice(string.ascii_lowercase)
                        file_pointer.write(newChar)
                        #sys.stdout.write(newChar) 
                file_pointer.write("\n")
                file_pointer.close()

        for x in range(20):
                file_pointer = open(mystr+str(x)+".txt","r")
                file_pointer.read()
                file_pointer.close()

	os.chdir('..') 
	shutil.rmtree(directory2)
	os.chdir('..') #move to the parent directory 
        shutil.rmtree(directory) # delete it
	#back in the home directory



