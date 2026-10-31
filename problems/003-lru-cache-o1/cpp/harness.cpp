#include <cassert>
#include <iostream>
#include <string>

#include "solution.hpp"

int main() {
    std::cout << "=== Running LRU Cache Test Suite ===\n";

    LruCache<int, std::string> cache(2);

    cache.put(1, "one");
    cache.put(2, "two");

    assert(cache.get(1).value_or("") == "one"); // promotes 1, making 2 LRU

    cache.put(3, "three"); // evicts 2

    assert(cache.get(2) == std::nullopt);
    assert(cache.get(3).value_or("") == "three");
    assert(cache.get(1).value_or("") == "one");

    std::cout << "  • Test 1: Eviction and promotion invariants ... [PASS]\n";
    std::cout << "All LRU Cache tests passed successfully!\n";
    return 0;
}
