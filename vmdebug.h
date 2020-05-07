//Macros and definitions for debugging with virtual mouse
//Author: Edwin Heerschap

#ifndef VM_DEBUG_H
#define VM_DEBUG_H

//Compile with VM_DEBUG_ENABLE flag to enable debugging lines in console.
//OR (if you are lazy like me you can uncomment the line below)
//#define VM_DEBUG_ENABLE
#ifdef VM_DEBUG_ENABLE
	#define VM_DBGPFX "Virtual Mouse Debug: "
	#define VM_DEBUG(x) printk(KERN_ALERT VM_DBGPFX x);
	#define VM_DEBUGVARS(x,v) printk(KERN_ALERT VM_DBGPFX x, v); 
#else
	#define VM_DEBUG(x)
	#define VM_DEBUGVARS(x,v)
	#define VM_DBGPFX
#endif

#endif
