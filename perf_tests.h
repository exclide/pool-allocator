//
// Created by asd on 14-Aug-23.
//

#ifndef LOCKFREE_PERF_TESTS_H
#define LOCKFREE_PERF_TESTS_H

#include <benchmark/benchmark.h>
#include "FixedPool.h"
#include "FixedPoolLock.h"
#include <list>

SimpleFixedPool<int> lk_pool;
SimpleFixedPoolLock<int, spinlock_mutex> spin_pool;
SimpleFixedPoolLock<int> wait_pool;

static void LockFreePool(benchmark::State& state) {
    for (auto _ : state) {
        auto ptr = lk_pool.allocate(1);
        lk_pool.deallocate(ptr, 1);
    }
}

static void LockPoolSpin(benchmark::State& state) {
    for (auto _ : state) {
        auto ptr = spin_pool.allocate(1);
        spin_pool.deallocate(ptr, 1);
    }
}

static void LockPoolWait(benchmark::State& state) {
    for (auto _ : state) {
        auto ptr = wait_pool.allocate(1);
        wait_pool.deallocate(ptr, 1);
    }
}


BENCHMARK(LockFreePool)->Iterations(10000)->Threads(16);
BENCHMARK(LockPoolSpin)->Iterations(10000)->Threads(16);
BENCHMARK(LockPoolWait)->Iterations(10000)->Threads(16);

#endif //LOCKFREE_PERF_TESTS_H
