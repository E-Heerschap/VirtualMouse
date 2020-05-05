//Contains declarations for VirtualHideout locking mechanisms
// Author: Edwin Heerschap

#ifndef VM_LOCK_H
#define VM_LOCK_H

struct vmLock; //Defined in vmlock.c
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
struct vmLock spinlockBuilder(void);

#endif
