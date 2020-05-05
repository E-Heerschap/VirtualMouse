//VirtualMouse - A driver to emulate a BUS protocol mouse for the linux kernel.
//Author: Edwin Heerschap
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/init.h>
#include "vmlock.h"
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Edwin Heerschap");

#define INSTRUC_BUF_LEN 10
#define BASE_MINOR 0
#define DRIVER_NAME "Virtual Mouse"

#ifndef VM_MAJOR
	#define VM_MAJOR 0 //0 = Dynamic assignment of major number
#endif

#define VM_MINOR_TYPE ushort //Required because preprocessor is unaware of
//typedef declarations and minor_t (below) type is required in 
//module_param macro.

typedef VM_MINOR_TYPE minor_t;
#define WORLD_READABLE 0444

static minor_t argMinorCount = 1;

module_param(argMinorCount, VM_MINOR_TYPE, WORLD_READABLE);
MODULE_PARM_DESC(argMinorCount, "Number of virtual mice to create.");

//Defines the instruction types that can be executed.
enum vmInstructionType {
	
	leftDown = 0,
	leftUp = 1,
	middleDown = 2,
	middleUp = 3,
	rightDown = 4,
	rightUp = 5, 
	move = 6,
	scroll = 7

};

//Defined to wrap vmInstructionType for now because may need to include
//extra information about instruction in the future.
struct vmInstruction {
	enum vmInstructionType type;
};

struct vmDevice {
	minor_t minor;
	struct vmInstruction intstruction[INSTRUC_BUF_LEN];
	struct vmLock lock;
	struct cdev dev;
};

static struct vmDevice* vmDevices;

static ssize_t vmRead(struct file* filp, char __user* buf, size_t count,
	       	loff_t* offset)
{
	
}

static ssize_t vmWrite(struct file* filp, const char __user* buf, size_t count, 
		loff_t* offset)
{



}

static __poll_t vmPoll(struct file * filp, struct poll_table_struct * pts) 
{
	
}

static int vmOpen (struct inode * in, struct file * filp) 
{
		
	printk(KERN_ALERT "WORKING");

}

static int vmRelease (struct inode * in, struct file * filp)
{

}

struct file_operations fops = {
	
	.open = vmOpen,
	.release = vmRelease,
	.write = vmWrite,
	.read = vmRead,
	.poll = vmPoll

};

//Used instead of VM_MAJOR definition because if VM_MAJOR = 0 dynamic
//assignment occurs.
unsigned int vmMajor;

static int __init virtualMouseInit(void)
{

	minor_t devNum = BASE_MINOR;
	dev_t firstDev = MKDEV(0, BASE_MINOR);
	struct vmDevice device;

	printk(KERN_ALERT "Virtual Mouse Driver Starting");	

	if (alloc_chrdev_region(&firstDev, BASE_MINOR, argMinorCount,
			       	DRIVER_NAME)) {
	
		printk(KERN_ALERT "Virtual Mouse Failed to allocate \
					character device region");
		return 1;
	}

	vmMajor = MAJOR(firstDev);

	//Number of devices is passed as parameter so device array size
	//is dynamically assigned.
	vmDevices = (struct vmDevice *) kmalloc(argMinorCount * \
		       	sizeof(struct vmDevice), GFP_KERNEL);
	
	
	for(; devNum < argMinorCount + BASE_MINOR; devNum++) {
		
		dev_t devicePair = MKDEV(vmMajor, devNum);
		
		device.minor = devNum;
		device.lock = spinlockBuilder();

		
		cdev_init(&device.dev, &fops);
		device.dev.owner = THIS_MODULE;

		//Last parameter is 1 as we only want 1 minor number
		//corresponding to each virtual device.
		cdev_add(&device.dev, devicePair, 1);
		
		vmDevices[devNum] = device;
	}

	return 0;

}

static void __exit virtualMouseExit(void)
{

	if (vmDevices) {
		
		unsigned short deviceNo;
		for(deviceNo = 0; deviceNo < argMinorCount; deviceNo++) {
					
			cdev_del(&vmDevices[deviceNo].dev);
			vmDevices[deviceNo].lock.cleanup(&vmDevices[deviceNo].lock);
			
		}
		
		kfree(vmDevices);
	}
	
	unregister_chrdev_region(MKDEV(vmMajor, BASE_MINOR), argMinorCount);

}

module_init(virtualMouseInit);
module_exit(virtualMouseExit);
