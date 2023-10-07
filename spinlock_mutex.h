//
// Created by asd on 14-Aug-23.
//

#ifndef LOCKFREE_SPINLOCK_MUTEX_H
#define LOCKFREE_SPINLOCK_MUTEX_H

#include <memory>

class spinlock_mutex {
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            __builtin_ia32_pause();
        } //spin
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

static inline void spin_pause() {
#if defined(__GNUC__) || defined(__GNUG__)
    __builtin_ia32_pause();
#elif defined(_MSC_VER)
    _mm_pause();
#endif
}

#endif //LOCKFREE_SPINLOCK_MUTEX_H
