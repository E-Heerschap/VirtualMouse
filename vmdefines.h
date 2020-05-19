//Contains general definitions and macros for virtual mouse. This is most
//likely included in every virtual mouse file.
//Author: Edwin Heerschap

#ifndef VM_DEFINES_H
#define VM_DEFINES_H

#include "vmdebug.h"

#define VM_ERROR 1 // Non-fatal error
#define VM_FATAL -1 //Fatal error
#define VM_SUCCESS 0 // =D

//Object call definitions. Call object methods on themselves
//in different manners.                          //////////////////////////
#define VM_POC_R_(self, call) self->call(self);  // POC = Pointer object call
#define VM_POC_C_(self, call) self->call(self*); // R = reference, C = copy
#define VM_OC_R_(self, call) self.call(&self);   // OC = Object call 
#define VM_OC_C_(self, call) self.call(self);    /////////////////////////

//Checks if fields are null before attempting to call "call".
#define NULL_CHECK_CALL_SELF(ptr, call) \
if (ptr != NULL) { \
	if (ptr->call != NULL) { \
	        VM_POC_R_(ptr, call) \
	} \
	else { \
	       	VM_DEBUG("NULL_CHECK_CALL passed ptr -> call is NULL\n"); \
	} \
} \
else { \
	VM_DEBUG("NULL_CHECK_CALL passed ptr is NULL!\n"); \
}

#endif
