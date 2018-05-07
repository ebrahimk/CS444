#!/bin/bash

#see currently in use scheduler
cat /sys/block/hda/queue/scheduler

#change scheduler
echo look > /sys/block/hda/queue/scheduler
#echo cfq > /sys/block/hda/queue/scheduler

#verify scheduler change
cat /sys/block/hda/queue/scheduler


#compile the I/O generate function 
g++ gen_IO.cpp 
 
#make a new test dir and move to it
mkdir test 
mv a.out test 
cd ./test

#call the executable
./a.out

#turn look off so printk statements quit popping up 
echo cfq > /sys/block/hda/queue/scheduler

#use the grep command to isolate request that were queued up
dmesg | grep LOOK_DISPATCH > data_results

#cat the results out 
cat data_results
