#!/bin/bash
# Starting the virtual mouse system (Alpha start script)
sudo insmod virtualmouse.ko
MAJOR=$(cat /proc/devices | awk '/Virtual Mouse/ { print $1 }')
echo $MAJOR
sudo mknod /dev/vmouse0 c $MAJOR 0