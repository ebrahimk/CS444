#!/bin/bash

GROUP_NUM=10
PROJECT_DIR="/scratch/spring2018/$GROUP_NUM"

# Setup enviroment

. /scratch/opt/environment-setup-i586-poky-linux

cd $PROJECT_DIR

mkdir ./linux-yocto-3.19.2-best_fit
mkdir ./linux-yocto-3.19.2-first_fit


cd ./linux-yocto-3.19.2-best_fit

echo "Making best_fit VM..."
unzip -q ../linux-yocto-3.19.2.zip

patch ./linux-yocto-3.19.2/mm/slob.c -i $PROJECT_DIR/CS444/project4/patch/best_fit.patch

echo "Copying source config files..."
cp /scratch/files/config-3.19.2-yocto-standard ./.config
cp /scratch/files/bzImage-qemux86.bin .
cp /scratch/files/core-image-lsb-sdk-qemux86.ext4 .
cp $PROJECT_DIR/CS444/project3/includes/spin-vm.sh .

make -j4 all

echo
echo
echo "   DONE"

cd ../linux-yocto-3.19.2-first_fit

echo "Making first_fit VM..."
unzip -q ../linux-yocto-3.19.2.zip

patch ./linux-yocto-3.19.2/mm/slob.c -i $PROJECT_DIR/CS444/project4/patch/first_fit.patch

cp /scratch/files/config-3.19.2-yocto-standard ./.config
cp /scratch/files/bzImage-qemux86.bin .
cp /scratch/files/core-image-lsb-sdk-qemux86.ext4 .
cp $PROJECT_DIR/CS444/project3/includes/spin-vm.sh .

make -j4 all

echo
echo
echo "   DONE"
sleep 3

echo "The script will continue on after the best_fit VM is started"
echo "Please start the VM in another terminal and log in with username: root"
echo
echo "  Launch spin-vm.sh within the best_fit VM directory"
echo "  NOTE: If you get a permissions error when trying to load the drive image,"
echo "        perform: chmod 770 core-image-lsb-sdk-qemux86.ext4"

read -p "Press enter to continue when ready..."

echo "scp'ing files to the VM... "
echo 

cd $PROJECT_DIR/CS444/project4/test
scp -P 5510 ./Makefile root@localhost:~/
scp -P 5510 ./s-call.c root@localhost:~/
scp -p 5510 ./test.sh root@localhost:~/
echo "Tests copied to the VM's /home/root/"

echo "Perform tests as shown in the testing notes for the best_fit VM"
echo "and then shutdown the VM when finished"

read -p "Press enter to continue when ready..."

echo "The script will continue on after the first_fit VM is started"
echo "Please start the VM in another terminal and log in with username: root"
echo
echo "  Launch spin-vm.sh within the first_fit VM directory"
echo "  NOTE: If you get a permissions error when trying to load the drive image,"
echo "        perform: chmod 770 core-image-lsb-sdk-qemux86.ext4"

read -p "Press enter to continue when ready..."

echo "scp'ing files to the VM... "
echo 
scp -P 5510 ./Makefile root@localhost:~/
scp -P 5510 ./s-call.c root@localhost:~/
scp -p 5510 ./test.sh root@localhost:~/
echo "Tests copied to the VM's /home/root/"

echo "Perform tests as shown in the testing notes for the best_fit VM"
echo "and then shutdown the VM when finished"

read -p "Press enter to continue when ready..."

echo "Setup script finished. Exiting..."

exit 0
