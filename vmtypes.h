//Defines type for the virtual mouse system
//Author: Edwin Heerschap

#ifndef VMDEVICE_H
#define VMDEVICE_H

#include "vmdefines.h"
#include <linux/fs.h>
#include <linux/cdev.h>

//Represents an access control for a virtual mouse.
struct vmAccess {
	char* name;
	struct file_operations* fops;
	void (*cleanup) (struct vmAccess *); //Do not use vmAccess struct afterwards.

};



//Represents a protocol for a virtual mouse.
struct vmProtocol {
	char* name;
	struct file_operations* fops;
	int (*ioctl) (struct inode*, struct file*, unsigned int, unsigned long);
	void (*cleanup) (struct vmProtocol *);
};


//Represents a virtual mouse device
struct vmDevice {
	
	struct vmProtocol* protocol;
	struct vmAccess* access;
	struct cdev cdev;
	dev_t dev;
	int (*ioctl) (struct inode *, struct file*, unsigned int, unsigned long);
	void (*cleanup) (struct vmDevice *);

};


#endif
