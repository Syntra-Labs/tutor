# 003: O(1) Intrusive LRU Cache

## Specification & Contract

Design and implement a Least Recently Used (LRU) cache supporting $O(1)$ operations without heap fragmentation on hits.

### Requirements:
1. `std::optional<V> get(const K& key)`:
   - Returns the cached value if `key` exists and moves the entry to the front (most recently used).
   - Returns `std::nullopt` if `key` does not exist.
2. `void put(const K& key, const V& value)`:
   - Updates value if key exists, promoting it to the front.
   - Inserts new key-value pair. If capacity is exceeded, evicts the least recently used key.
3. Both operations must run in strictly $O(1)$ average time complexity.
