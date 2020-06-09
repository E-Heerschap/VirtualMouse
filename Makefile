obj-m += virtualmouse.o
virtualmouse-objs += vmcore.o protocol/BUS.o access/all.o vmioctl.o
ccflags-y := -std=gnu89
