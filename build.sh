sudo make -C /home/virtualhideout/build M=`pwd` clean
sudo make -C /home/virtualhideout/build M=`pwd` "KCFLAGS= -D VM_DEBUG_ENABLE"  modules
