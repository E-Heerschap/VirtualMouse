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
#include "vmdefines.h"
#include "vmtypes.h"
#include "vmdebug.h"
//#include "vmioctl.h"
#include "protocol/protocol.h"
#include "access/access.h"


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
#define VM_ARG_TYPE short
#define WORLD_READABLE 0444


typedef VM_MINOR_TYPE minor_t;
typedef struct vmAccess vmAccess; /////////////////////////
typedef struct vmProtocol vmProtocol; // Might never need extra abstraction :)
typedef struct vmDevice vmDevice;/////////////////////////
typedef VM_ARG_TYPE arg_t;

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

static arg_t argProtocol = 0;
module_param(argProtocol, VM_ARG_TYPE, WORLD_READABLE);
MODULE_PARM_DESC(argProtocol, "Default protocol to use for mice\n");

static arg_t argAccess = 0;
module_param(argAccess, VM_ARG_TYPE, WORLD_READABLE);
MODULE_PARM_DESC(argAccess, "Default access to use for mice\n");

static arg_t argMiceCount = 1;
module_param(argMiceCount, VM_ARG_TYPE, WORLD_READABLE);
MODULE_PARM_DESC(argMiceCount, "Number of mice to create on startup.");

///////////////////////////////////
// CORE CODE \_( ;; )_/
/////////////////////////////////

static unsigned int vmMajor;

static vmDevice* vmDevices;


/*******************************************************************
 * file_operations managment for vmCore
 *******************************************************************/

/**
 * Calls the file operations on the accessFOP and protocolFOP. 
 * If the access returns VM_FAILURE in the first byte, it is successful and the 
 * call to the protocol with the same arguments will then be made. Otherwise,
 * it should return VM_FAILURE;
 *
 * @accessFOP Type: vmAccess
 * @protocolFOP type: vmProtocol
 * @data Is the variable to store the result data in. A caveat is if the access
 * fails, then the data variables first byte will be -1;
 */
