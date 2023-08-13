//
// Created by asd on 13-Aug-23.
//

#ifndef LOCKFREE_FIXEDPOOL_H
#define LOCKFREE_FIXEDPOOL_H

#include <memory>
#include <atomic>


template<class T, size_t num_chunks = 1 << 16>
class SimpleFixedPool {
private:
    struct Chunk {
        Chunk* next;
    };

    const size_t type_size {sizeof(T)};
    std::shared_ptr<uint8_t> mem_start;
    std::atomic<Chunk*> next_free;

public:
    using value_type = T;
    using pointer = T*;

    template<class U>
    struct rebind {
        using other = SimpleFixedPool<U, num_chunks>;
    };

    SimpleFixedPool(SimpleFixedPool &&other) = delete;
    SimpleFixedPool(const SimpleFixedPool &other) = delete;
    SimpleFixedPool& operator=(SimpleFixedPool &&other) = delete;
    SimpleFixedPool& operator=(const SimpleFixedPool &other) = delete;
    ~SimpleFixedPool() = default;

    SimpleFixedPool() noexcept : mem_start(new uint8_t[num_chunks*std::max(type_size, sizeof(Chunk))]) {
        Chunk* start = reinterpret_cast<Chunk*>(mem_start.get());
        next_free.store(start);


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

        if (!ptr) {
            throw std::bad_alloc();
        }

        return reinterpret_cast<pointer>(ptr);
    }

    inline void deallocate(pointer ptr, size_t n) {
        if (n != 1) {
            throw std::bad_alloc();
        }

        push(reinterpret_cast<Chunk*>(ptr));
    }

    inline Chunk* pop() noexcept {
        auto tmp = next_free.load();
        //ABA problem
        while (tmp && !next_free.compare_exchange_weak(tmp, tmp->next));
        return tmp;
    }

    inline void push(Chunk* ptr) noexcept {
        ptr->next = next_free.load();
        while (!next_free.compare_exchange_weak(ptr->next, ptr));
    }
};

#endif //LOCKFREE_FIXEDPOOL_H
