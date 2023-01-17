#include <stdatomic.h>

typedef struct {
    atomic_flag lock;
} mutex_t;

typedef struct {
    atomic_int value;
} semaphore;

typedef struct {
    atomic_flag lock;
    atomic_int readers;
} rwlock_t;

void mutex_init(mutex_t *m) {
    atomic_flag_clear(&m->lock);
}

void mutex_lock(mutex_t *m) {
    while (atomic_flag_test_and_set(&m->lock)) {};
}

void mutex_unlock(mutex_t *m) {
    atomic_flag_clear(&m->lock);
}

void semaphore_init(semaphore *sem, int initial_value) {
    atomic_init(&sem->value, initial_value);
}

void semaphore_wait(semaphore *sem) {
    while (true)
    {
        int expected = atomic_load(&sem->value);
        if (expected == 0) {
           
        }
        else if (atomic_compare_exchange_weak_explicit(&sem->value, &expected, expected - 1, memory_order_acquire, memory_order_relaxed)) {
            break;
        }
    }
}

void semaphore_post(semaphore *sem) {
    atomic_fetch_add_explicit(&sem->value, 1, memory_order_release);
}



void rwlock_init(rwlock_t *rw) {
    atomic_flag_clear(&rw->lock);
    atomic_init(&rw->readers, 0);
}

void rwlock_rdlock(rwlock_t *rw) {
    while (atomic_flag_test_and_set(&rw->lock)) {};
    atomic_fetch_add(&rw->readers, 1);
    atomic_flag_clear(&rw->lock);
}

void rwlock_wrlock(rwlock_t *rw) {
    while (atomic_flag_test_and_set(&rw->lock)) {};
    while (atomic_load(&rw->readers) != 0) {
        atomic_flag_clear(&rw->lock);
        while (atomic_flag_test_and_set(&rw->lock)) {};
    }
}

void rwlock_unlock(rwlock_t *rw) {
    if (atomic_load(&rw->readers) > 0) {
        atomic_fetch_sub(&rw->readers, 1);
    } else {
        atomic_flag_clear(&rw->lock);
    }
}