#define vmCoreFOPHandler(accessFOP, protocolFOP, data, args...) \
	if (accessFOP != NULL) { \
		data = accessFOP(args); \
		if ((char) data != (char) VM_SUCCESS) { \
			VM_DEBUG("Acceess to "#accessFOP" is denied!"); \
		}\
	}\
	\
	if (protocolFOP != NULL) { \
		data = protocolFOP(args); \
	}


static int vmCoreOpen(struct inode * in, struct file * filp) {
	
	vmDevice * device;
	int result = 0;

	device = container_of(in->i_cdev, vmDevice, cdev);

	VM_DEBUG("vmCoreOpen()");

	filp->private_data = device;
	
	vmCoreFOPHandler(device->access->fops->open,
			device->protocol->fops->open,
			result, in, filp);
	return result;
}

static ssize_t vmCoreRead(struct file * filp, char* __user buf, size_t size,
	       	loff_t* off){

	vmDevice * device = filp->private_data;
	ssize_t result = 0;
	vmCoreFOPHandler(device->access->fops->read,
		device->protocol->fops->read,
		result, filp, buf, size, off);
	return result;

}


#define __IOCTLChainLen 3
static struct __IOCTLChain {
    long (*ioctls[__IOCTLChainLen]) (struct file*, unsigned int, unsigned long);
    const long (*handler) (struct __IOCTLChain*, struct file*,
                           unsigned int, unsigned long);
    const void (*buildChain) (struct __IOCTLChain *, vmDevice);
}

static long __IOCTLChainHandler(
		const __IOCTLChain * chain,
		struct file * filp,
		unsigned int ioctlNum,
		unsigned long ptr
		) {
	long result;
	for(u8 count = 0; count < chain->len; count++) {
		if (chain->ioctl[count] != NULL){
			result = chain->ioctls[count](filp, ioctlNum, ptr);
			if (result != ENOIOCTLCMD) { return result };
		}
	}

	return ENOIOCTLCMD;
}

static void __IOCTLChainBuild(__IOCTLChain * chain, const vmDevice * device) {
	chain->ioctls = { device->ioctl,
		device->protocol->ioctl,
		device->fops->ioctl };
}

static __IOCTLChain* __getIOCTLChain(void) {

    return __IOCTLChain{{NULL,NULL,NULL},__IOCTLChainHandler,__IOCTLChainBuild};

}


static long vmCoreIOCTL(struct file * filp, unsigned int ioctlNum,
	       	unsigned long ptr) {

	long result = NULL;
	vmDevice * device = filp->private_data;
	struct __IOCTLChain chain = __getIOCTLChain();
	VM_AOC_R_(chain, buildChain, device);

	//TODO Add core IOCTL commands :)
	
	//IOCTL chain is different from the rest of the file_operations.
	//See the design document.
	//   |	
	//   v	in access/access.h
	if (VM_ACCESS_ALLOWED(device->access->fops->ioctl,
			       	ioctl, filp, ioctlNum, ptr)) {
			
		return VM_AOC_R_(chain, handler, filp, ioctlNum, ptr);
		
	} else {
		return EACCES; //Permission denied
	}

}
*/
/*******************************************************************
 * End of file_operations managment
 ********************************************************************/

static int argValidation(void) {

	if (VM_DEVMEMALLOC < argMiceCount) {
		//TODO Make the allocated memory expand dynamically in future.
		printk(KERN_ALERT "Virtual Mouse cannot start %u mice. "\
				"Maximum with compiled system is %d. To "\
				"change maximum use compile with KCFLAG "\
				"-D VM_DEVMEMALLOC=x\n"\
				"Starting with %d mice.\n", argMiceCount,\
				VM_DEVMEMALLOC, VM_DEVMEMALLOC);

		return VM_ERROR;
	}

	//TODO Have default vmAccess and vmProtocol to fall back on
	
	//-1 as VM_ACCESS_COUNT starts at 1.
	if (argAccess > VM_ACCESS_COUNT - 1 || argAccess < 0 )
	{
		printk(KERN_ALERT "Invalid argAccess argument. Maximum is %d " \
				"and minimum is 0\n", VM_ACCESS_COUNT);
		return VM_FATAL;
	}

	if (argProtocol > VM_PROTOCOL_COUNT - 1 || argProtocol < 0)
	{
		printk(KERN_ALERT "Invalid argProtocol argument. Maximum is " \
				"%d and minimum is 0\n", VM_PROTOCOL_COUNT);
		return VM_FATAL;
	}
	
	return VM_SUCCESS;	

}

void deviceCleanUp(vmDevice * vmd)
{

	NULL_CHECK_CALL_SELF(vmd->protocol, cleanup);
	NULL_CHECK_CALL_SELF(vmd->access, cleanup);
}


static struct file_operations vmCoreFops = {
	.owner = THIS_MODULE,
	.open = vmCoreOpen,
	.read = vmCoreRead
};

static int __init vmCoreInit(void)
{

	dev_t firstDev = 0;
	int result = 0, vmDeviceSize = 0, devCount = 0;
	vmDevice* device;

	printk(KERN_NOTICE "Virtual Mouse Driver Starting\n");
	
	//Performing pre-checks before allocating anything
	result = argValidation();
	
	if (result == VM_FATAL) {
		return VM_FATAL;
	}

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

	for (devCount = 0; devCount < argMiceCount; devCount++) {
		
		device = &vmDevices[devCount];
		device->access = accessFactories[argAccess](); //access/access.h
		device->protocol = protocolFactories[argProtocol]();
		device->ioctl = NULL; //TODO SETUP THIS		
		device->cleanup = deviceCleanUp;
		cdev_init(&device->cdev, &vmCoreFops);
		device->dev = MKDEV(vmMajor, devCount);
		cdev_add(&device->cdev, device->dev, 1);

	}
 
	
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
				VM_OC_R_(vmDevices[devNo], cleanup);
				cdev_del(&vmDevices[devNo].cdev);
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
