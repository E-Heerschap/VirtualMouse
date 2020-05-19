#ifndef VM_ACCESS_H
#define VM_ACCESS_H

//TODO for now the accesses will be defined by their number.
//In the future for usability it will be defined by string. I.e
//"all" for all access. HashMap may be helpful here.

#include "all.h"

#define VM_ACCESS_COUNT 1
struct vmAccess * ( *accessFactories[VM_ACCESS_COUNT]) (void) = {allAccessFactory};


#endif
