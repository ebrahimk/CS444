#!/bin/bash

GROUP_NUM=10
PROJECT_DIR="/scratch/spring2018/$GROUP_NUM"

# Setup enviroment

. /scratch/opt/environment-setup-i586-poky-linux

cd $PROJECT_DIR

echo "Unzipping yocto vanilla image"
unzip -q linux-yocto-3.19.2.zip

cd ./linux-yocto-3.19.2

echo "Copying source config files..."
cp /scratch/files/config-3.19.2-yocto-standard ./.config
cp /scratch/files/bzImages-qemux86.bin .
cp /scratch/files/core-image-lsb-sdk-qemux86.ext4 .
cp $PROJECT_DIR/CS444/project3/includes/spin-vm.sh .

# Begin compilation
echo "Compiling new kernel for VM..."
make -j4 all

echo ""
echo ""
echo "   DONE"
sleep 5

# Move to the directory containing the source code for the new module
cd $PROJECT_DIR/CS444/project3/module

# Compile the kernel module
make

echo "   DONE"
sleep 5

echo "The script will continue on after the VM is started"
echo "Please start the VM in another terminal and log in with username: root"
echo
echo "  Launch spin-vm.sh within the VM directory"
echo "  NOTE: If you get a permissions error when trying to load the drive image,"
echo "        perform: chmod 770 core-image-lsb-sdk-qemux86.ext4"

read -p "Press enter to continue when ready..."

echo "scp'ing files to the VM... "
echo 
scp -P 5510 sbd.ko root@localhost:~
echo
scp -P 5510 ../includes/demo-script.sh root@localhost:~

echo "Module and demo script copied to the VM's /home/root/"

cd $PROJECT_DIR/CS444/project3/

exit 0
