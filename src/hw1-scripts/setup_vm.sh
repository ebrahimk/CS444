#!/bin/bash

SCRIPT_DIR=$(pwd)

GROUP_NUM=10

PROJECT_DIR="/scratch/spring2018/$GROUP_NUM"

# The following commands setup the necessary environment for HW1

# Make the group's directory (group 10) if it doesn't exist
if [ ! -d "$PROJECT_DIR" ]
then
	printf "\033[0;36mMaking new project directory...\n"
	mkdir "$PROJECT_DIR"
fi
printf "\033[0;33mProject directory exists - moving to...\n"
cd "$PROJECT_DIR"

# Download the proper kernel
printf "\033[0;36mGetting yocto VM...\033[0;39m\n"
wget http://git.yoctoproject.org/cgit.cgi/linux-yocto/snapshot/linux-yocto-3.19.2.zip

# Unzip it here (no stdout)
printf "\033[0;36mUnzipping in current location...\n"
unzip -q linux-yocto-3.19.2.zip

# Move into the new dir
printf "\033[0;33mMoving into new dir...\n"
cd ./linux-yocto-3.19.2

# Copy the proper config file
printf "\033[0;36mCopying source config files...\n"
cp /scratch/files/config-3.19.2-yocto-standard ./.config

# Copy the proper initial binary and ext4 file system
cp /scratch/files/bzImage-qemux86.bin .
cp /scratch/files/core-image-lsb-sdk-qemux86.ext4 .

# Begin compiling the new kernel
printf "\033[0;35mCompiling new VM...\033[0;39m\n"
make -j4 all

# Copy the qemu command script over to the project directory
cp $SCRIPT_DIR/qemu-this.sh .


printf "\n\n     \033[0;32mCOMPLETE\033[0;39m\n\n"


exit
