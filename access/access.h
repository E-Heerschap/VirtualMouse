#ifndef VM_ACCESS_H
#define VM_ACCESS_H

//TODO for now the accesses will be defined by their number.
//In the future for usability it will be defined by string. I.e
//"all" for all access. HashMap may be helpful here.

#include "../vmdefines.h"
#include "all.h"

//A successful access will either have NULL file_operation or 0 in the first
//byte of the return value. This (should) be equivalent of setting:
//(char) retval = (char) VM_SUCCESS; (I think).
//
//@x vmAccess to use
//@y Field in vmAccess->fops to use
//@args... arguments to pass to @y
#define VM_ACCESS_ALLOWED(x,y,args...) ((x->y == NULL || (char) x->y(args) == 0) \
		? 1 : 0)
#define VM_ACCESS_COUNT 1
struct vmAccess * ( *accessFactories[VM_ACCESS_COUNT]) (void) = {allAccessFactory};


#endif
