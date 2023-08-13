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
        while (flag.test_and_set()); //spin
    }

    void unlock() {
        flag.clear();
    }
};

#endif //LOCKFREE_SPINLOCK_MUTEX_H
