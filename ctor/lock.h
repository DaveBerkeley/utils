#if ! defined(__LOCK_H__)

#define __LOCK_H__

struct Lock;

struct Lock* lock_init();
void lock_lock(struct Lock *lock);
void lock_unlock(struct Lock *lock);

#endif // __LOCK_H__

