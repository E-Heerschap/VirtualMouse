//VirtualMouse  - A driver to emulate a mouse of different protocols for the
//linux kernel. General software design can be found in the 
//design folder. Will make reading my ugly code a bit easier :).
//Author: Edwin Heerschap

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include "vmtypes.h"
#include "vmdebug.h"

#define VM_BASE_MINOR 0
#define VM_DEVMEMALLOC argMinorCount //Number of vmDevices to initially
//allocate memory on the heap for. Change this to something small to value
//memory over cpu time.
#define VM_DRIVER_NAME "Virtual Mouse"
#ifndef VM_MAJOR
#define VM_MAJOR 0
#endif
#define VM_MINOR_TYPE ushort //Required because preprocessor is unaware of
//typedef declarations and minor_t (below) is required in module_param macro.
#define WORLD_READABLE 0444


typedef VM_MINOR_TYPE minor_t;
typedef struct vmAccess vmAccess; /////////////////////////
typedef struct vmProtocol vmProtocol; // Might never need extra abstraction :)
typedef struct vmDevice vmDevice;/////////////////////////

/////////////////////////////////////
// Module information
////////////////////////////////////
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Edwin \"KingPulse\" Heerschap");
MODULE_DESCRIPTION("A driver for virtual mouse");

////////////////////////////////////
// Module Parameters
//////////////////////////////////
static minor_t argMinorCount = 1;
module_param(argMinorCount, VM_MINOR_TYPE, WORLD_READABLE);
MODULE_PARM_DESC(argMinorCount, "Number of minor numbers to allocate." \
		"This will be the maximum number of mice allocatable.\n");//For now

///////////////////////////////////
// CORE CODE \_( ;; )_/
/////////////////////////////////

static unsigned int vmMajor;

static vmDevice* vmDevices;

static int __init vmCoreInit(void)
{

	minor_t devNum = VM_BASE_MINOR;
	dev_t firstDev = 0;
	int result = 0, vmDeviceSize = 0;

	printk(KERN_NOTICE "Virtual Mouse Driver Starting\n");
	
	//Getting driver major and minor regions

	result = alloc_chrdev_region(&firstDev, VM_BASE_MINOR, argMinorCount,
			VM_DRIVER_NAME);

	if (result) {

		VM_DEBUGVARS("alloc_chrdev_region -> %d\n", result)

		printk(KERN_ALERT "Virtual Mouse failed to allocate \
				character device region.\n");
		return 1;
	}

	vmMajor = MAJOR(firstDev);

	vmDeviceSize = sizeof(vmDevice);
	vmDevices = (struct vmDevice *) kmalloc(VM_DEVMEMALLOC * vmDeviceSize,
			GFP_KERNEL);
	memset(vmDevices, 0, VM_DEVMEMALLOC * vmDeviceSize);

	// TODO Implement loading mice from start up parameters.
	
	printk(KERN_NOTICE "Virtual Mouse started successfully\n");

	return 0;
}

static void __exit vmCoreExit(void)
{
	minor_t devNo;
	vmDevice zeroDev;
	int vmDeviceSize = sizeof(vmDevice);
	printk(KERN_NOTICE "Stopping Virtual Mouse\n");
	
	if (vmDevices) {

		VM_DEBUG("vmDevices not NULL\n")

		for(devNo = 0; devNo < argMinorCount; devNo++) {
			
			//They may be NULL as the entire array
			//is initialized with zeros during init.
			//Potential bug due to padding here? Not sure :/
			//As long as vmDevices[devNo] remains untouched
			//it should be gaurunteed to have 0s in padding also:
			//https://stackoverflow.com/questions/52684192
			memset(&zeroDev, 0, vmDeviceSize);
			if (memcmp(&vmDevices[devNo], &zeroDev,
				       	vmDeviceSize)) {

				VM_DEBUGVARS("vmDevice[%d] not null\n", devNo)

				// TODO Clean vmDevice!
			}

			#ifdef VM_DEBUG_ENABLE
			else {VM_DEBUGVARS("vmDevice[%d] was null\n", devNo);}
			#endif

		}

		kfree(vmDevices);


	}

	unregister_chrdev_region(MKDEV(vmMajor, VM_BASE_MINOR), argMinorCount);
}

module_init(vmCoreInit);
module_exit(vmCoreExit);
