//Implements the default access used by vmDevices.
//all access allows all requests to pass to the protocol. 
//Author: Edwin Heerschap

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>
#include "../vmdebug.h"
#include "../vmtypes.h"

#define VM_ACCESS_ALL_NAME "all"

static struct file_operations fops = {
	
	// Default behaviour is a pass
	.owner = THIS_MODULE,
	.llseek = NULL,
	.read = NULL,
	.write = NULL,
	.unlocked_ioctl = NULL,
	.mmap = NULL,
	.open = NULL,
	.flush = NULL,
	.release = NULL,
	.fsync = NULL,
	.fasync = NULL,
	.lock = NULL,

};

static void cleanup(struct vmAccess* access) {

	kfree(access);

}

/**
 * Creates an "all" access on the heap. 
 */
struct vmAccess* allAccessFactory(void) 
{
	struct vmAccess* access;
	access = (struct vmAccess *) kmalloc(sizeof(struct vmAccess),
		       	GFP_KERNEL);
	memset(access, 0, sizeof(struct vmAccess));
	
	access->name = VM_ACCESS_ALL_NAME;
	access->fops = &fops;
	access->cleanup = cleanup;

	return access;
}
