obj-m := virtualmouse.o
virtualmouse-objs := main.o vmlock.o
ccflags-y := -std=gnu89
