soFile : interface.o
	gcc -Wall interface.o -shared -o vmLib.so

interface.o : ../vmioctl.h
	gcc -Wall -c -fPIC interface.c -o interface.o

clean :
	rm interface.o vmLib.so