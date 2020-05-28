//Implements the BUS mouse driver protocol for VM mouse.
//Current implementation is assuming only 1 process reads the from the mouse driver.
//TODO implement multi-process mouse reading :)
//Author: Edwin Heerschap

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/sched/signal.h>
#include "../vmdebug.h"
#include "../vmtypes.h"
#include "../vmioctl.h"

#define VM_PROTOCOL_BUS_BUFFER_LEN 1024

/**
 * This should be accessed via container_of macro from the protocol pointer
 * stored in file->private_data->protocol.
 * Or, use the vmbus_get_protocol macro
 */
struct busProtocol {

    struct vmProtocol protocol;
    wait_queue_head_t readQueue;
    struct vmMouseEvent eventBuffer[VM_PROTOCOL_BUS_BUFFER_LEN]; //Wraps once end is reached overwriting previous
    unsigned int eventBufferReadPos;
    //Should always be at least ((eventBufferReadPos + 1) % VM_PROTOCOL_BUS_BUFFER_LEN) in buffer loop.
    //eventBufferReadPos != eventBufferWritePos must ALWAYS be true.
    unsigned int eventBufferWritePos;
    spinlock_t writeLock;
    struct fasync_struct* fasync;

};

static inline unsigned int getIncrement(unsigned int * num) {
    return (*num + 1) % VM_PROTOCOL_BUS_BUFFER_LEN;
}

static u8 incrementRead(struct busProtocol * protocol) {

    if (getIncrement(&protocol->eventBufferReadPos) == protocol->eventBufferWritePos) {
        return 1;
    } else {
        protocol->eventBufferReadPos = getIncrement(&protocol->eventBufferReadPos);
        return 0;
    }

}

static u8 incrementWrite(struct busProtocol* protocol) {

    if (getIncrement(&protocol->eventBufferWritePos) == protocol->eventBufferReadPos) {
        return 1;
    } else {
        protocol->eventBufferWritePos = getIncrement(&protocol->eventBufferWritePos);
        return 0;
    }

}

static inline struct vmMouseEvent * getCurrentEvent(struct busProtocol * protocol) {
    //readPosition set to 0 at initialization.
    return &protocol->eventBuffer[protocol->eventBufferReadPos];
}

static inline unsigned int eventOccured(struct busProtocol * protocol) {
    return protocol->eventBufferReadPos != (protocol->eventBufferWritePos - 1) % VM_PROTOCOL_BUS_BUFFER_LEN;
}

#define VM_BUS_GET_FILP_PROTOCOL(file) ({struct vmDevice* device = (struct vmDevice*) file->private_data; \
container_of(device->protocol, struct busProtocol, protocol);})

/**
 * Assuming only 1 process is reading the mouse. Otherwise mouse event reading
 * will be inaccurate. This will be changed in a future version. Current
 * implementation is for prototyping purposes.
 * @param filp
 * @param __user
 * @param size
 * @param off
 * @return
 */
static ssize_t busRead (struct file * filp, char* __user buf, size_t size,
		loff_t* off) {

    VM_DEBUGVARS("PROC PID: %d", current->pid);

    struct busProtocol* protocol = VM_BUS_GET_FILP_PROTOCOL(filp);

    struct vmMouseEvent* event;
    char byteData[3] = {0,0,0};
    DEFINE_WAIT(wait);
    set_current_state(TASK_INTERRUPTIBLE);

    VM_DEBUG("BUS protocol being read\n");

    if (size < 3) { //BUS protocol requires 3 byte reading.
        return -EINVAL;
    }

    while(!eventOccured(protocol)) {

        if (filp->f_flags & O_NDELAY) { // If no blocking is set on read
            return -EWOULDBLOCK;
        }

        if (signal_pending(current)) {
            return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
        }
        prepare_to_wait(&protocol->readQueue, &wait, TASK_INTERRUPTIBLE);

        schedule();

        set_current_state(TASK_INTERRUPTIBLE);
    }

    set_current_state(TASK_RUNNING);
    event = getCurrentEvent(protocol);

    if (event->dx == 0 || event->dy == 0) {
        if (incrementRead(protocol)) {
            VM_DEBUG("Read failed to increment!");
        }
        event = getCurrentEvent(protocol);
    }

    VM_DEBUGVARS("X Y: %d %d", event->dx, event->dy);

    //x axis
    byteData[1] = (event->dx > 127 ? 127 : event->dx );
    if (event->dx > 127) {
        VM_DEBUG("dx-=127\n");
        event->dx -= 127;
    } else {
        VM_DEBUG("dx=0\n");
        event->dx = 0;
    }
    //y axis
    byteData[2] = (event->dy > 127 ? 127 : event->dy);
    if (event->dy > 127) {
        VM_DEBUG("dy-=127\n");
        event->dy -= 127 ;
    } else {
        VM_DEBUG("dy=0\n");
        event->dy = 0;
    }

    //Buttons
    event->buttons = event->buttons | (unsigned) 0x80; //Setting the 128 bit
    byteData[0] = event->buttons;

    VM_DEBUGVARS("DATA TO COPY %x %x %x", byteData[0], byteData[1], byteData[2]);

    if(copy_to_user(buf, &byteData, 3)) {
        VM_DEBUG("COPY TO USER FAILED\n");
    } else {
        VM_DEBUG("COPY TO USER PASSED!\n");
    }

    int n = 0;
    if(size > 3) {
        VM_DEBUG("REMAINING BYTES EXIST\n");
        for(n = 3; n < size; n++) {
            if(put_user((char) 0x00, buf+n)) {
                VM_DEBUG("FAILED TO FILL REMAINING BYTES\n");
                return -EFAULT;
            };
        }
    }

    /*
    //Still runs if VM_DEBUG_ENABLE is not set, just wont print on failure :)
    VM_DEBUGIF(copy_to_user(buf, &byteData, sizeof(char[3])),
            "Failed to copy data to user space from BUS driver!");
            */

    return size;

}

