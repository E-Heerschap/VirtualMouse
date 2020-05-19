//Provides the protocols for virtual mouse
//Author: Edwin Heerschap


#ifndef VM_PROTOCOL_H
#define VM_PROTOCOL_H

//Different style to the access system. Factories defined here
//instead of in their own header file. Will eventually see which is nicer...

struct vmProtocol * vmBUSProtocolFactory(void);

#define VM_PROTOCOL_COUNT 1

struct vmProtocol * (*protocolFactories[VM_PROTOCOL_COUNT]) (void) = {
	vmBUSProtocolFactory	
};

#endif
