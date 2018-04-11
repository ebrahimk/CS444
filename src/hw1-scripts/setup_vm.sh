#!/bin/bash

SCRIPT_DIR=$(pwd)

GROUP_NUM=10

PROJECT_DIR="/scratch/spring2018/$GROUP_NUM"

# The following commands setup the necessary environment for HW1

# Make the group's directory (group 10) if it doesn't exist
if [ ! -d "$PROJECT_DIR" ]
then
	echo "Making new project directory..."
	mkdir "$PROJECT_DIR"
fi
echo "Project directory exists - moving to.."
cd "$PROJECT_DIR"

# Download the proper kernel
echo "Getting yocto kernel..."
wget http://git.yoctoproject.org/cgit.cgi/linux-yocto/snapshot/linux-yocto-3.19.2.zip

# Unzip it here (no stdout)
echo "Unzipping in current location..."
unzip -q linux-yocto-3.19.2.zip

# Move into the new dir
echo "Moving into new dir..."
cd ./linux-yocto-3.19.2

# Copy the proper config file
echo "Copying source config files..."
cp /scratch/files/config-3.19.2-yocto-standard ./.config

# Copy the proper initial binary and ext4 file system
cp /scratch/files/bzImage-qemux86.bin .
cp /scratch/files/core-image-lsb-sdk-qemux86.ext4 .

# Begin compiling the new kernel
echo "Compiling new kernel for VM..."
make -j4 all

# Copy the qemu command script over to the project directory
cp $SCRIPT_DIR/qemu-this.sh .


printf "\n\n     \033[0;32mCOMPLETE\033[0m\n\n"


exit
