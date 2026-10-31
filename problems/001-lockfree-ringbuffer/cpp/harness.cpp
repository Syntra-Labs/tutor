#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "solution.hpp"

void test_basic_fifo() {
    SpscRingBuffer<int, 4> q;
    assert(q.empty());
    assert(q.push(10));
    assert(q.push(20));
    assert(q.push(30));
    assert(!q.push(40)); // Capacity is 4, standard FIFO keeps 1 slot open to distinguish full vs empty

    int val = 0;
    assert(q.pop(val) && val == 10);
    assert(q.pop(val) && val == 20);
    assert(q.push(50));
    assert(q.pop(val) && val == 30);
    assert(q.pop(val) && val == 50);
    assert(!q.pop(val)); // Empty
    assert(q.empty());
    std::cout << "  • Test 1: Basic single-threaded FIFO semantics ... [PASS]\n";
}

void test_concurrent_stress() {
    constexpr size_t TOTAL = 100000;
    SpscRingBuffer<size_t, 1024> q;

    std::vector<size_t> received;
    received.reserve(TOTAL);

    std::thread producer([&]() {
        for (size_t i = 0; i < TOTAL; ++i) {
            while (!q.push(i)) {
                std::this_thread::yield();
            }
        }
    });

    std::thread consumer([&]() {
        size_t val = 0;
        for (size_t i = 0; i < TOTAL; ++i) {
            while (!q.pop(val)) {
                std::this_thread::yield();
            }
            received.push_back(val);
        }
    });

    producer.join();
    consumer.join();

    assert(received.size() == TOTAL);
    for (size_t i = 0; i < TOTAL; ++i) {
        assert(received[i] == i);
    }
    std::cout << "  • Test 2: Concurrent multi-threaded stress (100k items) ... [PASS]\n";
}

int main() {
    std::cout << "=== Running SpscRingBuffer Test Suite ===\n";
    try {
        test_basic_fifo();
        test_concurrent_stress();
        std::cout << "All SpscRingBuffer test suites passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failure: " << e.what() << "\n";
        return 1;
    }
}
