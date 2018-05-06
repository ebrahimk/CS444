#!/bin/bash

cat /sys/block/hda/queue/scheduler
#change scheduler
echo look > /sys/block/hda/queue/scheduler
#verify scheduler change
cat /sys/block/hda/queue/scheduler
#Writes any data buffered in memory out to disk
#ensures we are doing disk I/O vs accessing data from RAM
sync
#grab from urandom (psuedo random number file) to ./data 8192 blocks of 1024 bytes 
dd if=/dev/urandom of=./data bs=1024 count=8192
sync
