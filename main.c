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

#define VM_DEBUG

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
	dev_t devno;
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

struct file_operations fops = {};

//Used instead of VM_MAJOR definition because if VM_MAJOR = 0 dynamic
//assignment occurs.
unsigned int vmMajor;

static int __init virtualMouseInit(void)
{

	minor_t devNum = BASE_MINOR;
	dev_t firstDev = 0;
	
	struct vmDevice* device;

	printk(KERN_NOTICE "Virtual Mouse Driver Starting\n");	
		
	if (alloc_chrdev_region(&firstDev, BASE_MINOR, argMinorCount,
			       	DRIVER_NAME)) {
	
		printk(KERN_ALERT "Virtual Mouse Failed to allocate \
					character device region\n");
		return 1;
	}

	vmMajor = MAJOR(firstDev);

	#ifdef VM_DEBUG
		printk(KERN_ALERT "VM DEBUG: Major # given: %u\n", vmMajor);
		printk(KERN_ALERT "VM DEBUG: Minor count: %u\n", argMinorCount);
	#endif

	//Number of devices is passed as parameter so device array size
	//is dynamically assigned.
	vmDevices = (struct vmDevice *) kmalloc(argMinorCount * \
		       	sizeof(struct vmDevice), GFP_KERNEL);

	memset(vmDevices, 0, sizeof(struct vmDevice) * argMinorCount);
	
	for(; devNum < argMinorCount + BASE_MINOR; devNum++) {
		
		device = &vmDevices[devNum];
		device->devno = MKDEV(vmMajor, devNum);
		device->minor = devNum;
		device->lock = spinlockBuilder();

		cdev_init(&device->dev, &fops);
		device->dev.owner = THIS_MODULE;
		device->dev.ops = &fops;
		//Last parameter is 1 as we only want 1 minor number
		//corresponding to each virtual device.
		if(cdev_add(&device->dev, device->devno, 1)) {
			printk(KERN_ALERT "VirtualMouse failed to \
					add character device to region. \
					Major: %u, Minor: %u\n", vmMajor,
				       	MINOR(device->devno));
		}
		
	}
	
	printk(KERN_NOTICE "VirtualMouse started successfully\n");

	return 0;

}

static void __exit virtualMouseExit(void)
{
	unsigned short deviceNo;
	printk(KERN_CRIT "Stopping virtual mouse.\n");
	
	if (vmDevices) {
	
		#ifdef VM_DEBUG
		printk(KERN_ALERT "VM DEBUG: vmDevices not NULL\n");
		#endif	
		
	
		for(deviceNo = BASE_MINOR; deviceNo < argMinorCount + BASE_MINOR; deviceNo++) {
			
			#ifdef VM_DEBUG
			printk(KERN_ALERT "VM DEBUG: Removing device %u %u\n", \
				vmMajor, deviceNo);
			#endif
			cdev_del(&vmDevices[deviceNo].dev);
			//vmDevices[deviceNo].lock.cleanup(&vmDevices[deviceNo].lock);
			
		}
		
		kfree(vmDevices);
	}
	
	unregister_chrdev_region(MKDEV(vmMajor, BASE_MINOR), argMinorCount);
	
}

module_init(virtualMouseInit);
module_exit(virtualMouseExit);
