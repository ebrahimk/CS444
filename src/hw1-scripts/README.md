# README

### THIS WILL ONLY RUN ON os2.engr.oregonstate.edu


To setup, run the setup-vm.sh script.
This will pull the correct version of yocto, and compile the qemu VM.

Execute the VM with the qemu-this.sh script:
`./qemu-this.sh`

To access the VM, run GDB: 
`$ $GDB vmlinux`

In GBD:
`target remote :5510`
`continue`

Go checkout the console where the qemu VM was spun for the VM interface.
