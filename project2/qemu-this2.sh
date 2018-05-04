#!/bin/bash

# Current config when running the VM
qemu-system-i386 -gdb tcp::5510 -S -nographic -kernel ./arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext4 -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug" -redir tcp:2222::22

# -gdb tcp::5510 
#The -redeir tcp:2200::22, this redirects traffic on the host port 2200 to port 22 on the QEMU VM, to ssh into the VM-->
#run: "ssh -p 2200 qemu-user@localhost" //you will have username "qemu-user", then we can scp the mypython.py into the vm to test the I/O 
