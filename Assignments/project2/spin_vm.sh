#!/bin/bash

# Current config when running the VM
qemu-system-i386 -gdb tcp::5510 -S -nographic -kernel ./arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext4 -enable-kvm -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug" -redir tcp:2222::22

#to view scheduler: cat /sys/block/hda/queue/scheduler  
#to change scheduler: echo look > /sys/block/hda/queue/scheduler
#remove -net none --> we can use wget to grab the python script located at https://raw.githubusercontent.com/ebrahimk/CS444/master/project2/mypython.py