static int fasync_mouse(int fd, struct file *filp, int on){
    VM_DEBUG("FASYNC");
    struct busProtocol* protocol = VM_BUS_GET_FILP_PROTOCOL(filp);
    int retval = fasync_helper(fd, filp, on, &protocol->fasync);
    if (retval<0) return retval;
    return 0;
}

static __poll_t busPoll(struct file * filp, struct poll_table_struct * pts) {
    VM_DEBUG("BUS POLL!\n");
    struct busProtocol * protocol = VM_BUS_GET_FILP_PROTOCOL(filp);
    poll_wait(filp, &protocol->readQueue, pts);

    if(eventOccured(protocol)) {
        VM_DEBUG("BUS POLL READ RETURN\n");
        return (POLLIN | POLLRDNORM);
    }

    return 0;

}

static int busOpen(struct inode * in, struct file * filp)
{
    printk(KERN_ALERT "BUS PROTOCOL OPENED!\n");
    return 0;
}

static int busClose(struct inode *inode, struct file *file){
    fasync_mouse(-1, file, 0);
    return 0;
}

static unsigned long spinLockFlags;

/**
 *
 * @param filp file* of the access device
 * @return IOCTL data
 */
static long writeMouseEvent(const struct file* filp, const struct vmMouseEvent* event) {

    struct busProtocol * protocol = VM_BUS_GET_FILP_PROTOCOL(filp);
    unsigned int newWritePos;
    //If locked and no delay flag set, return error would block.
    if (spin_is_locked(&protocol->writeLock) && (filp->f_flags& O_NDELAY)) {
        return -EWOULDBLOCK;
    }
    spin_lock_irqsave(&protocol->writeLock, spinLockFlags);

    protocol->eventBuffer[protocol->eventBufferWritePos] = *event;
    newWritePos = (protocol->eventBufferWritePos + 1) % VM_PROTOCOL_BUS_BUFFER_LEN;
    if (newWritePos == protocol->eventBufferReadPos) { // ints are considered atmoic types. Shouldn't be racy.
        printk(KERN_ALERT "Virtual mouse BUS protocol write position caught read position! Not writing event!");
        return -ENOMEM;
    } else {
        protocol->eventBufferWritePos = newWritePos;
    }

    spin_unlock_irqrestore(&protocol->writeLock, spinLockFlags);
    kill_fasync(&protocol->fasync, SIGIO, POLL_IN);
    wake_up_interruptible(&protocol->readQueue);

    return 0;

}

static long busIOCTL(struct file* filp, unsigned int ioctlNum, unsigned long ptr) {

    struct vmMouseEvent event;
    switch(ioctlNum) {
        case(VM_IOCTL_MOUSE_EVENT):
            VM_DEBUG("Mouse move trigged\n")

            if(copy_from_user(&event, (void*) ptr, sizeof(struct vmMouseEvent))) {
                VM_DEBUG("Copying mouse event from user space failed!\n")
                return -1;
            }

            //TODO Validate event?

            return writeMouseEvent(filp, &event);

            break;
        case(VM_IOCTL_CLEAR_EVENT):
            VM_DEBUG("Flush event triggered\n")

            return 0;
            break;
        default:
            printk(KERN_ALERT "Protocol default reached :(\n");
            return -ENOIOCTLCMD;
    }

    return -ENOIOCTLCMD;

}

static struct file_operations fopsBUS = {
	
	.owner = THIS_MODULE,
	.open = busOpen,
	.release = busClose,
 	.read = busRead,
 	.unlocked_ioctl = busIOCTL,
 	.fasync = fasync_mouse,
    .poll = busPoll
};


static void cleanup(struct vmProtocol * protocol)
{
    struct busProtocol* busProtocol = container_of(protocol, struct busProtocol, protocol);
	kfree(busProtocol);
}

struct vmProtocol * vmBUSProtocolFactory(void) 
{

	struct busProtocol* busProtocol;
	busProtocol = (struct busProtocol *) kmalloc(sizeof(struct busProtocol), GFP_KERNEL);
	if (!busProtocol) {
		
		VM_DEBUG("Failed to allocate protocol memory space!\n");
		return NULL;

	}

	memset(busProtocol, 0, sizeof(struct busProtocol));
	busProtocol->protocol.name = "BUS";
	busProtocol->protocol.fops = &fopsBUS;
	busProtocol->protocol.cleanup = cleanup;
	init_waitqueue_head(&busProtocol->readQueue);
    spin_lock_init(&busProtocol->writeLock);
    busProtocol->eventBufferReadPos = 0;
    busProtocol->eventBufferWritePos = 1;

	return &busProtocol->protocol;

}
