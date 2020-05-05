//VirtualMouse - A driver to emulate a BUS protocol mouse for the linux kernel.
//Author: Edwin Heerschap
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include "vmlock.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Edwin Heerschap");

#define INSTRUC_BUF_LEN 10
#define BASE_MINOR 0
#define DRIVER_NAME "Virtual Mouse"

#ifndef VM_MAJOR
	#define VM_MAJOR 0 //0 = Dynamic assignment of major number
#endif

typedef uint8_t minor_t;

static minor_t argMinorCount = 1;

MODULE_PARAM(argMinorCount, "i");
MODULE_PARAM_DESC(argMinorCount, "Number of virtual mice to create.");

//Defines the instruction types that can be executed.
static enum vmInstructionType {
	
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
static struct vmInstruction {
	enum vmInstructionType type;
};

static struct vmDevice {
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
		

}

static vmRelease (struct inode * in, struct file * filp)
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

int virtualMouseInit(void)
{
	
	//Check linux/fs/char_dev.c, helpful information in there.
	//
	//Registering directly because we don't want to preallocate a dev_t.
	struct char_device_struct *cd;
	cd = __register_chrdev_region(VM_MAJOR, BASEMINOR, 
			VM_MINOR_COUNT, DRIVER_NAME);
	
	if (IS_ERR(cd)) {
		printk(KERN_ALERT "Failed to start Virtual Mouse Driver");
		return PTR_ERR(cd);
	}

	vmMajor = cd->major;
	
	//Number of devices is passed as parameter so device array size
	//is dynamically assigned.
	vmDevices = (vmDevice *) kmalloc(argMinorCount * sizeof(vmDevice),
			GFP_KERNEL);

	for(minor_t devNum = BASE_MINOR; 
			devNum < argMinorCount + BASEMINOR; i++) {
		
		dev_t *devicePair = MKDEV(cd->vmMajor, devNum);
		
		struct vmDevice device {
			.minor = devNum,
			.instruction = [],
			.lock = spinlockBuilder()
		};
		
		cdev_init(&device.dev, &fops);
		device.dev.owner = THIS_MODULE;

		//Last parameter is 1 as we only want 1 minor number
		//corresponding to each virtual device.
		cdev_add(device.dev, devicePair, 1);
		
		vmDevices[devNum] = device;
	}

	return 0;

}

void virtualMouseExit(void)
{

	if (vmDevices) {
		
		for(int deviceNo = 0; deviceNo < argMinorCount; deviceNo++) {
			
			cdev_del(&vmDevices[deviceNo].cdev);
			
		}
		
		kfree(vmDevices);
	}
	
	unregister_chrdev_region(MKDEV(vmMajor, BASE_MINOR), argMinorCount);

}

module_init(virtualMouseInit);
module_exit(virtualMouseExit);
