#!/bin/bash

cat /sys/block/hda/queue/scheduler
#change scheduler
#echo cfq > /sys/block/hda/queue/scheduler
#verify sceduler change
echo look > /sys/block/hda/queue/scheduler
cat /sys/block/hda/queue/scheduler

#call the I/O generate function 
generate_io.py

#turn look off so printk statements quit popping up 
echo cfq > /sys/block/hda/queue/scheduler

#use the grep command to isolate request that were queued up
dmesg | grep LOOK_DISPATCH > data_results

#cat the results out 
cat data_results
