//Defines type for the virtual mouse system
//Author: Edwin Heerschap

#ifndef VMDEVICE_H
#define VMDEVICE_H

#include <linux/fs.h>
#include <linux/cdev.h>

//Represents an access control for a virtual mouse.
//See next comment for "this"
struct vmAccess {
	char* name;
	struct file_operations fops;
	struct vmAccess* this;
};

//Represents a protocol for a virtual mouse.
//"vmProtocol* this" is recommended to be used with the container_of function
//so the vmProtocol can be an attribute in whatever structure the developer 
//requires.
struct vmProtocol {
	char* name;
	struct file_operations* fops;
	struct vmProtocol* this; //Just used as a helper :)
	int (*ioctl) (struct inode*, struct file*, unsigned int, unsigned long);
};

//Represents a virtual mouse device
struct vmDevice {
	
	struct vmProtocol protocol;
	struct vmAccess access;
	struct cdev cdev;
	dev_t dev;
	int (*ioctl) (struct inode *, struct file*, unsigned int, unsigned long);

};


#endif
