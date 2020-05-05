//Contains system for the VM locks. VM locks are locking mechanisms for handling
//write operaitons to the virtual mice. This was abstracted instead of 
//directly implementing spinlocks incase future users want to implement
//a different locking mechanisim.
//Author: Edwin Heerschap


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <asm/spinlock.h>
#include <linux/slab.h>

struct vmLock {

	int (*canRead) (struct vmLock *);
	int (*canWrite) (struct vmLock *);
	int (*isLocked) (struct vmLock *);
	void(*readLock) (struct vmLock *);
	void (*writeLock) (struct vmLock *);
	void (*rwLock) (struct vmLock *);
	void (*readUnlock) (struct vmLock *);
	void (*writeUnlock) (struct vmLock *);
	void (*rwUnlock) (struct vmLock *);
	void (*cleanup) (struct vmLock *);
	void *lock;

};


//////////////////////////////////////////////
// The following is a simple spinlock implementation
// of the vmLock struct.
/////////////////////////////////////////////

void vmSpinlockLock(struct vmLock* vml)
{

	return spin_lock_irq((spinlock_t*) vml->lock);

}

void vmSpinlockUnlock(struct vmLock* vml)
{
	spin_unlock_irq((spinlock_t *)vml->lock);
}

int vmSpinlockCan(struct vmLock* vml) 
{
	
	int i = spin_is_locked((spinlock_t*) vml->lock);
	return i;
	
}

void vmSpinlockClean(struct vmLock* vml)
{
	
	kfree(vml->lock);

}

int isLocked(struct vmLock* vml)
{
	int i = spin_is_locked((spinlock_t *) vml->lock);
	return i;
}

//Creates a very simplified spinlock oriented vmLock struct
//which uses the same lock for read and write purposes.
struct vmLock spinlockBuilder(void) 
{

	struct vmLock vml;
	vml.canRead = vmSpinlockCan;
	vml.canWrite = vmSpinlockCan;
	vml.isLocked = isLocked;
	vml.readLock = vmSpinlockLock;
	vml.writeLock = vmSpinlockLock;
	vml.rwLock = vmSpinlockLock;
	vml.readUnlock = vmSpinlockUnlock;
	vml.writeUnlock = vmSpinlockUnlock;
	vml.rwUnlock = vmSpinlockUnlock;
	kmalloc(sizeof(spinlock_t), GFP_KERNEL);
	DEFINE_SPINLOCK(spinlock);
	*((spinlock_t *) vml.lock) = spinlock;
	return vml;

};
