#pragma once

#include <atomic>
#include <vector>
#include <cstddef>
#include <optional>

template <typename T, size_t Capacity = 1024>
class SpscRingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");

public:
    SpscRingBuffer() : head_(0), tail_(0) {
        buffer_.resize(Capacity);
    }

    // Push item into buffer. Returns true if successful, false if full.
    bool push(const T& item) {
        // TODO: Implement lock-free push using appropriate memory orderings
        (void)item;
        return false;
    }

    // Pop item from buffer. Returns true if successful, false if empty.
    bool pop(T& item) {
        // TODO: Implement lock-free pop using appropriate memory orderings
        (void)item;
        return false;
    }

    [[nodiscard]] bool empty() const noexcept {
        return head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] size_t size() const noexcept {
        size_t t = tail_.load(std::memory_order_relaxed);
        size_t h = head_.load(std::memory_order_relaxed);
        return (t >= h) ? (t - h) : (Capacity - (h - t));
    }

private:
    std::vector<T> buffer_;
    alignas(64) std::atomic<size_t> tail_; // Written by producer
    alignas(64) std::atomic<size_t> head_; // Written by consumer
};
