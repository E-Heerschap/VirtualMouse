//Implements the BUS mouse driver protocol for VM mouse.
//Author: Edwin Heerschap

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include "../vmdebug.h"
#include "../vmtypes.h"

static int busOpen(struct inode * in, struct file * filp)
{
	printk(KERN_ALERT "BUS PROTOCOL OPENED!\n");
	return 0;
}

static struct file_operations fopsBUS = {
	
	.owner = THIS_MODULE,
	.open = busOpen
	
};


static void cleanup(struct vmProtocol * protocol)
{
	kfree(protocol);
}

struct vmProtocol * vmBUSProtocolFactory(void) 
{
	
	struct vmProtocol* protocol;
	protocol = (struct vmProtocol *) kmalloc(sizeof(struct vmProtocol), GFP_KERNEL);
	if (!protocol) {
		
		VM_DEBUG("Failed to allocate protocol memory space!\n");
		return NULL;

	}
	memset(protocol, 0, sizeof(struct vmProtocol));
	protocol->name = "BUS";
	protocol->fops = &fopsBUS;
	protocol->cleanup = cleanup;
	return protocol;
}
