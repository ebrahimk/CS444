#! /usr/bin/env python

import os
import random
import string

directory = "test_dir"

if not os.path.exists(directory):
    os.makedirs(directory)

os.chdir(directory)

mystr = "file"

for x in range(200):
	file_pointer = open(mystr+str(x)+".txt","w+")
	for y in range(10):
		newChar = random.choice(string.ascii_lowercase)
		file_pointer.write(newChar)
		#sys.stdout.write(newChar) 
	file_pointer.write("\n")
	file_pointer.close()
	#reopen the file to verify that the contents were written and a newline character is at the end


