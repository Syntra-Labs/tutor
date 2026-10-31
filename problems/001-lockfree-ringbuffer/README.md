# 001: SPSC Lock-Free Ring Buffer

## Specification & Contract

Implement a Single-Producer Single-Consumer (SPSC) lock-free bounded FIFO queue in C++20.

### Requirements:
1. **Thread Safety**: Exactly one producer thread and one consumer thread may access the queue concurrently without locks or mutexes.
2. **Fixed Capacity**: The capacity $N$ is fixed at construction and guaranteed to be a power of two.
3. **Operations**:
   - `bool push(const T& item)`: Inserts `item` at the tail. Returns `false` if the buffer is full.
   - `bool pop(T& item)`: Extracts an item from the head. Returns `false` if the buffer is empty.
   - `size_t size() const`: Returns the approximate number of items currently in the buffer.
   - `bool empty() const` / `bool full() const`.
4. **Memory Ordering**:
   - Use atomic indices (`std::atomic<size_t>`) with non-seq-cst semantics (`memory_order_acquire`, `memory_order_release`, `memory_order_relaxed`) to ensure strict cache synchronization and minimal bus traffic.
