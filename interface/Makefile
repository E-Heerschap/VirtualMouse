soFile : interface.o
	gcc -Wall interface.o vmioctl.o -shared -o libvm.so

interface.o :
	gcc -Wall -c -fPIC interface.c -o interface.o
	gcc -Wall -c -fPIC -D VM_EXPORT_GET_BUTTONS vmioctl.c -o vmioctl.o

clean :
	rm interface.o vmioctl.o libvm.so