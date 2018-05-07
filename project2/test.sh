#!/bin/bash

#see currently in use scheduler
cat /sys/block/hda/queue/scheduler

#change scheduler
echo look > /sys/block/hda/queue/scheduler

#verify scheduler change
cat /sys/block/hda/queue/scheduler

#compile the I/O generate function 
c++ gen_IO.cpp -o gen_io

#call the executable
./gen_io 

#turn look off so printk statements quit popping up 
echo cfq > /sys/block/hda/queue/scheduler

#use the grep command to isolate request that were queued up
dmesg | grep LOOK_DISPATCH > data_results

#cat the results out 
cat data_results
