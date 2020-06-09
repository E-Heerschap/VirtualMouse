soFile : interface.o
	gcc -D VM_EXPORT_GET_BUTTONS -Wall interface.o -shared -o libvm.so

interface.o : ../vmioctl.h
	gcc -Wall -c -fPIC interface.c -o interface.o

clean :
	rm interface.o libvm.so