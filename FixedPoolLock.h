//
// Created by asd on 14-Aug-23.
//

#ifndef LOCKFREE_FIXEDPOOLLOCK_H
#define LOCKFREE_FIXEDPOOLLOCK_H

#include <memory>
#include <atomic>
#include <mutex>
#include "spinlock_mutex.h"


template<class T, class Mutex = std::mutex, size_t num_chunks = 1 << 16>
class SimpleFixedPoolLock {
private:
    struct Chunk {
        Chunk* next;
    };

    const size_t type_size {sizeof(T)};
    std::shared_ptr<uint8_t> mem_start;
    Chunk* next_free;
    Mutex mutex;

public:
    using value_type = T;
    using pointer = T*;

    template<class U>
    struct rebind {
        using other = SimpleFixedPoolLock<U, Mutex, num_chunks>;
    };

    SimpleFixedPoolLock(SimpleFixedPoolLock &&other) = delete;
    SimpleFixedPoolLock(const SimpleFixedPoolLock &other) = delete;
    SimpleFixedPoolLock& operator=(SimpleFixedPoolLock &&other) = delete;
    SimpleFixedPoolLock& operator=(const SimpleFixedPoolLock &other) = delete;
    ~SimpleFixedPoolLock() = default;

    SimpleFixedPoolLock() noexcept : mem_start(new uint8_t[num_chunks*std::max(type_size, sizeof(Chunk))]) {
        Chunk* start = reinterpret_cast<Chunk*>(mem_start.get());
        next_free = start;

        for (int i = 0; i < num_chunks - 1; i++) {
            start->next = reinterpret_cast<Chunk*>(reinterpret_cast<uint8_t*>(start) + std::max(type_size, sizeof(Chunk)));
            start = start->next;
        }

        start->next = nullptr;
    }

    inline pointer allocate(size_t n) {
        if (n != 1 || !next_free) {
            throw std::bad_alloc();
        }

        auto ptr = pop();

        return reinterpret_cast<pointer>(ptr);
    }

    inline void deallocate(pointer ptr, size_t n) {
        if (n != 1) {
            throw std::bad_alloc();
        }

        push(reinterpret_cast<Chunk*>(ptr));
    }

    inline Chunk* pop() noexcept {
        std::scoped_lock<Mutex> lk(mutex);

        auto tmp = next_free;
        next_free = tmp->next;
        return tmp;
    }

    inline void push(Chunk* ptr) noexcept {
        std::scoped_lock<Mutex> lk(mutex);

        ptr->next = next_free;
        next_free = ptr;
    }
};

#endif //LOCKFREE_FIXEDPOOLLOCK_H
