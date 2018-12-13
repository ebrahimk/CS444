#!/bin/bash

if [ "$#" -eq 1 ]
then
    if [[ "$1" == "setup" ]]
    then
        echo "$0: setting up..."
        insmod sbd.ko
        shred -z /dev/sbd0
        mkfs.ext2 /dev/sbd0
        mount /dev/sbd0 /mnt/
        lsmod
	echo
        echo "$0: setup complete!"
	exit 0

    elif [[ "$1" == "teardown" ]]
    then
        echo "$0: tearing down..."
        umount /mnt
        rmmod sbd.ko
        lsmod
	echo
        echo "$0: teardown complete!"
        exit 0

    elif [[ "$1" == "demo" ]]
    then
	echo
        echo "$0: running demo..."

        echo "creating demofile in /mnt/ block device."
        touch /mnt/demofile
        ls -la /mnt/
        echo 

        echo "writing 'Hello world!' to /mnt/ block device."
        echo "using command: echo 'Hello world!' > /mnt/demofile"
        echo "Hello world!" > /mnt/demofile
        echo "contents of demofile:"
        cat /mnt/demofile
        echo 

        echo "searching for 'Hello world!' in /dev/sbd0."
        echo "using command: grep -a 'Hello world!' /dev/sbd0"
        grep "Hello world!" /dev/sbd0
        echo 
        echo "If nothing was displayed 'Hello world!' could not be found"
        echo 

        echo "To prove encryption of /sbd0 cat'ing its contents"
        echo "using command: cat /dev/sbd0"
        echo 
        cat /dev/sbd0
        echo 
        echo "gibberish should be displayed above."
        echo 

        echo "encryption and decryption occurs when commands are written to /mnt/"
        echo " cat'ing contents of demofile from /mnt/demofile:"
        echo "using command: cat /mnt/demofile"
        cat /mnt/demofile
        echo ""

        echo "deleting /mnt/demofile"
        rm /mnt/demofile
	echo
        echo "$0: demo complete!"
        exit 0

    elif [[ "$1" == "listkey" ]]
    then
        echo "listing encryption key from dmesg...\n"
        dmesg | grep -a -m 1 "sbd: enc_key >"
        echo "if two or more keys appear use 'dmesg -c' to clear dmesg and re run I/O"
        echo 
    fi
elif [ "$#" -eq 2 ]
then
    if [[ "$1" == "setup" ]]
    then
        echo "$0: setting up..."
        insmod sbd.ko key="$2"
        shred -z /dev/sbd0
        mkfs.ext2 /dev/sbd0
        mount /dev/sbd0 /mnt/
        lsmod
	echo
        echo "$0: setup complete!"
        exit 0
    fi
else
    echo "$0: { demo | listkey | setup [encryption-key] | teardown }" >&2
    exit 1
fi
