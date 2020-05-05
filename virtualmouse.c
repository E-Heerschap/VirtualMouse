//VirtualMouse - A driver to emulate a BUS protocol mouse for the linux kernel.
//Author: Edwin Heerschap
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include "vmlock.h"

#define MINOR_COUNT 1
#define INSTRUC_BUF_LEN 10
#define BASE_MINOR 0;
#define DRIVER_NAME "Virtual Mouse"

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
	unsigned short int minor;
	struct vmInstruction intstruction[INSTRUC_BUF_LEN];
	struct vmLock;
	dev_t dev;
};


static ssize_t vmRead(struct file* filp, char __user* buf, size_t count, loff_t* offset)
{
	
}

static ssize_t vmWrite(struct file* filp, const char __user* buf, size_t count, loff_t* offset)
{



}

static __poll_t vmPoll(struct file * filp, struct poll_table_struct * pts) 
{
	
}

static int open (struct inode * in, struct file * filp) 
{
		

}

static release (struct inode * in, struct file * filp)
{

}

int virtualMouseInit(void)
{

	int alloc_chrdev_region(dev, BASE_MINOR, MINOR_COUNT, DRIVER_NAME);
	return 0;

}

void virtualMouseExit(void)
{

}

module_init(virtualMouseInit);
module_exit(virtualMouseExit);
